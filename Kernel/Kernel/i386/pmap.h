//
//  pmap.h
//  BetaOS
//
//  Created by Adam Kopeć on 4/30/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef pmap_h
#define pmap_h

#ifndef __ASSEMBLY__

#include <stdint.h>
#include <stdbool.h>
#include <i386/mp.h>
#include <i386/vm_types.h>
#include <sys/cdefs.h>
#include "vm_param.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif  /* MAX */

#endif /* __ASSEMBLY__ */

#define NPGPTD          4ULL
#define PDESHIFT        21ULL
#define PTEMASK         0x1ffULL
#define PTEINDX         3ULL

#define PTESHIFT        12ULL

#define NPTEPG          (PAGE_SIZE/(sizeof (pt_entry_t)))

#define LOW_4GB_MASK	((vm_offset_t)0x00000000FFFFFFFFUL)

#define	INTEL_PGBYTES		I386_PGBYTES
#define INTEL_PGSHIFT		I386_PGSHIFT
#define INTEL_OFFMASK       (I386_PGBYTES - 1)
#define INTEL_LOFFMASK      (I386_LPGBYTES - 1)

#define PG_FRAME        0x000FFFFFFFFFF000ULL

#define PTE_PER_PAGE	512 /* number of PTE's per page on any level */

/* in 64 bit spaces, the number of each type of page in the page tables */
#define NPML4PGS        (1ULL     * (PAGE_SIZE/(sizeof (pml4_entry_t))))
#define NPDPTPGS        (NPML4PGS * (PAGE_SIZE/(sizeof (pdpt_entry_t))))
#define NPDEPGS         (NPDPTPGS * (PAGE_SIZE/(sizeof (pd_entry_t))))
#define NPTEPGS         (NPDEPGS  * (PAGE_SIZE/(sizeof (pt_entry_t))))

#define KERNEL_PML4_INDEX           511
#define KERNEL_KEXTS_INDEX          510     /* Home of KEXTs - the basement */
#define KERNEL_PHYSMAP_PML4_INDEX	509     /* virtual to physical map */
#define KERNEL_BASE                 (0ULL - NBPML4)
#define KERNEL_BASEMENT             (KERNEL_BASE - NBPML4)

#ifndef __ASSEMBLY__

 /* cleanly define parameters for all the page table levels */
typedef uint64_t        pml4_entry_t;
#define NPML4PG         (PAGE_SIZE/(sizeof (pml4_entry_t)))
#define PML4SHIFT       39
#define PML4PGSHIFT     9
#define NBPML4          (1ULL << PML4SHIFT)
#define PML4MASK        (NBPML4-1)
#define PML4_ENTRY_NULL ((pml4_entry_t *) 0)

typedef uint64_t        pdpt_entry_t;
#define NPDPTPG         (PAGE_SIZE/(sizeof (pdpt_entry_t)))
#define PDPTSHIFT       30
#define PDPTPGSHIFT     9
#define NBPDPT          (1ULL << PDPTSHIFT)
#define PDPTMASK        (NBPDPT-1)
#define PDPT_ENTRY_NULL ((pdpt_entry_t *) 0)

typedef uint64_t        pd_entry_t;
#define NPDPG           (PAGE_SIZE/(sizeof (pd_entry_t)))
#define PDSHIFT         21
#define PDPGSHIFT       9
#define NBPD            (1ULL << PDSHIFT)
#define PDMASK          (NBPD-1)
#define PD_ENTRY_NULL   ((pd_entry_t *) 0)

typedef uint64_t        pt_entry_t;
#define NPTPG           (PAGE_SIZE/(sizeof (pt_entry_t)))
#define PTSHIFT         12
#define PTPGSHIFT       9
#define NBPT            (1ULL << PTSHIFT)
#define PTMASK          (NBPT-1)
#define PT_ENTRY_NULL	((pt_entry_t *) 0)

typedef uint64_t        pmap_paddr_t;

#endif /* __ASSEMBLY__ */

