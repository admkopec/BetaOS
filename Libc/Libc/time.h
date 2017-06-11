//
//  time.h
//  BetaOS
//
//  Created by Adam Kopeć on 11/30/15.
//  Copyright © 2015-2017 Adam Kopeć. All rights reserved.
//

#pragma once
#ifndef time_h
#define time_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CURRENT_CENTURY 21

extern unsigned char second;
extern unsigned char minute;
extern unsigned char hour;
extern unsigned char day;
extern unsigned char month;
extern unsigned int year;
extern char* pmam;
extern char* monthl;
extern char* dayofweekshort;
extern char* dayofweeklong;

typedef int64_t absolute_time_t;
typedef absolute_time_t time_t;

static const int century_register = 0x00;                                // Set by ACPI table parsing code
                                                                         // TODO: initialize in acpi()
enum {
    cmos_address = 0x70,
    cmos_data    = 0x71
};

int get_update_in_progress_flag(void);

unsigned char get_RTC_register(int reg);

void read_rtc(void);

void gettime(void);

time_t time(void);

#endif /* time_h */
