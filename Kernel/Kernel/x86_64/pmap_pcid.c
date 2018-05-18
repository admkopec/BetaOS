//
//  pmap_pcid.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/26/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <i386/pmap.h>
#include <i386/vm_types.h>
#include <i386/vm_param.h>
#include <i386/proc_reg.h>
#include <i386/cpuid.h>
#include <i386/cpu_data.h>
#include <i386/pmap_pcid.h>
#include <i386/machine_routines.h>

#include <i386/misc_protos.h>

extern pmap_t kernel_pmap;

#ifdef DEBUG
#define pmap_pcid_log(x...) printf(x)
#else
#define pmap_pcid_log(x...) 
#endif

uint32_t	pmap_pcid_ncpus;
bool        pmap_pcid_disabled = false;

void
pmap_pcid_configure(void) {
    int ccpu = cpu_number();
    uintptr_t cr4 = get_cr4();
    bool      pcid_present = false;
    
    pmap_pcid_log("PCID configure invoked on CPU %d\n", ccpu);
    assert(ml_get_interrupts_enabled() == false || get_preemption_level() !=0);
    assert(cpu_mode_is64bit());
    
    //if (Parse_boot_argn("-pmap_pcid_disable", &pmap_pcid_disabled, sizeof (pmap_pcid_disabled))) {
    //    pmap_pcid_log("PMAP: PCID feature disabled\n");
    //    printf("PMAP: PCID feature disabled, %u\n", pmap_pcid_disabled);
    //    printf("PMAP: PCID feature disabled %u\n", pmap_pcid_disabled);
    //}
    /* no_shared_cr3+PCID is currently unsupported */
//#if	DEBUG
//    if (pmap_pcid_disabled == false)
//        no_shared_cr3 = false;
//    else
//        no_shared_cr3 = true;
//#else
    if (no_shared_cr3)
        pmap_pcid_disabled = true;
//#endif
    if (pmap_pcid_disabled || no_shared_cr3) {
        unsigned i;
        /* Reset PCID status, as we may have picked up
         * strays if discovered prior to platform
         * expert initialization.
         */
        for (i = 0; i < real_ncpus; i++) {
            if (cpu_datap(i)) {
                cpu_datap(i)->cpu_pmap_pcid_enabled = false;
            }
            pmap_pcid_ncpus = 0;
        }
        cpu_datap(ccpu)->cpu_pmap_pcid_enabled = false;
        return;
    }
    /* DRKTODO: assert if features haven't been discovered yet. Redundant
     * invocation of cpu_mode_init and descendants masks this for now.
     */
    if ((cpuid_features() & CPUID_FEATURE_PCID))
        pcid_present = true;
    else {
        cpu_datap(ccpu)->cpu_pmap_pcid_enabled = false;
        pmap_pcid_log("PMAP: PCID not detected CPU %d\n", ccpu);
        return;
    }
    if ((cr4 & (CR4_PCIDE | CR4_PGE)) == (CR4_PCIDE|CR4_PGE)) {
        cpu_datap(ccpu)->cpu_pmap_pcid_enabled = true;
        pmap_pcid_log("PMAP: PCID already enabled %d\n", ccpu);
        return;
    }
    if (pcid_present == true) {
        pmap_pcid_log("Pre-PCID:CR0: 0x%lx, CR3: 0x%lx, CR4(CPU %d): 0x%lx\n", get_cr0(), get_cr3_raw(), ccpu, cr4);
        
        if (cpu_number() >= PMAP_PCID_MAX_CPUS) {
            panic("PMAP_PCID_MAX_CPUS %d\n", cpu_number());
        }
        if ((get_cr4() & CR4_PGE) == 0) {
            set_cr4(get_cr4() | CR4_PGE);
            pmap_pcid_log("Toggled PGE ON (CPU: %d\n", ccpu);
        }
        set_cr4(get_cr4() | CR4_PCIDE);
        pmap_pcid_log("Post PCID: CR0: 0x%lx, CR3: 0x%lx, CR4(CPU %d): 0x%lx\n", get_cr0(), get_cr3_raw(), ccpu, get_cr4());
        tlb_flush_global();
        cpu_datap(ccpu)->cpu_pmap_pcid_enabled = true;
        
        //if (OSIncrementAtomic(&pmap_pcid_ncpus) == machine_info.max_cpus) {
         //   pmap_pcid_log("All PCIDs enabled: real_ncpus: %d, pmap_pcid_ncpus: %d\n", real_ncpus, pmap_pcid_ncpus);
        //}
        cpu_datap(ccpu)->cpu_pmap_pcid_coherentp =
        cpu_datap(ccpu)->cpu_pmap_pcid_coherentp_kernel =
        &(kernel_pmap->pmap_pcid_coherency_vector[ccpu]);
        cpu_datap(ccpu)->cpu_pcid_refcounts[0] = 1;
    }
}

void pmap_pcid_initialize(pmap_t p) {
    unsigned i;
    unsigned nc = sizeof(p->pmap_pcid_cpus)/sizeof(pcid_t);
    
    assert(nc >= real_ncpus);
    for (i = 0; i < nc; i++) {
        p->pmap_pcid_cpus[i] = PMAP_PCID_INVALID_PCID;
        /* We assume here that the coherency vector is zeroed by
         * pmap_create
         */
    }
}

void pmap_pcid_initialize_kernel(pmap_t p) {
    unsigned i;
    unsigned nc = sizeof(p->pmap_pcid_cpus)/sizeof(pcid_t);
    
    for (i = 0; i < nc; i++) {
        p->pmap_pcid_cpus[i] = 0;
        printf("");
        /* We assume here that the coherency vector is zeroed by
         * pmap_create
         */
    }
}
