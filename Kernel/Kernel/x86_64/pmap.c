//
//  pmap.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/30/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <i386/pmap.h>
#include <i386/pmap_pcid.h>
#include <i386/pmap_internal.h>
#include <i386/vm_types.h>
#include <i386/cpuid.h>
#include <i386/cpu_data.h>
#include <i386/proc_reg.h>
#include <i386/vm_prot.h>
#include <kernel/misc_protos.h>
#include <sys/cdefs.h>

#include <platform/platform.h>
#include <platform/boot.h>

// Compile time assert
char pmap_cpu_data_assert[(((offsetof(cpu_data_t, cpu_tlb_invalid) - offsetof(cpu_data_t, cpu_active_cr3)) == 8) && (offsetof(cpu_data_t, cpu_active_cr3) % 64 == 0)) ? 1 : -1];
boolean_t pmap_trace = FALSE;
#ifdef DEBUG
boolean_t	no_shared_cr3 = TRUE;		/* TRUE for DEBUG by default */
#else
boolean_t   no_shared_cr3 = FALSE;
#endif

int nx_enabled = 1;                 /* enable no-execute protection */
//int allow_data_exec  = VM_ABI_32;	/* 32-bit apps may execute data by default, 64-bit apps may not */
int allow_stack_exec = 0;           /* No apps may execute from the stack by default */

const boolean_t cpu_64bit  = TRUE;

uint64_t max_preemption_latency_tsc = 0;

uint64_t vm_last_addr;
uint32_t npvhashmask = 0, npvhashbuckets = 0;

struct pmap	kernel_pmap_store;
pmap_t		kernel_pmap;

uint32_t    vm_cache_geometry_colors;

unsigned pmap_memory_region_count;
unsigned pmap_memory_region_current;
pmap_memory_region_t pmap_memory_regions[PMAP_MEMORY_REGIONS_SIZE];

unsigned int	inuse_ptepages_count  =  0;
long long       alloc_ptepages_count __attribute__((aligned(8))) = 0; /* aligned for atomic access */
unsigned int	bootstrap_wired_pages =  0;
int             pt_fake_zone_index    = -1;
extern 	long	NMIPI_acks;
boolean_t       kernel_text_ps_4K = TRUE;
boolean_t       wpkernel          = TRUE;
extern char     end;

// Temp
#define NPVHASHBUCKETS (4096)
#define NPVHASHMASK ((NPVHASHBUCKETS) - 1) /* MUST BE 2^N - 1 */
uint32_t	vm_lopage_free_count = 0;
uint32_t	vm_lopage_free_limit = 0;
uint32_t	vm_lopage_lowater    = 0;
bool        vm_lopage_refill     = FALSE;
bool        vm_lopage_needed     = FALSE;

uint64_t	max_valid_dma_address = 0xffffffffffffffffULL;
ppnum_t		max_valid_low_ppnum   = 0xffffffff;

static int	nkpt;

boolean_t	pmap_disable_kheap_nx  = FALSE;
boolean_t	pmap_disable_kstack_nx = FALSE;

extern long __stack_chk_guard[];

extern void pmap_pcid_configure(void);

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

extern uint64_t physmap_base, physmap_max;

bool pmap_smep_enabled = false;
bool pmap_smap_enabled = false;

// <TEMP>
void kernel_preempt_check(void) {
    bool          intr;
    unsigned long flags;
    
    assert(get_preemption_level() == 0);
    
    __asm__ volatile("pushf; pop	%0" :  "=r" (flags));
    
    intr = ((flags & EFL_IF) != 0);
    
    if ((*ast_pending() & AST_URGENT) && intr == true) {
        /*
         * can handle interrupts and preemptions
         * at this point
         */
        
        /*
         * now cause the PRE-EMPTION trap
         */
        __asm__ volatile ("int %0" :: "N" (T_PREEMPT));
    }
}
// </TEMP>

uint64_t kvtophys(vm_offset_t addr) {
    pmap_paddr_t pa;
    
    pa = ((pmap_paddr_t)pmap_find_phys(kernel_pmap, addr)) << INTEL_PGSHIFT;
    if (pa)
        pa |= (addr & INTEL_OFFMASK);
    
    return ((uint64_t)pa);
}

