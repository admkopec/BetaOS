//
//  pmap.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/30/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/pmap.h>
#include <i386/pmap_pcid.h>
#include <i386/vm_types.h>
#include <i386/cpuid.h>
#include <i386/cpu_data.h>
#include <i386/proc_reg.h>
#include <kernel/misc_protos.h>
#include <sys/cdefs.h>

#include <platform/platform.h>
#include <platform/boot.h>

uint64_t vm_last_addr;

struct pmap	kernel_pmap_store;
pmap_t		kernel_pmap;
bool        pmap_smep_enabled = false;
bool        pmap_smap_enabled = false;
bool        no_shared_cr3     = false;

uint32_t    vm_cache_geometry_colors;

unsigned pmap_memory_region_count;
unsigned pmap_memory_region_current;
pmap_memory_region_t pmap_memory_regions[PMAP_MEMORY_REGIONS_SIZE];
// Temp
uint32_t	vm_lopage_free_count = 0;
uint32_t	vm_lopage_free_limit = 0;
uint32_t	vm_lopage_lowater    = 0;
bool        vm_lopage_refill     = FALSE;
bool        vm_lopage_needed     = FALSE;

uint64_t	max_valid_dma_address = 0xffffffffffffffffULL;
ppnum_t		max_valid_low_ppnum   = 0xffffffff;

static int	nkpt;

extern long __stack_chk_guard[];

extern void pmap_pcid_configure();

extern	char			*first_avail;
extern	vm_offset_t		virtual_avail, virtual_end;
extern	pmap_paddr_t	avail_start,     avail_end;
extern  vm_offset_t		sHIB;
extern  vm_offset_t		eHIB;
extern  vm_offset_t		stext;
extern  vm_offset_t		etext;
extern  vm_offset_t		sdata, edata;
extern  vm_offset_t		sconstdata, econstdata;

extern void             *KPTphys;

void cpu_pmc_control(void *enablep) {
    boolean_t enable = *(boolean_t *)enablep;
    cpu_data_t	*cdp = current_cpu_datap();
    
    if (enable) {
        wrmsr64(0x38F, 0x70000000FULL);
        wrmsr64(0x38D, 0x333);
        set_cr4(get_cr4() | CR4_PCE);
        
    } else {
        wrmsr64(0x38F, 0);
        wrmsr64(0x38D, 0);
        set_cr4((get_cr4() & ~CR4_PCE));
    }
    cdp->cpu_fixed_pmcs_enabled = enable;
}

void
pmap_cpu_init(void) {
    cpu_data_t	*cdp = current_cpu_datap();
    /*
     * Here early in the life of a processor (from cpu_mode_init()).
     * Ensure global page feature is disabled at this point.
     */
    
    set_cr4(get_cr4() &~ CR4_PGE);
    
    /*
     * Initialize the per-cpu, TLB-related fields.
     */
    cdp->cpu_kernel_cr3 = kernel_pmap->pm_cr3;
    cdp->cpu_active_cr3 = kernel_pmap->pm_cr3;
    cdp->cpu_tlb_invalid = FALSE;
    cdp->cpu_task_map = TASK_MAP_64BIT;
    pmap_pcid_configure();
    if (cpuid_leaf7_features() & CPUID_LEAF7_FEATURE_SMEP) {
        //bool nsmep;
        //if (!Parse_boot_argn("-pmap_smep_disable", &nsmep, sizeof(nsmep))) {
            set_cr4(get_cr4() | CR4_SMEP);
            pmap_smep_enabled = TRUE;
        //}
    }
    if (cpuid_leaf7_features() & CPUID_LEAF7_FEATURE_SMAP) {
        //bool nsmap;
        //if (!Parse_boot_argn("-pmap_smap_disable", &nsmap, sizeof(nsmap))) {
            set_cr4(get_cr4() | CR4_SMAP);
            pmap_smap_enabled = TRUE;
        //}
    }
    
    if (cdp->cpu_fixed_pmcs_enabled) {
        bool enable = TRUE;
        cpu_pmc_control(&enable);
    }
}

