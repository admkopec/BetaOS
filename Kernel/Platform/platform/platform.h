//
//  platform.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef platform_h
#define platform_h

#include <stdint.h>
#include <stdbool.h>
#include <platform/boot.h>

typedef struct clock_frequency_info_t {
    unsigned long bus_clock_rate_hz;
    unsigned long cpu_clock_rate_hz;
    unsigned long dec_clock_rate_hz;
    unsigned long bus_clock_rate_num;
    unsigned long bus_clock_rate_den;
    unsigned long bus_to_cpu_rate_num;
    unsigned long bus_to_cpu_rate_den;
    unsigned long bus_to_dec_rate_num;
    unsigned long bus_to_dec_rate_den;
    unsigned long timebase_frequency_hz;
    unsigned long timebase_frequency_num;
    unsigned long timebase_frequency_den;
    unsigned long long bus_frequency_hz;
    unsigned long long bus_frequency_min_hz;
    unsigned long long bus_frequency_max_hz;
    unsigned long long cpu_frequency_hz;
    unsigned long long cpu_frequency_min_hz;
    unsigned long long cpu_frequency_max_hz;
    unsigned long long prf_frequency_hz;
    unsigned long long prf_frequency_min_hz;
    unsigned long long prf_frequency_max_hz;
    unsigned long long mem_frequency_hz;
    unsigned long long mem_frequency_min_hz;
    unsigned long long mem_frequency_max_hz;
    unsigned long long fix_frequency_hz;
} clock_frequency_info_t;

extern clock_frequency_info_t gClockFrequencyInfo;

typedef struct Boot_Video	Boot_Video;

/*  Scale factor values for Platform_Video.v_scale */
enum {
    kPScaleFactorUnknown = 0,
    kPScaleFactor1x      = 1,
    kPScaleFactor2x      = 2
};

struct Platform_Video {
    unsigned long   v_baseAddr;     /* Base address of video memory */
    unsigned long   v_rowBytes;     /* Number of bytes per pixel row */
    unsigned long   v_width;        /* Width */
    unsigned long   v_height;       /* Height */
    unsigned long   v_depth;        /* Pixel Depth */
    unsigned long   v_display;      /* Text or Graphics */
    char            v_pixelFormat[64];
    unsigned long	v_offset;       /* offset into video memory to start at */
    unsigned long	v_length;       /* length of video memory (0 for v_rowBytes * v_height) */
    unsigned char	v_rotate;       /* Rotation: 0:normal, 1:right 90, 2:left 180, 3:left 90 */
    unsigned char	v_scale;        /* Scale Factor for both X & Y */
    char            reserved1[2];
    long            reserved2;
};

typedef struct Platform_Video       Platform_Video;

#define kPGraphicsMode           1
#define kPTextMode               2
#define kPTextScreen             3
#define kPAcquireScreen          4
#define kPReleaseScreen          5
#define kPEnableScreen           6
#define kPDisableScreen          7
#define kPBaseAddressChange      8
#define kPRefreshBootGraphics	 9

typedef struct Platform_state {
    bool            initialized;
    Platform_Video  video;
    void*           deviceTreeHead;
    boot_args*      bootArgs;
} Platform_state_t;

extern Platform_state_t Platform_state;

extern void Platform_init(bool vm_initialized, void * _args);
extern int  Platform_initialize_console(Platform_Video * info, int op);
extern void Platform_create_console(void);
extern int  Platform_current_console(Platform_Video * info);

extern bool Parse_boot_argn(const char *arg_string, void *arg_ptr, int max_arg);

#endif /* platform_h */
