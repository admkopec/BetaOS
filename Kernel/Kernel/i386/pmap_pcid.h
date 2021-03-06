//
//  pmap_pcid.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/26/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef pmap_pcid_h
#define pmap_pcid_h
#include <i386/machine_routines.h>
#include <i386/cpu_data.h>
#include <i386/proc_reg.h>
#include <i386/misc_protos.h>
#if defined(__x86_64__)
void    pmap_pcid_initialize(pmap_t);
void    pmap_pcid_initialize_kernel(pmap_t);
pcid_t	pmap_pcid_allocate_pcid(int);
void	pmap_pcid_deallocate_pcid(int, pmap_t);
void	pmap_destroy_pcid_sync_action(void *);
void	pmap_destroy_pcid_sync(pmap_t);
void	pmap_pcid_lazy_flush(pmap_t);
void	pmap_pcid_activate(pmap_t, int);
pcid_t	pcid_for_pmap_cpu_tuple(pmap_t, int);

#define PMAP_INVALID ((pmap_t)0xDEAD7347)
#define PMAP_PCID_INVALID_PCID	(0xDEAD)
#define	PMAP_PCID_MAX_REFCOUNT (0xF0)
#define	PMAP_PCID_MIN_PCID (1)

extern uint32_t pmap_pcid_ncpus;

static inline void
tlb_flush_global(void) {
    uintptr_t cr4 = get_cr4();
    assert(ml_get_interrupts_enabled() == false || get_preemption_level() !=0);
    //assert(((cr4 & CR4_PGE) || ml_at_interrupt_context()), "CR4: 0x%lx", cr4);
    assert((cr4 &CR4_PGE) || ml_at_interrupt_context());
    /*
     * We are, unfortunately, forced to rely on this expensive
     * read-modify-write-write scheme due to the inadequate
     * TLB invalidation ISA. The read is necessary as
     * the kernel does not "own" the contents of CR4, the VMX
     * feature in particular. It may be possible to
     * avoid a global flush and instead track a generation
     * count of kernel invalidations, but that scheme
     * has its disadvantages as well.
     */
    set_cr4(cr4 & ~CR4_PGE);
    set_cr4(cr4 | CR4_PGE);
    return;
}

static inline void pmap_pcid_invalidate_all_cpus(pmap_t tpmap) {
    unsigned i;
    
    assert((sizeof(tpmap->pmap_pcid_coherency_vector) >= real_ncpus) && (!(sizeof(tpmap->pmap_pcid_coherency_vector) & 7)));
    
    for (i = 0; i < real_ncpus; i+=8) {
        *(uint64_t *)(uintptr_t)&tpmap->pmap_pcid_coherency_vector[i] = (~0ULL);
    }
}

static inline void pmap_pcid_validate_current(void) {
    int	ccpu = cpu_number();
    volatile uint8_t *cptr = cpu_datap(ccpu)->cpu_pmap_pcid_coherentp;
#ifdef	PMAP_MODULE
    assert(cptr == &(current_thread()->map->pmap->pmap_pcid_coherency_vector[ccpu]));
#endif
    if (cptr) {
        *cptr = 0;
    }
    
}

static inline void pmap_pcid_invalidate_cpu(pmap_t tpmap, int ccpu) {
    tpmap->pmap_pcid_coherency_vector[ccpu] = 0xFF;
}

static inline void pmap_pcid_validate_cpu(pmap_t tpmap, int ccpu) {
    tpmap->pmap_pcid_coherency_vector[ccpu] = 0;
}
#endif /* x86_64 */
#endif /* pmap_pcid_h */
