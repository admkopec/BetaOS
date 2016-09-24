//
//  tsc.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/23/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef tsc_h
#define tsc_h

#define BASE_NHM_CLOCK_SOURCE 	133333333ULL
#define BASE_ART_CLOCK_SOURCE 	24000000ULL	/* 24Mhz */
#define IA32_PERF_STS           0x198
#define	SLOW_TSC_THRESHOLD      1000067800	/* if slower, nonzero shift required in nanotime() algorithm */

#ifndef __ASSEMBLY__

extern uint64_t	busFCvtt2n;
extern uint64_t	busFCvtn2t;
extern uint64_t tscFreq;
extern uint64_t tscFCvtt2n;
extern uint64_t tscFCvtn2t;
extern uint64_t tscGranularity;
extern uint64_t bus2tsc;
extern uint64_t busFreq;
extern uint32_t	flex_ratio;
extern uint32_t	flex_ratio_min;
extern uint32_t	flex_ratio_max;
extern uint64_t	tsc_at_boot;

struct tscInfo {
    uint64_t	busFCvtt2n;
    uint64_t	busFCvtn2t;
    uint64_t	tscFreq;
    uint64_t	tscFCvtt2n;
    uint64_t	tscFCvtn2t;
    uint64_t	tscGranularity;
    uint64_t	bus2tsc;
    uint64_t	busFreq;
    uint32_t	flex_ratio;
    uint32_t	flex_ratio_min;
    uint32_t	flex_ratio_max;
};
typedef struct tscInfo tscInfo_t;

extern void tsc_get_info(tscInfo_t *info);
extern void tsc_init(void);

#endif /* __ASSEMBLY__ */
#endif /* tsc_h */