#define VM_MEM_GUARDED          0x1		/* (G) Guarded Storage */
#define VM_MEM_COHERENT         0x2		/* (M) Memory Coherency */
#define VM_MEM_NOT_CACHEABLE	0x4		/* (I) Cache Inhibit */
#define VM_MEM_WRITE_THROUGH	0x8		/* (W) Write-Through */

#define VM_WIMG_USE_DEFAULT     0x80
#define VM_WIMG_MASK            0xFF

#define	VM_WIMG_COPYBACK	VM_MEM_COHERENT
#define	VM_WIMG_COPYBACKLW  VM_WIMG_COPYBACK
#define	VM_WIMG_DEFAULT		VM_MEM_COHERENT
#define VM_WIMG_IO         (VM_MEM_COHERENT      | VM_MEM_NOT_CACHEABLE | VM_MEM_GUARDED)
#define VM_WIMG_WTHRU      (VM_MEM_WRITE_THROUGH | VM_MEM_COHERENT      | VM_MEM_GUARDED)
#define VM_WIMG_WCOMB      (VM_MEM_NOT_CACHEABLE | VM_MEM_COHERENT)
#define	VM_WIMG_INNERWBACK	VM_MEM_COHERENT

#define CPU_CR3_MARK_INACTIVE()						\
current_cpu_datap()->cpu_active_cr3 |= 1

#define CPU_CR3_MARK_ACTIVE()	 					\
current_cpu_datap()->cpu_active_cr3 &= ~1

#define CPU_CR3_IS_ACTIVE(cpu)						\
((cpu_datap(cpu)->cpu_active_cr3 & 1) == 0)

#define CPU_GET_ACTIVE_CR3(cpu)						\
(cpu_datap(cpu)->cpu_active_cr3 & ~1)

#define CPU_GET_TASK_CR3(cpu)						\
(cpu_datap(cpu)->cpu_task_cr3)

/*
 * Pte related macros
 */
#define KVADDR(pmi, pdpi, pdi, pti)		  \
((vm_offset_t)			  \
((uint64_t) -1    << 47)        | \
((uint64_t)(pmi)  << PML4SHIFT) | \
((uint64_t)(pdpi) << PDPTSHIFT) | \
((uint64_t)(pdi)  << PDESHIFT)  | \
((uint64_t)(pti)  << PTESHIFT))

/*
 * Size of Kernel address space.  This is the number of page table pages
 * (4MB each) to use for the kernel.  256 pages == 1 Gigabyte.
 * This **MUST** be a multiple of 4 (eg: 252, 256, 260, etc).
 */
#ifndef KVA_PAGES
#define KVA_PAGES   1024
#endif

#ifndef NKPT
#define	NKPT        500             /* actual number of kernel page tables */
#endif
#ifndef NKPDE
#define NKPDE       (KVA_PAGES - 1) /* addressable number of page tables/pde's */
#endif

/*
 *	Hardware pte bit definitions (to be used directly on the ptes
 *	without using the bit fields).
 */

#define INTEL_PTE_VALID		0x00000001ULL
#define INTEL_PTE_WRITE		0x00000002ULL
#define INTEL_PTE_RW		0x00000002ULL
#define INTEL_PTE_USER		0x00000004ULL
#define INTEL_PTE_WTHRU		0x00000008ULL
#define INTEL_PTE_NCACHE 	0x00000010ULL
#define INTEL_PTE_REF		0x00000020ULL
#define INTEL_PTE_MOD		0x00000040ULL
#define INTEL_PTE_PS		0x00000080ULL
#define INTEL_PTE_PTA		0x00000080ULL
#define INTEL_PTE_GLOBAL	0x00000100ULL
#define INTEL_PTE_WIRED		0x00000400ULL
#define INTEL_PDPTE_NESTED	0x00000800ULL
#define INTEL_PTE_PFN		PG_FRAME

#define INTEL_PTE_NX		(1ULL << 63)

