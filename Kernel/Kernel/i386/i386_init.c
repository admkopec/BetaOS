//
//  i386_init.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/1/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <i386/cpu_data.h>
#include <i386/cpuid.h>
#include <i386/proc_reg.h>
#include <i386/mp.h>
#include <i386/asm.h>
#include <i386/pmap.h>
#include <i386/mp_desc.h>
#include <i386/vm_param.h>
#include <i386/locks.h>
#include <i386/lapic.h>
#include <i386/pal.h>
#include <i386/tsc.h>
#include <i386/fpu.h>
#include <i386/machine_routines.h>
#include <i386/cpu_threads.h>
#include <i386/thread_status.h>
#include <i386/rtclock.h>
#include <platform/platform.h>
#include <platform/boot.h>
#include <platform/device_tree.h>

#include <sys/cdefs.h>
#include "misc_protos.h"

extern void i386_init(void);
extern void i386_init_slave(void);
extern void clear_screen(void);

int master_cpu;

#if DEBUG
#define DBG(x...)   printf(x)
#else
#define DBG(x...)
#endif

int                 debug_task;
extern pmap_t       kernel_pmap;
static boot_args	*kernelBootArgs;  // Make it multiboot info not XNU args or leave it :)
extern int          disableConsoleOutput;
extern const char	version[];
extern const char	version_variant[];
extern int          nx_enabled;

uint64_t            physmap_base, physmap_max;

pd_entry_t          *KPTphys;
pd_entry_t          *IdlePTD;
pdpt_entry_t		*IdlePDPT;
pml4_entry_t		*IdlePML4;

char                *physfree;

extern pmap_paddr_t first_avail;
#undef bzero
/*
 * Note: ALLOCPAGES() can only be used safely within Idle_PTs_init()
 * due to the mutation of physfree.
 */
static void *
ALLOCPAGES(int npages) {
    unsigned long tmp = (unsigned long)physfree;
    bzero(physfree, npages * PAGE_SIZE);
    physfree += npages * PAGE_SIZE;
    tmp += VM_MIN_KERNEL_ADDRESS & ~LOW_4GB_MASK;
    return (void *)tmp;
}

static void
fillkpt(pt_entry_t *base, int prot, unsigned long src, int index, int count) {
    for (int i=0; i<count; i++) {
        printf("");
        base[index] = src | prot | INTEL_PTE_VALID;
        src += PAGE_SIZE;
        index++;
    }
}
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif
/* Temporary */
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
__attribute__((noreturn))
void __stack_chk_fail(void) {
    panic("Kernel stack memory corruption detected");
    for (; ;) { pal_stop_cpu(true); }
}

// Set up the physical mapping - NPHYSMAP GB of memory mapped at a high address
// NPHYSMAP is determined by the maximum supported RAM size plus 4GB to account
// the PCI hole (which is less 4GB but not more).

/* Compile-time guard: NPHYSMAP is capped to 256GiB, accounting for
 * randomisation
 */
extern int maxphymapsupported[NPHYSMAP <= (PTE_PER_PAGE/2) ? 1 : -1];