void
pmap_bootstrap(__unused vm_offset_t	load_start, bool IA32e) {
#if NCOPY_WINDOWS > 0
    vm_offset_t	va;
    int i;
#endif
    assert(IA32e);
    
    vm_last_addr = VM_MAX_KERNEL_ADDRESS;	/* Set the highest address
                                             * known to VM */
    /*
     *	The kernel's pmap is statically allocated so we don't
     *	have to use pmap_create, which is unlikely to work
     *	correctly at this part of the boot sequence.
     */
    
    kernel_pmap = &kernel_pmap_store;
    kernel_pmap->ref_count   = 1;
    kernel_pmap->nx_enabled  = TRUE;
    kernel_pmap->pm_task_map = TASK_MAP_64BIT;
    //kernel_pmap->pm_obj = (vm_object_t) NULL;
    kernel_pmap->dirbase = (pd_entry_t *)((uintptr_t)IdlePTD);
    kernel_pmap->pm_pdpt = (pd_entry_t *) ((uintptr_t)IdlePDPT);
    kernel_pmap->pm_pml4 = IdlePML4;
    kernel_pmap->pm_cr3  = (uintptr_t)ID_MAP_VTOP(IdlePML4);
    kernel_pmap->pm_eptp = 0;
    pmap_pcid_initialize_kernel(kernel_pmap);
    
    
    
    current_cpu_datap()->cpu_kernel_cr3 = (uint64_t) kernel_pmap->pm_cr3;
    
    nkpt = NKPT;
    /*OSAddAtomic(NKPT,  &inuse_ptepages_count);
    OSAddAtomic64(NKPT,  &alloc_ptepages_count);
    bootstrap_wired_pages = NKPT;*/
    
    virtual_avail = (vm_offset_t)(VM_MIN_KERNEL_ADDRESS) + (vm_offset_t)first_avail;
    virtual_end   = (vm_offset_t)(VM_MAX_KERNEL_ADDRESS);
    
#if NCOPY_WINDOWS > 0
    /*
     * Reserve some special page table entries/VA space for temporary
     * mapping of pages.
     */
#define	SYSMAP(c, p, v, n)	\
v = (c)va; va += ((n)*INTEL_PGBYTES);
    
    va = virtual_avail;
    
    for (i=0; i<PMAP_NWINDOWS; i++) {
#if 1
        kprintf("trying to do SYSMAP idx %d %p\n", i,
                current_cpu_datap());
        kprintf("cpu_pmap %p\n", current_cpu_datap()->cpu_pmap);
        kprintf("mapwindow %p\n", current_cpu_datap()->cpu_pmap->mapwindow);
        kprintf("two stuff %p %p\n",
                (void *)(current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CMAP),
                (void *)(current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CADDR));
#endif
        SYSMAP(caddr_t,
               (current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CMAP),
               (current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CADDR),
               1);
        current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CMAP =
        &(current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CMAP_store);
        *current_cpu_datap()->cpu_pmap->mapwindow[i].prv_CMAP = 0;
    }
    
    /* DMAP user for debugger */
    SYSMAP(caddr_t, DMAP1, DADDR1, 1);
    SYSMAP(caddr_t, DMAP2, DADDR2, 1);  /* XXX temporary - can remove */
    
    virtual_avail = va;
#endif
    //if (!Parse_boot_argn("npvhash", &npvhashmask, sizeof (npvhashmask))) {
        //npvhashmask = ((NPVHASHBUCKETS) << pmap_scale_shift()) - 1;
        
    //}
    
    //npvhashbuckets = npvhashmask + 1;
    
    //if (0 != ((npvhashbuckets) & npvhashmask)) {
    //    panic("invalid hash %d, must be ((2^N)-1), using default %d\n", npvhashmask, NPVHASHMASK);
   // }
    
    /*simple_lock_init(&kernel_pmap->lock, 0);
    simple_lock_init(&pv_hashed_free_list_lock, 0);
    simple_lock_init(&pv_hashed_kern_free_list_lock, 0);
    simple_lock_init(&pv_hash_table_lock,0);
    simple_lock_init(&phys_backup_lock, 0);*/
    
    pmap_cpu_init();
    
    if (pmap_pcid_ncpus)
        kprintf("PMAP: PCID enabled\n");
    
    if (pmap_smep_enabled)
        kprintf("PMAP: Supervisor Mode Execute Protection enabled\n");
    if (pmap_smap_enabled)
        kprintf("PMAP: Supervisor Mode Access Protection enabled\n");
    
#if	DEBUG
    kprintf("Stack canary: 0x%lx\n", __stack_chk_guard[0]);
    //kprintf("early_random(): 0x%qx\n", early_random());
#endif
    //boolean_t ptmp;
    /* Check if the user has requested disabling stack or heap no-execute
     * enforcement. These are "const" variables; that qualifier is cast away
     * when altering them. The TEXT/DATA const sections are marked
     * write protected later in the kernel startup sequence, so altering
     * them is possible at this point, in pmap_bootstrap().
     */
    /*if (Parse_boot_argn("-pmap_disable_kheap_nx", &ptmp, sizeof(ptmp))) {
        boolean_t *pdknxp = (boolean_t *) &pmap_disable_kheap_nx;
        *pdknxp = TRUE;
    }
    
    if (Parse_boot_argn("-pmap_disable_kstack_nx", &ptmp, sizeof(ptmp))) {
        boolean_t *pdknhp = (boolean_t *) &pmap_disable_kstack_nx;
        *pdknhp = TRUE;
    }
    */
    boot_args *args = (boot_args *)Platform_state.bootArgs;
    if (args->efiMode == kBootArgsEfiMode32) {
        kprintf("EFI32: kernel virtual space limited to 4GB\n");
        virtual_end = VM_MAX_KERNEL_ADDRESS_EFI32;
    }
    kprintf("Kernel virtual space from 0x%lx to 0x%lx.\n", (long)KERNEL_BASE, (long)virtual_end);
    kprintf("Available physical space from 0x%llx to 0x%llx\n", avail_start, avail_end);
    
    /*
     * The -no_shared_cr3 boot-arg is a debugging feature (set by default
     * in the DEBUG kernel) to force the kernel to switch to its own map
     * (and cr3) when control is in kernelspace. The kernel's map does not
     * include (i.e. share) userspace so wild references will cause
     * a panic. Only copyin and copyout are exempt from this. 
     */
    //(void) Parse_boot_argn("-no_shared_cr3", &no_shared_cr3, sizeof (no_shared_cr3));
    if (no_shared_cr3)
        kprintf("Kernel not sharing user map\n");
    
#ifdef	PMAP_TRACES
    //if (Parse_boot_argn("-pmap_trace", &pmap_trace, sizeof (pmap_trace))) {
    //   kprintf("Kernel traces for pmap operations enabled\n");
    //}
#endif	/* PMAP_TRACES */
}
