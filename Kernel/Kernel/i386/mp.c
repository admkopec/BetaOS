//
//  mp.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/mp.h>
#include <i386/cpu_data.h>
#include <i386/cpuid.h>
#include <i386/lapic.h>
#include <i386/proc_reg.h>
#include <i386/machine_routines.h>
#include <i386/misc_protos.h>
#include <i386/tsc.h>
#include <i386/rtclock.h>
#include <i386/pmap.h>

char		mp_slave_stack[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE))); // Temp stack for slave init
extern void slave_pstart(void);

bool 		smp_initialized = false;

void
i386_activate_cpu(void) {
    cpu_data_t	*cdp = current_cpu_datap();
    
    assert(!ml_get_interrupts_enabled());
    
    if (!smp_initialized) {
        cdp->cpu_running = true;
        return;
    }
    
    //simple_lock(&x86_topo_lock);
    cdp->cpu_running = true;
    //started_cpu();
    //simple_unlock(&x86_topo_lock);
    flush_tlb_raw();
}

void
i386_start_cpu(int lapic_id, __unused int cpu_num ) {
    LAPIC_WRITE_ICR(lapic_id, LAPIC_ICR_DM_INIT);
//    delay(100);
    LAPIC_WRITE_ICR(lapic_id, LAPIC_ICR_DM_STARTUP|(REAL_MODE_BOOTSTRAP_OFFSET>>12));
}

bool
i386_smp_init(int nmi_vector, i386_intr_func_t nmi_handler, int ipi_vector, i386_intr_func_t ipi_handler) {
    if (!lapic_probe()) {
        return false;
    }
    
    lapic_init();
    lapic_set_intr_func(nmi_vector, nmi_handler);
    lapic_set_intr_func(ipi_vector, ipi_handler);
    
//    install_real_mode_bootstrap(slave_pstart);
    
    return true;
}

static int
NMIInterruptHandler(x86_saved_state_t *regs) {
    return 0;
}

int
cpu_signal_handler(x86_saved_state_t *regs) {
    return 0;
}

void
smp_init(void) {
    if (!i386_smp_init(LAPIC_NMI_INTERRUPT, NMIInterruptHandler, LAPIC_VECTOR(INTERPROCESSOR), cpu_signal_handler)) {
        return;
    }
    
    smp_initialized = true;
}

/* <Temp> */
void
cpu_init(void) {
    cpu_data_t	*cdp = current_cpu_datap();
    
    //timer_call_queue_init(&cdp->rtclock_timer.queue);
    cdp->rtclock_timer.deadline = EndOfAllTime;
    
    cdp->cpu_type = cpuid_info()->cpuid_type;
    cdp->cpu_subtype = cpuid_info()->cpuid_cpu_subtype;
    
    i386_activate_cpu();
}
/* </Temp> */