ppnum_t pmap_find_phys(pmap_t pmap, uint64_t va) {
    pt_entry_t	*ptp;
    pd_entry_t	*pdep;
    ppnum_t		ppn = 0;
    pd_entry_t	pde;
    pt_entry_t	pte;
    bool        is_ept;
    
    is_ept = is_ept_pmap(pmap);
    
    mp_disable_preemption();
    
    /* This refcount test is a band-aid--several infrastructural changes
     * are necessary to eliminate invocation of this routine from arbitrary
     * contexts.
     */
    
    if (!pmap->ref_count)
        goto pfp_exit;
    
    pdep = pmap_pde(pmap, va);
    
    if ((pdep != PD_ENTRY_NULL) && ((pde = *pdep) & PTE_VALID_MASK(is_ept))) {
        if (pde & PTE_PS) {
            ppn  = (ppnum_t) i386_btop(pte_to_pa(pde));
            ppn += (ppnum_t) ptenum(va);
        }
        else {
            ptp = pmap_pte(pmap, va);
            if ((PT_ENTRY_NULL != ptp) && (((pte = *ptp) & PTE_VALID_MASK(is_ept)) != 0)) {
                ppn = (ppnum_t) i386_btop(pte_to_pa(pte));
            }
        }
    }
pfp_exit:
    mp_enable_preemption();
    
    return ppn;
}

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
        bool nsmep;
        if (!Parse_boot_argn("-pmap_smep_disable", &nsmep, sizeof(nsmep))) {
            set_cr4(get_cr4() | CR4_SMEP);
            pmap_smep_enabled = TRUE;
        }
    }
    if (cpuid_leaf7_features() & CPUID_LEAF7_FEATURE_SMAP) {
        bool nsmap;
        if (!Parse_boot_argn("-pmap_smap_disable", &nsmap, sizeof(nsmap))) {
            set_cr4(get_cr4() | CR4_SMAP);
            pmap_smap_enabled = TRUE;
        }
    }
    
    if (cdp->cpu_fixed_pmcs_enabled) {
        bool enable = TRUE;
        cpu_pmc_control(&enable);
    }
}

static uint32_t pmap_scale_shift(void) {
    uint32_t scale = 0;
    if (sane_size <= 8*GB) {
        scale = (uint32_t)(sane_size / (2 * GB));
    } else if (sane_size <= 32*GB) {
        scale = 4 + (uint32_t)((sane_size - (8 * GB))/ (4 * GB));
    } else {
        scale = 10 + (uint32_t)MIN(4, ((sane_size - (32 * GB))/ (8 * GB)));
    }
    return scale;
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
    OSAddAtomic64(NKPT,  &alloc_ptepages_count);*/
    bootstrap_wired_pages = NKPT;
    
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
    if (!Parse_boot_argn("npvhash", &npvhashmask, sizeof (npvhashmask))) {
        npvhashmask = ((NPVHASHBUCKETS) << pmap_scale_shift()) - 1;
    }
    
    npvhashbuckets = npvhashmask + 1;
    
    if (0 != ((npvhashbuckets) & npvhashmask)) {
        panic("invalid hash %d, must be ((2^N)-1), using default %d\n", npvhashmask, NPVHASHMASK);
    }
    
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
    boolean_t ptmp;
    /* Check if the user has requested disabling stack or heap no-execute
     * enforcement. These are "const" variables; that qualifier is cast away
     * when altering them. The TEXT/DATA const sections are marked
     * write protected later in the kernel startup sequence, so altering
     * them is possible at this point, in pmap_bootstrap().
     */
    if (Parse_boot_argn("-pmap_disable_kheap_nx", &ptmp, sizeof(ptmp))) {
        boolean_t *pdknxp = (boolean_t *) &pmap_disable_kheap_nx;
        *pdknxp = TRUE;
    }
    
    if (Parse_boot_argn("-pmap_disable_kstack_nx", &ptmp, sizeof(ptmp))) {
        boolean_t *pdknhp = (boolean_t *) &pmap_disable_kstack_nx;
        *pdknhp = TRUE;
    }
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
    (void) Parse_boot_argn("-no_shared_cr3", &no_shared_cr3, sizeof (no_shared_cr3));
    if (no_shared_cr3)
        kprintf("Kernel not sharing user map\n");
    
#ifdef	PMAP_TRACES
    if (Parse_boot_argn("-pmap_trace", &pmap_trace, sizeof (pmap_trace))) {
       kprintf("Kernel traces for pmap operations enabled\n");
    }
#endif	/* PMAP_TRACES */
}

void
pmap_virtual_space(vm_offset_t *startp, vm_offset_t *endp) {
    *startp = virtual_avail;
    *endp = virtual_end;
}

