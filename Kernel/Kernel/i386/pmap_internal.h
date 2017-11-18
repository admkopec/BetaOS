//
//  pmap_internal.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/4/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef pmap_internal_h
#define pmap_internal_h

#include <i386/vm_types.h>
#include <i386/cpu_data.h>
#include <stddef.h>
#include "eflags.h"
#include "machine_routines.h"
#include "misc_protos.h"

void	pmap_pcid_configure(void);

// <Temporary>
void kernel_preempt_check(void);

// </Temporary>

extern pmap_t kernel_pmap;

static inline bool is_ept_pmap(pmap_t p) {
    if (__probable(p->pm_cr3 != 0)) {
        assert(p->pm_eptp == 0);
        return false;
    }
    
    assert(p->pm_eptp != 0);
    
    return true;
}

extern	uint64_t physmap_base, physmap_max; // i386_init

static	inline void * PHYSMAP_PTOV_check(void *paddr) {
    uint64_t pvaddr = (uint64_t)paddr + physmap_base;
    if (__improbable(pvaddr >= physmap_max))
        panic("PHYSMAP_PTOV bounds exceeded, %x, %x, %x", pvaddr, physmap_base, physmap_max);
    
    return (void *)pvaddr;
}

#define PHYSMAP_PTOV(x)	(PHYSMAP_PTOV_check((void*) (x)))

static inline pml4_entry_t * pmap64_pml4(pmap_t pmap, vm_map_offset_t vaddr) {
    if (__improbable((vaddr > 0x00007FFFFFFFFFFFULL) &&
                     (vaddr < 0xFFFF800000000000ULL))) {
        return (NULL);
    }
    
#if	PMAP_ASSERT
    return PHYSMAP_PTOV(&((pml4_entry_t *)pmap->pm_cr3)[(vaddr >> PML4SHIFT) & (NPML4PG-1)]);
#else
    return &pmap->pm_pml4[(vaddr >> PML4SHIFT) & (NPML4PG-1)];
#endif
}

static inline pdpt_entry_t * pmap64_pdpt(pmap_t pmap, vm_map_offset_t vaddr) {
    pml4_entry_t	newpf;
    pml4_entry_t	*pml4;
    bool            is_ept;
    
    pml4 = pmap64_pml4(pmap, vaddr);
    is_ept = is_ept_pmap(pmap);
    
    if (pml4 && (*pml4 & PTE_VALID_MASK(is_ept))) {
        newpf = *pml4 & PG_FRAME;
        return &((pdpt_entry_t *) PHYSMAP_PTOV(newpf))
        [(vaddr >> PDPTSHIFT) & (NPDPTPG-1)];
    }
    return (NULL);
}

static inline pd_entry_t * pmap64_pde(pmap_t pmap, vm_map_offset_t vaddr) {
    pdpt_entry_t	newpf;
    pdpt_entry_t	*pdpt;
    bool            is_ept;
    
    pdpt = pmap64_pdpt(pmap, vaddr);
    is_ept = is_ept_pmap(pmap);
    
    if (pdpt && (*pdpt & PTE_VALID_MASK(is_ept))) {
        newpf = *pdpt & PG_FRAME;
        return &((pd_entry_t *) PHYSMAP_PTOV(newpf))
        [(vaddr >> PDSHIFT) & (NPDPG-1)];
    }
    return (NULL);
}

static inline pt_entry_t * pmap_pte(pmap_t pmap, vm_map_offset_t vaddr) {
    pd_entry_t	*pde;
    pd_entry_t	newpf;
    bool        is_ept;
    
    assert(pmap);
    pde = pmap64_pde(pmap, vaddr);
    
    is_ept = is_ept_pmap(pmap);
    
    if (pde && (*pde & PTE_VALID_MASK(is_ept))) {
        if (*pde & PTE_PS)
            return pde;
        newpf = *pde & PG_FRAME;
        return &((pt_entry_t *)PHYSMAP_PTOV(newpf))[i386_btop(vaddr) & (ppnum_t)(NPTEPG-1)];
    }
    return (NULL);
}

static inline pd_entry_t* pmap_pde(pmap_t m, vm_map_offset_t v) {
    pd_entry_t  *pde;
    
    pde = pmap64_pde(m, v);
    
    return pde;
}

/*
 * pmap_find_phys returns the (4K) physical page number containing a
 * given virtual address in a given pmap.
 * Note that pmap_pte may return a pde if this virtual address is
 * mapped by a large page and this is taken into account in order
 * to return the correct page number in this case.
 */
ppnum_t pmap_find_phys(pmap_t pmap, uint64_t va);

uint64_t kvtophys(vm_offset_t addr);

#endif /* pmap_internal_h */