static void
physmap_init(void) {
    pt_entry_t *physmapL3 = ALLOCPAGES(1);
    struct {
        pt_entry_t entries[PTE_PER_PAGE];
    } * physmapL2 = ALLOCPAGES(NPHYSMAP);
    
    uint8_t  phys_random_L3 = /*early_random()*/ 1145125151236523357 & 0xFF;
    //                        ^^^^^^^^^^^^^^ Implement!
    /* Check NX support. Mark all levels of the PHYSMAP NX
     * to avoid granting executability via a single bit flip.
     */
    
    uint32_t reg[4];
    do_cpuid(0x80000000, reg);
    if (reg[eax] >= 0x80000001) {
        do_cpuid(0x80000001, reg);
        assert(reg[edx] & CPUID_EXTFEATURE_XD);
    }
    
    for(uint64_t i = 0; i < NPHYSMAP; i++) {
        physmapL3[i + phys_random_L3] =
        ((unsigned long)ID_MAP_VTOP(&physmapL2[i]))
        | INTEL_PTE_VALID
        | INTEL_PTE_NX
        | INTEL_PTE_WRITE;

        for(uint64_t j = 0; j < PTE_PER_PAGE; j++) {
            physmapL2[i].entries[j] =
           ((i * PTE_PER_PAGE + j) << PDSHIFT)
            | INTEL_PTE_PS
            | INTEL_PTE_VALID
            | INTEL_PTE_NX
            | INTEL_PTE_WRITE;
            if (j > 0x1FE) {       // Using the same hack as above :)
                printf("");
            }
        }
    }
    
    IdlePML4[KERNEL_PHYSMAP_PML4_INDEX] =
    ((unsigned long)ID_MAP_VTOP(physmapL3))
    | INTEL_PTE_VALID
    | INTEL_PTE_NX
    | INTEL_PTE_WRITE;
    
    physmap_base = KVADDR(KERNEL_PHYSMAP_PML4_INDEX, phys_random_L3, 0, 0);
    physmap_max  = physmap_base + NPHYSMAP * GB;
}

static void
descriptor_alias_init() {
    vm_offset_t	master_gdt_phys;
    vm_offset_t	master_gdt_alias_phys;
    vm_offset_t	master_idt_phys;
    vm_offset_t	master_idt_alias_phys;
    
    assert(((vm_offset_t)master_gdt   & PAGE_MASK) == 0);
    assert(((vm_offset_t)master_idt64 & PAGE_MASK) == 0);
    
    master_gdt_phys         = (vm_offset_t) ID_MAP_VTOP(master_gdt);
    master_idt_phys         = (vm_offset_t) ID_MAP_VTOP(master_idt64);
    master_gdt_alias_phys   = (vm_offset_t) ID_MAP_VTOP(MASTER_GDT_ALIAS);
    master_idt_alias_phys   = (vm_offset_t) ID_MAP_VTOP(MASTER_IDT_ALIAS);
    
    DBG("master_gdt_phys:        %p\n", (void *) master_gdt_phys);
    DBG("master_idt_phys:        %p\n", (void *) master_idt_phys);
    DBG("master_gdt_alias_phys:  %p\n", (void *) master_gdt_alias_phys);
    DBG("master_idt_alias_phys:  %p\n", (void *) master_idt_alias_phys);
    
    KPTphys[atop_kernel(master_gdt_alias_phys)] = master_gdt_phys | INTEL_PTE_VALID | INTEL_PTE_NX | INTEL_PTE_WRITE;
    printf(""); // Same hack as above :)
    KPTphys[atop_kernel(master_idt_alias_phys)] = master_idt_phys | INTEL_PTE_VALID | INTEL_PTE_NX;	/* read-only */
}
extern bool early;
static void
Idle_PTs_init(void) {
    /* Allocate the "idle" kernel page tables: */
    KPTphys  = ALLOCPAGES(NKPT);	/* level 1 */
    IdlePTD  = ALLOCPAGES(NPGPTD);	/* level 2 */
    IdlePDPT = ALLOCPAGES(1);		/* level 3 */
    IdlePML4 = ALLOCPAGES(1);		/* level 4 */
    
    // Fill the lowest level with everything up to physfree
    fillkpt(KPTphys, INTEL_PTE_WRITE, 0, 0, (int)(((unsigned long)physfree) >> PAGE_SHIFT));
    
    /* IdlePTD */
    fillkpt(IdlePTD, INTEL_PTE_WRITE, (unsigned long)ID_MAP_VTOP(KPTphys), 0, NKPT);
    
    // IdlePDPT entries
    fillkpt(IdlePDPT, INTEL_PTE_WRITE, (unsigned long)ID_MAP_VTOP(IdlePTD), 0, NPGPTD);
    
    // IdlePML4 single entry for kernel space.
    fillkpt(IdlePML4 + KERNEL_PML4_INDEX, INTEL_PTE_WRITE, (unsigned long)ID_MAP_VTOP(IdlePDPT), 0, 1);
    
    physmap_init();
    
    descriptor_alias_init();
    
    // Switch to the page tables..
    DBG("Before Switch....\n");
    early = true;
    set_cr3_raw((unsigned long)ID_MAP_VTOP(IdlePML4));
}

