//
//  pmap.h
//  BetaOS
//
//  Created by Adam Kopeć on 4/30/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdint.h>

/* <Temporary> */
#define BYTE_SIZE		8		/* byte size in bits */

#define I386_PGBYTES		4096		/* bytes per 80386 page */
#define I386_PGSHIFT		12		/* bitshift for pages */

#define	PAGE_SIZE		I386_PGBYTES
#define	PAGE_SHIFT		I386_PGSHIFT
#define	PAGE_MASK		(PAGE_SIZE - 1)

#define PAGE_MAX_SHIFT          PAGE_SHIFT
#define PAGE_MAX_SIZE           PAGE_SIZE
#define PAGE_MAX_MASK           PAGE_MASK

#define PAGE_MIN_SHIFT          PAGE_SHIFT
#define PAGE_MIN_SIZE           PAGE_SIZE
#define PAGE_MIN_MASK           PAGE_MASK

/* </Temporary> */

#define NPML4PGS        (1ULL * (PAGE_SIZE/(sizeof (pml4_entry_t))))
#define NPDPTPGS        (NPML4PGS * (PAGE_SIZE/(sizeof (pdpt_entry_t))))
#define NPDEPGS         (NPDPTPGS * (PAGE_SIZE/(sizeof (pd_entry_t))))
#define NPTEPGS         (NPDEPGS * (PAGE_SIZE/(sizeof (pt_entry_t))))

#define KERNEL_PML4_INDEX		511
#define KERNEL_KEXTS_INDEX	510	/* Home of KEXTs - the basement */
#define KERNEL_PHYSMAP_PML4_INDEX	509	/* virtual to physical map */
#define KERNEL_BASE		(0ULL - NBPML4)
#define KERNEL_BASEMENT		(KERNEL_BASE - NBPML4)

#define PTE_PER_PAGE	512 /* number of PTE's per page on any level */

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

typedef uint64_t  pmap_paddr_t;

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

#define INTEL_COMPRESSED	(1ULL << 62) /* marker, for invalid PTE only -- ignored by hardware for both regular/EPT entries*/
