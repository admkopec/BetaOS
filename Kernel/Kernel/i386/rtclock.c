//
//  rtclock.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/27/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "rtclock.h"
#include "machine_routines.h"
#include "tsc.h"
#include <platform/platform.h>

#include "misc_protos.h"

static void rtc_set_timescale(uint64_t cycles);

typedef unsigned spl_t;

#define	splhigh()	(spl_t) ml_set_interrupts_enabled(FALSE)
#define	splsched()	(spl_t) ml_set_interrupts_enabled(FALSE)
#define	splclock()	(spl_t) ml_set_interrupts_enabled(FALSE)
#define	splx(x)		(void)  ml_set_interrupts_enabled(x)
#define	spllo()		(void)  ml_set_interrupts_enabled(TRUE)

/*
 * Re-evaluate the outstanding deadlines and select the most proximate.
 *
 * Should be called at splclock.
 */
void
timer_resync_deadlines(void) {
    uint64_t		deadline = EndOfAllTime;
    //uint64_t		pmdeadline;
    rtclock_timer_t	*mytimer;
    spl_t			s = splclock();
    cpu_data_t		*pp;
    uint32_t		decr;
    
    pp = current_cpu_datap();
    if (!pp->cpu_running)
    /* There's really nothing to do if this processor is down */
        return;
    
    /*
     * If we have a clock timer set, pick that.
     */
    mytimer = &pp->rtclock_timer;
    if (!mytimer->has_expired && 0 < mytimer->deadline && mytimer->deadline < EndOfAllTime)
        deadline = mytimer->deadline;
    
    /*
     * If we have a power management deadline, see if that's earlier.
     */
    //pmdeadline = pmCPUGetDeadline(pp);
    //if (0 < pmdeadline && pmdeadline < deadline)
    //    deadline = pmdeadline;
    
    /*
     * Go and set the "pop" event.
     */
    decr = (uint32_t) setPop(deadline);
    
    /* Record non-PM deadline for latency tool */
    /*if (decr != 0 && deadline != pmdeadline) {
        KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE,
                                  DECR_SET_DEADLINE | DBG_FUNC_NONE,
                                  decr, 2,
                                  deadline,
                                  mytimer->queue.count, 0);
    }*/
    splx(s);
}

/*
 * Definitions for timer operations table
 */

rtc_timer_t	rtc_timer_lapic;/*  = {
    rtc_lapic_config_timer,
    rtc_lapic_set_timer
};

rtc_timer_t	rtc_timer_tsc_deadline  = {
    rtc_lapic_config_tsc_deadline_timer,
    rtc_lapic_set_tsc_deadline_timer
};*/

rtc_timer_t	*rtc_timer = &rtc_timer_lapic; /* defaults to LAPIC timer */

void
rtc_timer_start(void) {
    /*
     * Force a complete re-evaluation of timer deadlines.
     */
    x86_lcpu()->rtcDeadline = EndOfAllTime;
    timer_resync_deadlines();
}

static inline void
rtc_nanotime_set_commpage(__unused pal_rtc_nanotime_t *rntp) {
    //commpage_set_nanotime(rntp->tsc_base, rntp->ns_base, rntp->scale, rntp->shift);
}

static uint64_t
rtc_export_speed(uint64_t cycles_per_sec) {
    pal_rtc_nanotime_t  *rntp = &pal_rtc_nanotime_info;
    uint64_t            cycles;
    
    if (rntp->shift != 0 )
        kprintf("Slow TSC, rtc_nanotime.shift == %d\n", rntp->shift);
    
    /* Round: */
    cycles = ((cycles_per_sec + (10000000/2)) / 10000000) * 10000000;
    /*
     * Set current measured speed.
     */
    if (cycles >= 0x100000000ULL) {
        gClockFrequencyInfo.cpu_clock_rate_hz = 0xFFFFFFFFUL;
    } else {
        gClockFrequencyInfo.cpu_clock_rate_hz = (unsigned long)cycles;
    }
    gClockFrequencyInfo.cpu_frequency_hz = cycles;
    
    kprintf("[RTCLOCK] frequency %llu (%llu)\n", cycles, cycles_per_sec);
    return(cycles);
}

/*
 * rtc_nanotime_init:
 *
 * Intialize the nanotime info from the base time.
 */
