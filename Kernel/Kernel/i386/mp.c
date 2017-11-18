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
#include <i386/proc_reg.h>
#include <i386/machine_routines.h>
#include <i386/tsc.h>
#include <i386/rtclock.h>
#include <i386/pmap.h>

char		mp_slave_stack[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE))); // Temp stack for slave init

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