#define INTEL_PTE_INVALID       0
/* This is conservative, but suffices */
#define INTEL_PTE_RSVD		((1ULL << 10) | (1ULL << 11) | (0x1FFULL << 54))

#define	pa_to_pte(a)		((a) & INTEL_PTE_PFN)
#define	pte_to_pa(p)		((p) & INTEL_PTE_PFN)
#define	pte_increment_pa(p)	((p) += INTEL_OFFMASK+1)

#define INTEL_COMPRESSED	(1ULL << 62) /* marker, for invalid PTE only -- ignored by hardware for both regular/EPT entries*/

#define INTEL_EPTP_AD		0x00000040ULL

#define INTEL_EPT_READ		0x00000001ULL
#define INTEL_EPT_WRITE 	0x00000002ULL
#define INTEL_EPT_EX		0x00000004ULL
#define INTEL_EPT_IPTA		0x00000040ULL
#define INTEL_EPT_PS		0x00000080ULL
#define INTEL_EPT_REF		0x00000100ULL
#define INTEL_EPT_MOD		0x00000200ULL

#define INTEL_EPT_CACHE_MASK 0x00000038ULL
#define INTEL_EPT_NCACHE	0x00000000ULL
#define INTEL_EPT_WC		0x00000008ULL
#define INTEL_EPT_WTHRU 	0x00000020ULL
#define INTEL_EPT_WP    	0x00000028ULL
#define INTEL_EPT_WB		0x00000030ULL

#define PTE_VALID_MASK(is_ept)	((is_ept) ? (INTEL_EPT_READ | INTEL_EPT_WRITE | INTEL_EPT_EX) : INTEL_PTE_VALID)
#define PTE_READ(is_ept)        ((is_ept) ? INTEL_EPT_READ : INTEL_PTE_VALID)
#define PTE_WRITE(is_ept)       ((is_ept) ? INTEL_EPT_WRITE : INTEL_PTE_WRITE)
#define PTE_PS                  INTEL_PTE_PS
#define PTE_COMPRESSED          INTEL_COMPRESSED
#define PTE_NCACHE(is_ept)      ((is_ept) ? INTEL_EPT_NCACHE : INTEL_PTE_NCACHE)
#define PTE_WTHRU(is_ept)       ((is_ept) ? INTEL_EPT_WTHRU : INTEL_PTE_WTHRU)
#define PTE_REF(is_ept)         ((is_ept) ? INTEL_EPT_REF : INTEL_PTE_REF)
#define PTE_MOD(is_ept)         ((is_ept) ? INTEL_EPT_MOD : INTEL_PTE_MOD)
#define PTE_WIRED               INTEL_PTE_WIRED

/*
 * For KASLR, we alias the master processor's IDT and GDT at fixed
 * virtual addresses to defeat SIDT/SGDT address leakage.
 * And non-boot processor's GDT aliases likewise (skipping LOWGLOBAL_ALIAS)
 * The low global vector page is mapped at a fixed alias also.
 */

/* <Temporary> */
#define VM_MIN_KERNEL_ADDRESS		((vm_offset_t) 0xFFFFFF8000000000UL)
#define I386_LOWMEM_RESERVED		3
/* </Temporary> */

#define MASTER_IDT_ALIAS	(VM_MIN_KERNEL_ADDRESS + 0x0000)
#define MASTER_GDT_ALIAS	(VM_MIN_KERNEL_ADDRESS + 0x1000)
#define LOWGLOBAL_ALIAS		(VM_MIN_KERNEL_ADDRESS + 0x2000)
#define FRAMEBUFFER_ALIAS   (VM_MIN_KERNEL_ADDRESS + 0x5000)
#define CPU_GDT_ALIAS(_cpu)	(LOWGLOBAL_ALIAS + (0x1000*(_cpu)))

#ifndef __ASSEMBLY__
extern uint32_t     pmap_pcid_ncpus;
extern pt_entry_t	*PTmap;
extern pdpt_entry_t	*IdlePDPT;
extern pml4_entry_t	*IdlePML4;
extern bool         no_shared_cr3;
extern uint64_t		kernel64_cr3;
extern pd_entry_t	*IdlePTD;	/* physical addr of "Idle" state PTD */