/*
 * vstart() is called in the natural mode (64bit for K64, 32 for K32)
 * on a set of bootstrap pagetables which use large, 2MB pages to map
 * all of physical memory in both. See idle_pt.c for details.
 *
 * In K64 this identity mapping is mirrored the top and bottom 512GB
 * slots of PML4.
 *
 * The bootstrap processor called with argument boot_args_start pointing to
 * the boot-args block. The kernel's (4K page) page tables are allocated and
 * initialized before switching to these.
 *
 * Non-bootstrap processors are called with argument boot_args_start NULL.
 * These processors switch immediately to the existing kernel page tables.
 */
extern void acpi(void);
void
vstart(vm_offset_t boot_args_start) {
    bool        is_boot_cpu = !(boot_args_start == 0);
    int         cpu;
    uint32_t	lphysfree;
    
    if (is_boot_cpu) {
        // Get startup parameters
        kernelBootArgs  = (boot_args *)boot_args_start;
        lphysfree       = kernelBootArgs->kaddr + kernelBootArgs->ksize;
        physfree        = (void *)(unsigned long)((lphysfree + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));

        #if DEBUG
        serial_init();
        can_use_serial = true;
        #endif
        
        kernelBootArgs = (boot_args *)ml_static_ptovirt(boot_args_start);
        DBG("i386_init(0x%lx) kernelBootArgs=%p\n", (unsigned long)boot_args_start, kernelBootArgs);
        Platform_init(false, kernelBootArgs);
        
        #ifdef DEBUG
        clear_screen();
        #endif
        acpi();
        
        DBG("revision      0x%X\n", kernelBootArgs->Revision);
        DBG("version       0x%X\n", kernelBootArgs->Version);
        DBG("command line  %s\n",   kernelBootArgs->CommandLine);
        DBG("memory map    0x%X\n", kernelBootArgs->MemoryMap);
        DBG("memory map sz 0x%X\n", kernelBootArgs->MemoryMapSize);
        DBG("kaddr         0x%X\n", kernelBootArgs->kaddr);
        DBG("ksize         0x%X\n", kernelBootArgs->ksize);
        DBG("bootargs: %p, &ksize: %p &kaddr: %p\n",
                                                     kernelBootArgs,
                                                    &kernelBootArgs->ksize,
                                                    &kernelBootArgs->kaddr);
        DBG("SMBIOS mem sz 0x%llx\n", kernelBootArgs->PhysicalMemorySize);
        DBG("EFI mode      %d\n",     kernelBootArgs->efiMode);
        DBG("Video BAR     0x%llX\n", kernelBootArgs->Video.v_baseAddr);
        DBG("Video Display %d\n",     kernelBootArgs->Video.v_display);
        DBG("Video rowbyte %d\n",     kernelBootArgs->Video.v_rowBytes);
        DBG("Video width   %d\n",     kernelBootArgs->Video.v_width);
        DBG("Video height  %d\n",     kernelBootArgs->Video.v_height);
        DBG("Video depth   %d\n",     kernelBootArgs->Video.v_depth);
        
        Idle_PTs_init();      // Using Boot Page Tables
        
        first_avail = (vm_offset_t)ID_MAP_VTOP(physfree);
        
        cpu = 0;
        cpu_data_alloc(true);
    } else {
        // Switch to kernel's page tables (from the Boot PTs)
        set_cr3_raw((unsigned long)ID_MAP_VTOP(IdlePML4));
        //Find our logical cpu number
        cpu = lapic_to_cpu[(LAPIC_READ(ID)>>24) & 0xFF];
        DBG("CPU: %d, GSBASE initial value: 0x%llx\n", cpu, rdmsr64(MSR_IA32_GS_BASE));
    }
    if(is_boot_cpu) {
        cpu_desc_init64(cpu_datap(cpu));
        cpu_desc_load64(cpu_datap(cpu));
    } if (is_boot_cpu)
        cpu_mode_init(current_cpu_datap()); // cpu_mode_init() will be invoked on the APs via i386_init_slave()
    x86_init_wrapper(is_boot_cpu ? (uintptr_t) i386_init : (uintptr_t) i386_init_slave, cpu_datap(cpu)->cpu_int_stack_top);
}
#include "acpi.h"
bool beforeInited = true;
extern bool enable;
extern bool use_screen_caching;
extern uint64_t Screen;
extern uintptr_t swift_isaMask;
extern RSDP_for_Swift RSDP_;
extern SMBIOS_for_Swift SMBIOS_;
extern SMBIOSHeader SmbiosHeaderForBiosSearch;
extern ACPI_2_Description RsdpForBiosSearch;
extern bool experimental;
void
i386_init(void) {
    //unsigned int max_mem;
    unsigned int max_mem_touse = 0;
    //unsigned int cpus = 0;
    //bool         fidn;
    bool         IA32e = true;
    tsc_init();
    rtclock_early_init();
    
    master_cpu = 0;
    cpu_init();
    
    i386_vm_init(max_mem_touse, IA32e, kernelBootArgs);
    use_screen_caching = true;
    enable = true;
    
    vm_offset_t v_physAddr = Platform_state.video.v_baseAddr & ~3;
    vm_offset_t v_newAddr  = 0;
    vm_size_t   fbsize     = 0;
    DBG("v_baseAddr = 0x%lx\n", Platform_state.video.v_baseAddr);
    if (!v_physAddr) {
        panic("v_physAddr == 0\n");
    } else {
        if (Platform_state.video.v_length != 0) {
            fbsize = (vm_size_t) round_page(Platform_state.video.v_length);
        } else {
            fbsize = (vm_size_t) round_page(Platform_state.video.v_height * Platform_state.video.v_rowBytes);
            Platform_state.video.v_length = (Platform_state.video.v_height * Platform_state.video.v_rowBytes);
        }
        unsigned int flags = VM_WIMG_IO;
        v_newAddr = io_map((vm_map_offset_t)v_physAddr, fbsize, flags);
        if (v_newAddr != 0) {
            Platform_state.video.v_baseAddr = v_newAddr + Platform_state.video.v_offset;
        } else {
            panic("Video Framebuffer allocation failed!\n");
        }
    }
    DBG("v_baseAddr = 0x%lx\n", Platform_state.video.v_baseAddr);
    early = false;
    Screen = (uintptr_t) malloc(fbsize);
    clear_screen();
    
    if (RSDP_.foundInBios) {
        RSDP_.RSDP = &RsdpForBiosSearch;
    } else if (RSDP_.OriginalAddress != 0) {
        RSDP_.RSDP     = (ACPI_2_Description *)io_map(((vm_offset_t)RSDP_.OriginalAddress & ~3), sizeof(ACPI_2_Description), VM_WIMG_IO);
    } if (SMBIOS_.foundInBios) {
        SMBIOS_.SMBIOS = &SmbiosHeaderForBiosSearch;
    } else if (SMBIOS_.OriginalAddress != 0) {
        SMBIOS_.SMBIOS = (SMBIOSHeader *)io_map(((vm_offset_t)SMBIOS_.OriginalAddress & ~3), sizeof(SMBIOSHeader), VM_WIMG_IO);
    }
    swift_isaMask = 0xF;
    init_fpu();
    enable_sse();
    //pmInit();
    
    kernelMain();
}

void
i386_init_slave(void) {
    printf("Init Slave!\n");
    for (; ;) {
        pal_hlt();
    }
    //i386_init();
}
