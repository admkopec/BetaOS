//
//  trap.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/7/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "trap.h"
#include "cpu_threads.h"
#include "thread_status.h"
#include "lapic.h"
#include "misc_protos.h"
#include "cpu_topology.h"

/*
 * Handle interrupts:
 *  - local APIC interrupts (IPIs, timers, etc) are handled by the kernel,
 *  - device interrupts go to the platform expert.
 */
void
interrupt(x86_saved_state_t *state) {
    uint64_t	rip;
    uint64_t	rsp;
    int         interrupt_num;
    bool        user_mode = false;
    int         ipl;
    int         cnum = cpu_number();
    cpu_data_t	*cdp = cpu_data_ptr[cnum];
    int         itype = 0;
    
    if (is_saved_state64(state) == true) {
        x86_saved_state64_t	*state64;
        
        state64         = saved_state64(state);
        rip             = state64->isf.rip;
        rsp             = state64->isf.rsp;
        interrupt_num   = state64->isf.trapno;
        if(state64->isf.cs & 0x03)
            user_mode = true;
    } else {
        x86_saved_state32_t	*state32;
        
        state32 = saved_state32(state);
        if (state32->cs & 0x03)
            user_mode   = true;
        rip             = state32->eip;
        rsp             = state32->uesp;
        interrupt_num   = state32->trapno;
    }
    
    if (cpu_data_ptr[cnum]->lcpu.package->num_idle == topoParms.nLThreadsPerPackage)
        cpu_data_ptr[cnum]->cpu_hwIntpexits[interrupt_num]++;
    if (interrupt_num == (LAPIC_DEFAULT_INTERRUPT_BASE + LAPIC_INTERPROCESSOR_INTERRUPT))
        itype = 1;
    else if (interrupt_num == (LAPIC_DEFAULT_INTERRUPT_BASE + LAPIC_TIMER_INTERRUPT))
        itype = 2;
    else
        itype = 3;
    
    //KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE, DBG_CODE(DBG_EXCP_INTR, 0) | DBG_FUNC_START, interrupt_num, (user_mode ? rip : VM_KERNEL_UNSLIDE(rip)), user_mode, itype, 0);
    //SCHED_STATS_INTERRUPT(current_processor());
    
#if CONFIG_TELEMETRY
    if (telemetry_needs_record) {
        telemetry_mark_curthread(user_mode);
    }
#endif
    
    ipl = get_preemption_level();
    
    /*
     * Handle local APIC interrupts
     * else call platform expert for devices.
     */
    //if (!lapic_interrupt(interrupt_num, state)) {
    //    PE_incoming_interrupt(interrupt_num);
    //}
    
    if (__improbable(get_preemption_level() != ipl)) {
        panic("Preemption level altered by interrupt vector 0x%x: initial 0x%x, final: 0x%x\n", interrupt_num, ipl, get_preemption_level());
    }
    
    
    if (__improbable(cdp->cpu_nested_istack)) {
        cdp->cpu_nested_istack_events++;
    }
    else  {
        //uint64_t ctime = mach_absolute_time();
        //uint64_t int_latency = ctime - cdp->cpu_int_event_time;
        __unused uint64_t esdeadline, ehdeadline;
        /* Attempt to process deferred timers in the context of
         * this interrupt, unless interrupt time has already exceeded
         * TCOAL_ILAT_THRESHOLD.
         */
#define TCOAL_ILAT_THRESHOLD (30000ULL)
        
        /*if ((int_latency < TCOAL_ILAT_THRESHOLD) &&
            interrupt_timer_coalescing_enabled) {
            esdeadline = cdp->rtclock_timer.queue.earliest_soft_deadline;
            ehdeadline = cdp->rtclock_timer.deadline;
            if ((ctime >= esdeadline) && (ctime < ehdeadline)) {
                interrupt_coalesced_timers++;
                TCOAL_DEBUG(0x88880000 | DBG_FUNC_START, ctime, esdeadline, ehdeadline, interrupt_coalesced_timers, 0);
                rtclock_intr(state);
                TCOAL_DEBUG(0x88880000 | DBG_FUNC_END, ctime, esdeadline, interrupt_coalesced_timers, 0, 0);
            } else {
                TCOAL_DEBUG(0x77770000, ctime, cdp->rtclock_timer.queue.earliest_soft_deadline, cdp->rtclock_timer.deadline, interrupt_coalesced_timers, 0);
            }
        }*/
        
        //if (__improbable(ilat_assert && (int_latency > interrupt_latency_cap) && !machine_timeout_suspended())) {
        //    panic("Interrupt vector 0x%x exceeded interrupt latency threshold, 0x%llx absolute time delta, prior signals: 0x%x, current signals: 0x%x", interrupt_num, int_latency, cdp->cpu_prior_signals, cdp->cpu_signals);
        //}
        
        //if (__improbable(int_latency > cdp->cpu_max_observed_int_latency)) {
        //    cdp->cpu_max_observed_int_latency = int_latency;
        //    cdp->cpu_max_observed_int_latency_vector = interrupt_num;
        //}
    }
    
    /*
     * Having serviced the interrupt first, look at the interrupted stack depth.
     */
    if (!user_mode) {
        __unused uint64_t depth = cdp->cpu_kernel_stack + sizeof(struct x86_kernel_state) + sizeof(struct i386_exception_link *) - rsp;
        /*if (__improbable(depth > kernel_stack_depth_max)) {
            kernel_stack_depth_max = (vm_offset_t)depth;
            KERNEL_DEBUG_CONSTANT(
                                  MACHDBG_CODE(DBG_MACH_SCHED, MACH_STACK_DEPTH),
                                  (long) depth, (long) VM_KERNEL_UNSLIDE(rip), 0, 0, 0);
        }*/
    }
    
    //if (cnum == master_cpu)
    //    ml_entropy_collect();
    
    //KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE,
    //                          MACHDBG_CODE(DBG_MACH_EXCP_INTR, 0) | DBG_FUNC_END,
    //                          interrupt_num, 0, 0, 0, 0);
    
    //assert(ml_get_interrupts_enabled() == FALSE);
}