static inline void
_rtc_nanotime_init(pal_rtc_nanotime_t *rntp, uint64_t base) {
    uint64_t tsc = rdtsc64();
    pal_rtc_nanotime_store(tsc, base, rntp->scale, rntp->shift, rntp);
}

static void
rtc_nanotime_init(uint64_t base) {
    _rtc_nanotime_init(&pal_rtc_nanotime_info, base);
    rtc_nanotime_set_commpage(&pal_rtc_nanotime_info);
}

/*
 * rtclock_early_init() is called very early at boot to
 * establish mach_absolute_time() and set it to zero.
 */
void
rtclock_early_init(void) {
    assert(tscFreq);
    rtc_set_timescale(tscFreq);
}

/*
 * Initialize the real-time clock device.
 * In addition, various variables used to support the clock are initialized.
 */
int
rtclock_init(void) {
    uint64_t	cycles;
    assert(!ml_get_interrupts_enabled());
    
    if (cpu_number() == master_cpu) {
        
        assert(tscFreq);
        
        /*
         * Adjust and set the exported cpu speed.
         */
        cycles = rtc_export_speed(tscFreq);
        
        /*
         * Set min/max to actual.
         * ACPI may update these later if speed-stepping is detected.
         */
        gClockFrequencyInfo.cpu_frequency_min_hz = cycles;
        gClockFrequencyInfo.cpu_frequency_max_hz = cycles;
        
        //rtc_timer_init();
        //clock_timebase_init();
        //ml_init_lock_timeout();
        //ml_init_delay_spin_threshold(10);
    }
    
    /* Set fixed configuration for lapic timers */
    rtc_timer->rtc_config();
    rtc_timer_start();
    
    return (1);
}

// utility routine
// Code to calculate how many processor cycles are in a second...

static void
rtc_set_timescale(uint64_t cycles) {
    pal_rtc_nanotime_t	*rntp = &pal_rtc_nanotime_info;
    uint32_t            shift = 0;
    
    /* the "scale" factor will overflow unless cycles>SLOW_TSC_THRESHOLD */
    
    while (cycles <= SLOW_TSC_THRESHOLD) {
        shift++;
        cycles <<= 1;
    }
    
    rntp->scale = (uint32_t)(((uint64_t)NSEC_PER_SEC << 32) / cycles);
    
    rntp->shift = shift;
    
    /*
     * On some platforms, the TSC is not reset at warm boot. But the
     * rebase time must be relative to the current boot so we can't use
     * mach_absolute_time(). Instead, we convert the TSC delta since boot
     * to nanoseconds.
     */
    //if (tsc_rebase_abs_time == 0)
    //    tsc_rebase_abs_time = _rtc_tsc_to_nanoseconds(rdtsc64() - tsc_at_boot, rntp);
    
    rtc_nanotime_init(0);
}

/*
 * rtc_nanotime_read:
 *
 * Returns the current nanotime value, accessable from any
 * context.
 */

extern uint64_t _rtc_nanotime_read(pal_rtc_nanotime_t *info);

static inline uint64_t
rtc_nanotime_read(void) {
    return	_rtc_nanotime_read(&pal_rtc_nanotime_info);
}

static inline void
absolutetime_to_nanotime(uint64_t abstime, uint64_t *secs, uint64_t *nanosecs) {
    *secs     = abstime / (uint64_t)NSEC_PER_SEC;
    *nanosecs = (uint64_t)(abstime % (uint64_t)NSEC_PER_SEC);
}

void
clock_get_system_nanotime(uint64_t *secs, uint64_t *nanosecs) {
    uint64_t now = rtc_nanotime_read();
    absolutetime_to_nanotime(now, secs, nanosecs);
}

uint64_t
absolute_time(void) {
    return rtc_nanotime_read();
}

/*
 *	Request timer pop from the hardware
 */

uint64_t
setPop(uint64_t time) {
    uint64_t	now;
    uint64_t	pop;
    
    /* 0 and EndOfAllTime are special-cases for "clear the timer" */
    if (time == 0 || time == EndOfAllTime ) {
        time = EndOfAllTime;
        now = 0;
        pop = rtc_timer->rtc_set(0, 0);
    } else {
        now = rtc_nanotime_read();	/* The time in nanoseconds */
        pop = rtc_timer->rtc_set(time, now);
    }
    
    /* Record requested and actual deadlines set */
    x86_lcpu()->rtcDeadline = time;
    x86_lcpu()->rtcPop      = pop;
    
    return pop - now;
}