vm_offset_t
pmap_map_bd(vm_offset_t virt, vm_map_offset_t start_addr, vm_map_offset_t end_addr, vm_prot_t prot, unsigned int flags) {
    pt_entry_t template;
    pt_entry_t *pte;
    vm_offset_t base = virt;
    template = pa_to_pte(start_addr) | INTEL_PTE_REF | INTEL_PTE_MOD | INTEL_PTE_WIRED | INTEL_PTE_VALID;
    
    if ((flags & (VM_MEM_NOT_CACHEABLE | VM_WIMG_USE_DEFAULT)) == VM_MEM_NOT_CACHEABLE) {
        template |= INTEL_PTE_NCACHE;
        if (!(flags & (VM_MEM_GUARDED)))
            template |= INTEL_PTE_PTA;
    }
    
    if ((prot & VM_PROT_EXECUTE) == 0) {
        template |= INTEL_PTE_NX;
    }
    
    if (prot & VM_PROT_WRITE) {
        template |= INTEL_PTE_WRITE;
    }
    
    while (start_addr < end_addr) {
        pte = pmap_pte(kernel_pmap, (vm_map_offset_t)virt);
        if (pte == PT_ENTRY_NULL) {
            panic("pmap_map_bd: Invalid kernel address!\n");
        }
        pmap_store_pte(pte, template);
        pte_increment_pa(template);
        virt += PAGE_SIZE;
        start_addr += PAGE_SIZE;
    }
    flush_tlb_raw();
    PMAP_UPDATE_TLBS(kernel_pmap, base, base + end_addr - start_addr);
    return virt;
}

