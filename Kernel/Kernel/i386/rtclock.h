//
//  rtclock.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/27/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef rtclock_h
#define rtclock_h

#include <stdint.h>
#include <stdbool.h>
#include <i386/cpu_topology.h>
#include <i386/cpu_threads.h>
#include <i386/proc_reg.h>

#define EndOfAllTime		0xFFFFFFFFFFFFFFFFULL

#define NSEC_PER_USEC	1000ull         /* nanoseconds  per microsecond */
#define USEC_PER_SEC	1000000ull      /* microseconds per second */
#define NSEC_PER_SEC	1000000000ull	/* nanoseconds  per second */
#define NSEC_PER_MSEC	1000000ull      /* nanoseconds  per millisecond */

/*
 * Timer control.
 */
typedef struct {
    void	 (*rtc_config)(void);
    uint64_t (*rtc_set)   (uint64_t, uint64_t);
} rtc_timer_t;
extern rtc_timer_t	*rtc_timer;

extern uint64_t setPop(uint64_t time);
extern void rtclock_early_init(void);
extern uint64_t absolute_time(void);
extern uint64_t mach_absolute_time(void);
extern void clock_get_system_nanotime(uint64_t *secs, uint64_t *nanosecs);

#endif /* rtclock_h */
