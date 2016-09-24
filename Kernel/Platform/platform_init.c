//
//  platform_init.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/26/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <platform/platform.h>
#include <platform/boot.h>
#include <platform/device_tree.h>
#include <i386/vm_types.h>
#include <i386/machine_routines.h>
#include <i386/misc_protos.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* private globals */
Platform_state_t  Platform_state;

/* Clock Frequency Info */
clock_frequency_info_t gClockFrequencyInfo;

void *gEFISystemTable;
void *gEFIRuntimeServices;

void identify_machine() {
    // Clear the gClockFrequencyInfo struct
    bzero((void *)&gClockFrequencyInfo, sizeof(clock_frequency_info_t));
    
    // Start with default values.
    gClockFrequencyInfo.timebase_frequency_hz = 1000000000;
    gClockFrequencyInfo.bus_frequency_hz      =  100000000;
    gClockFrequencyInfo.cpu_frequency_hz      =  300000000;
    kprintf("");
    gClockFrequencyInfo.bus_frequency_min_hz = gClockFrequencyInfo.bus_frequency_hz;
    gClockFrequencyInfo.bus_frequency_max_hz = gClockFrequencyInfo.bus_frequency_hz;
    gClockFrequencyInfo.cpu_frequency_min_hz = gClockFrequencyInfo.cpu_frequency_hz;
    gClockFrequencyInfo.cpu_frequency_max_hz = gClockFrequencyInfo.cpu_frequency_hz;
    kprintf("");
    gClockFrequencyInfo.dec_clock_rate_hz = gClockFrequencyInfo.timebase_frequency_hz;
    gClockFrequencyInfo.bus_clock_rate_hz = gClockFrequencyInfo.bus_frequency_hz;
    gClockFrequencyInfo.cpu_clock_rate_hz = gClockFrequencyInfo.cpu_frequency_hz;
    
    // Get real number from some where.
    kprintf("");
    // Set the num & den pairs form the hz values.
    gClockFrequencyInfo.bus_clock_rate_num = gClockFrequencyInfo.bus_clock_rate_hz;
    gClockFrequencyInfo.bus_clock_rate_den = 1;
    kprintf("");
    gClockFrequencyInfo.bus_to_cpu_rate_num =
    (2 * gClockFrequencyInfo.cpu_clock_rate_hz) / gClockFrequencyInfo.bus_clock_rate_hz;
    gClockFrequencyInfo.bus_to_cpu_rate_den = 2;
    kprintf("");
    gClockFrequencyInfo.bus_to_dec_rate_num = 1;
    gClockFrequencyInfo.bus_to_dec_rate_den =
    gClockFrequencyInfo.bus_clock_rate_hz / gClockFrequencyInfo.dec_clock_rate_hz;
}
// I'll use it when console is 'good enough'
int Platform_initialize_console(Platform_Video * info, int op) {
    static int   last_console = -1;
    if (info) {
        info->v_offset  = 0;
        info->v_length  = 0;
        info->v_display = GRAPHICS_MODE;
    }
    
    switch (op) {
            
        case kPDisableScreen:
            //initialize_screen(info, op);
            kprintf("kPDisableScreen %d\n", last_console);
            //if (!console_is_serial())
            //    last_console = switch_to_serial_console();
            break;
            
        case kPEnableScreen:
            //initialize_screen(info, op);
            if (info) Platform_state.video = *info;
            kprintf("kPEnableScreen %d\n", last_console);
            //if(last_console != -1)
            //    switch_to_old_console(last_console);
            break;
            
        case kPBaseAddressChange:
            if (info) Platform_state.video = *info;
            
        default:
            //initialize_screen(info, op);
            break;
    }
    
    return 0;
}

void Platform_init(bool vm_initialized, void * _args) {
    boot_args *args = (boot_args *)_args;
    
    if (Platform_state.initialized == false) {
        Platform_state.initialized  = true;
        
        Platform_state.bootArgs           = _args;
        Platform_state.deviceTreeHead	  = (void *) ml_static_ptovirt(args->deviceTreeP);
        Platform_state.video.v_baseAddr   = args->Video.v_baseAddr; // remains physical address
        Platform_state.video.v_rowBytes   = args->Video.v_rowBytes;
        Platform_state.video.v_width	  = args->Video.v_width;
        Platform_state.video.v_height	  = args->Video.v_height;
        Platform_state.video.v_depth	  = args->Video.v_depth;
        Platform_state.video.v_display    = args->Video.v_display;
        strlcpy(Platform_state.video.v_pixelFormat, "PPPPPPPP", sizeof(Platform_state.video.v_pixelFormat));
        if (args->flags & kBootArgsFlagHiDPI)
            Platform_state.video.v_scale = kPScaleFactor2x;
        else
            Platform_state.video.v_scale = kPScaleFactor1x;
    }
    if (!vm_initialized) {
        
        if (Platform_state.deviceTreeHead) {
            DTInit(Platform_state.deviceTreeHead);
        }
        identify_machine();
    } else {
        //pe_init_debug();
        kprintf("");
    }
    
}

void Platform_create_console(void) {
    if (Platform_state.video.v_display == GRAPHICS_MODE)
        Platform_initialize_console(&Platform_state.video, kPGraphicsMode);
    else
        Platform_initialize_console(&Platform_state.video, kPTextMode);
}

int Platform_current_console(Platform_Video * info) {
    *info = Platform_state.video;
    return (0);
}