extern uint64_t		pmap_pv_hashlist_walks;
extern uint64_t		pmap_pv_hashlist_cnts;
extern uint32_t		pmap_pv_hashlist_max;
extern uint32_t		pmap_kernel_text_ps;

#define ptenum(a)       (((vm_offset_t)(a) >> PTESHIFT) & PTEMASK)

#define ID_MAP_VTOP(x)	((void *)(((uint64_t)(x)) & LOW_4GB_MASK))

#define NPHYSMAP        (MAX(K64_MAXMEM/GB + 4, 4))

#ifndef TASK_MAP_DEFINED
typedef enum {
    TASK_MAP_32BIT,			/* 32-bit user, compatibility mode */
    TASK_MAP_64BIT,			/* 64-bit user thread, shared space */
} task_map_t;
typedef	uint16_t	pcid_t;
typedef	uint8_t		pcid_ref_t;
#define TASK_MAP_DEFINED
#endif

struct pmap {
    //decl_simple_lock_data(,lock)	/* lock on map */
    pmap_paddr_t    pm_cr3;         /* physical addr */
    pmap_paddr_t	pm_eptp;        /* EPTP */
    bool            pm_shared;
    pd_entry_t      *dirbase;       /* page directory pointer */
    vm_object_t     pm_obj;         /* object to hold pde's */
    task_map_t      pm_task_map;
    pdpt_entry_t    *pm_pdpt;       /* KVA of 3rd level page */
    pml4_entry_t    *pm_pml4;       /* VKA of top level */
    vm_object_t     pm_obj_pdpt;    /* holds pdpt pages */
    vm_object_t     pm_obj_pml4;    /* holds pml4 pages */
#define	PMAP_PCID_MAX_CPUS	MAX_CPUS	/* Must be a multiple of 8 */
    pcid_t          pmap_pcid_cpus[PMAP_PCID_MAX_CPUS];
    volatile uint8_t pmap_pcid_coherency_vector[PMAP_PCID_MAX_CPUS];
    //struct pmap_statistics	stats;	/* map statistics */
    int             ref_count;      /* reference count */
    int             nx_enabled;
    //ledger_t        ledger;         /* ledger tracking phys mappings */
};

typedef struct pmap_memory_regions {
    ppnum_t base;		/* first page of this region */
    ppnum_t alloc_up;	/* pages below this one have been "stolen" */
    ppnum_t alloc_down;	/* pages above this one have been "stolen" */
    ppnum_t end;		/* last page of this region */
    uint32_t type;
    uint64_t attribute;
} pmap_memory_region_t;

extern unsigned pmap_memory_region_count;
extern unsigned pmap_memory_region_current;
extern uint64_t sane_size;

extern void		pmap_bootstrap(vm_offset_t load_start, bool IA32e);
extern ppnum_t  pmap_find_phys(pmap_t pmap, uint64_t va);

#define PMAP_MEMORY_REGIONS_SIZE 128
extern pmap_memory_region_t pmap_memory_regions[];

/*
 * Atomic 64-bit store of a page table entry.
 */
static inline void
pmap_store_pte(pt_entry_t *entryp, pt_entry_t value) {
    /*
     * In the 32-bit kernel a compare-and-exchange loop was
     * required to provide atomicity. For K64, life is easier:
     */
    *entryp = value;
}

typedef struct {
    long	pfc_cpus;
    long	pfc_invalid_global;
} pmap_flush_context;

#define PMAP_UPDATE_TLBS(pmap, s, e)			\
        pmap_flush_tlbs(pmap, s, e, 0, NULL)

void
pmap_flush_tlbs(pmap_t, vm_map_offset_t, vm_map_offset_t, int, pmap_flush_context *);


#endif /* __ASSEMBLY__ */

#endif /* pmap_h */