void
pmap_flush_tlbs(pmap_t	pmap, vm_map_offset_t startv, vm_map_offset_t endv, __unused int options, __unused pmap_flush_context *pfc) {
    unsigned int	cpu;
    unsigned int	cpu_bit;
    //cpumask_t       cpus_to_signal = 0;
    unsigned int	my_cpu = cpu_number();
    pmap_paddr_t	pmap_cr3 = pmap->pm_cr3;
    boolean_t       flush_self = FALSE;
    __unused uint64_t        deadline;
    boolean_t       pmap_is_shared = (pmap->pm_shared || (pmap == kernel_pmap));
    boolean_t       need_global_flush = FALSE;
    __unused uint32_t        event_code;
    vm_map_offset_t	event_startv, event_endv;
    boolean_t       is_ept = is_ept_pmap(pmap);
    
    //assert((processor_avail_count < 2) || (ml_get_interrupts_enabled() && get_preemption_level() != 0));
    
    if (pmap == kernel_pmap) {
        //event_code = PMAP_CODE(PMAP__FLUSH_KERN_TLBS);
        //event_startv = VM_KERNEL_UNSLIDE_OR_PERM(startv);
        //event_endv = VM_KERNEL_UNSLIDE_OR_PERM(endv);
    } else if (is_ept) {
        //event_code = PMAP_CODE(PMAP__FLUSH_EPT);
        event_startv = startv;
        event_endv = endv;
    } else {
        //event_code = PMAP_CODE(PMAP__FLUSH_TLBS);
        event_startv = startv;
        event_endv = endv;
    }
    
    //PMAP_TRACE_CONSTANT(event_code | DBG_FUNC_START, VM_KERNEL_UNSLIDE_OR_PERM(pmap), options, event_startv, event_endv, 0);
    
    if (is_ept) {
        //mp_cpus_call(CPUMASK_ALL, ASYNC, invept, (void*)pmap->pm_eptp);
        goto out;
    }
    
    /*
     * Scan other cpus for matching active or task CR3.
     * For idle cpus (with no active map) we mark them invalid but
     * don't signal -- they'll check as they go busy.
     */
    if (pmap_pcid_ncpus) {
        if (pmap_is_shared)
            need_global_flush = TRUE;
        pmap_pcid_invalidate_all_cpus(pmap);
        mfence();
    }
    for (cpu = 0, cpu_bit = 1; cpu < real_ncpus; cpu++, cpu_bit <<= 1) {
        //if (!cpu_datap(cpu)->cpu_running)
        //    continue;
        uint64_t	cpu_active_cr3  = CPU_GET_ACTIVE_CR3(cpu);
        uint64_t	cpu_task_cr3    = CPU_GET_TASK_CR3(cpu);
        
        if ((pmap_cr3 == cpu_task_cr3) || (pmap_cr3 == cpu_active_cr3) || (pmap_is_shared)) {
            
            //if (options & PMAP_DELAY_TLB_FLUSH) {
            //    if (need_global_flush == TRUE)
            //        pfc->pfc_invalid_global |= cpu_bit;
            //    pfc->pfc_cpus |= cpu_bit;
            //
            //    continue;
            //}
            if (cpu == my_cpu) {
                flush_self = TRUE;
                continue;
            }
            if (need_global_flush == TRUE)
                cpu_datap(cpu)->cpu_tlb_invalid_global = TRUE;
            else
                cpu_datap(cpu)->cpu_tlb_invalid_local = TRUE;
            mfence();
            
            /*
             * We don't need to signal processors which will flush
             * lazily at the idle state or kernel boundary.
             * For example, if we're invalidating the kernel pmap,
             * processors currently in userspace don't need to flush
             * their TLBs until the next time they enter the kernel.
             * Alterations to the address space of a task active
             * on a remote processor result in a signal, to
             * account for copy operations. (There may be room
             * for optimization in such cases).
             * The order of the loads below with respect
             * to the store to the "cpu_tlb_invalid" field above
             * is important--hence the barrier.
             */
            //if (CPU_CR3_IS_ACTIVE(cpu) &&
            //    (pmap_cr3 == CPU_GET_ACTIVE_CR3(cpu) ||
            //     pmap->pm_shared ||
            //     (pmap_cr3 == CPU_GET_TASK_CR3(cpu)))) {
            //        cpus_to_signal |= cpu_bit;
            //        i386_signal_cpu(cpu, MP_TLB_FLUSH, ASYNC);
            //    }
        }
    }
    //if ((options & PMAP_DELAY_TLB_FLUSH))
    //    goto out;
    
    /*
     * Flush local tlb if required.
     * Do this now to overlap with other processors responding.
     */
    if (flush_self) {
        if (pmap_pcid_ncpus) {
            pmap_pcid_validate_cpu(pmap, my_cpu);
            if (pmap_is_shared)
                tlb_flush_global();
            else
                flush_tlb_raw();
        }
        else
            flush_tlb_raw();
    }
    
    //if (cpus_to_signal) {
    //    cpumask_t	cpus_to_respond = cpus_to_signal;
    //
    //    deadline = absolute_time() +
    //    (TLBTimeOut ? TLBTimeOut : LockTimeOut);
    //    boolean_t is_timeout_traced = FALSE;
    //
    //    /*
    //     * Wait for those other cpus to acknowledge
    //     */
    //    while (cpus_to_respond != 0) {
    //        long orig_acks = 0;
    //
    //        for (cpu = 0, cpu_bit = 1; cpu < real_ncpus; cpu++, cpu_bit <<= 1) {
    //            /* Consider checking local/global invalidity
    //             * as appropriate in the PCID case.
    //             */
    //            if ((cpus_to_respond & cpu_bit) != 0) {
    //                if (!cpu_datap(cpu)->cpu_running ||
    //                    cpu_datap(cpu)->cpu_tlb_invalid == FALSE ||
    //                    !CPU_CR3_IS_ACTIVE(cpu)) {
    //                    cpus_to_respond &= ~cpu_bit;
    //                }
    //                cpu_pause();
    //            }
    //            if (cpus_to_respond == 0)
    //                break;
    //        }
    //        if (cpus_to_respond && (mach_absolute_time() > deadline)) {
    //            if (machine_timeout_suspended())
    //                continue;
    //            if (TLBTimeOut == 0) {
    //                /* cut tracepoint but don't panic */
    //                if (is_timeout_traced)
    //                    continue;
    //                PMAP_TRACE_CONSTANT(PMAP_CODE(PMAP__FLUSH_TLBS_TO), VM_KERNEL_UNSLIDE_OR_PERM(pmap), cpus_to_signal, cpus_to_respond, 0, 0);
    //                is_timeout_traced = TRUE;
    //                continue;
    //            }
    //            pmap_tlb_flush_timeout = TRUE;
    //            orig_acks = NMIPI_acks;
    //            mp_cpus_NMIPI(cpus_to_respond);
    //
    //            panic("TLB invalidation IPI timeout: "
    //                  "CPU(s) failed to respond to interrupts, unresponsive CPU bitmap: 0x%llx, NMIPI acks: orig: 0x%lx, now: 0x%lx", cpus_to_respond, orig_acks, NMIPI_acks);
    //        }
    //    }
    //}
    
    if (__improbable((pmap == kernel_pmap) && (flush_self != TRUE))) {
        kprintf("I'm here in flush _improbable panic()!\n");
        panic("pmap_flush_tlbs: pmap == kernel_pmap && flush_self != TRUE; kernel CR3: 0x%llX, pmap_cr3: 0x%llx, CPU active CR3: 0x%llX, CPU Task Map: %d", kernel_pmap->pm_cr3, pmap_cr3, current_cpu_datap()->cpu_active_cr3, current_cpu_datap()->cpu_task_map);
    }
    
out:
    //PMAP_TRACE_CONSTANT(event_code | DBG_FUNC_END, VM_KERNEL_UNSLIDE_OR_PERM(pmap), cpus_to_signal, event_startv, event_endv, 0);
    kprintf("");
}
