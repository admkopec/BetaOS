//
//  pal.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/29/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef pal_h
#define pal_h

#include <stdint.h>
#include <stdbool.h>

#define pal_hlt()			__asm__ volatile ("sti; hlt")
#define pal_sti()			__asm__ volatile ("sti")
#define pal_cli()			__asm__ volatile ("cli")

static inline
void pal_stop_cpu(bool cli) {
    if(cli)
        __asm__ volatile ( "cli" );
    __asm__ volatile ( "wbinvd; hlt" );
}

typedef struct pal_rtc_nanotime {
    volatile uint64_t	tsc_base;	/* timestamp */
    volatile uint64_t	ns_base;	/* nanoseconds */
    uint32_t            scale;		/* tsc -> nanosec multiplier */
    uint32_t            shift;		/* shift is nonzero only on "slow" machines, where tscFreq <= SLOW_TSC_THRESHOLD */
    volatile uint32_t	generation;	/* 0 == being updated */
    uint32_t            spare1;
} pal_rtc_nanotime_t;

/* assembly function to update TSC / timebase info */
extern void pal_rtc_nanotime_store(uint64_t           tsc,
                                   uint64_t           nsec,
                                   uint32_t           scale,
                                   uint32_t           shift,
                                   pal_rtc_nanotime_t *dst);

extern pal_rtc_nanotime_t pal_rtc_nanotime_info;

extern int  serial_init(void);
extern void serial_putc(int c);
extern int  serial_getc(void);

#endif /* pal_h */
