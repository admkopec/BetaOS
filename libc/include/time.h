//
//  time.h
//  OS
//
//  Created by Adam Kopeć on 11/30/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#ifndef time_h
#define time_h

#include <stdbool.h>
#include <stddef.h>
#include <kernel/interrupts.h>

#define CURRENT_YEAR 2016

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;
char* pmam;
char* monthl;
char* dayofweekshort;
char* dayofweeklong;
char* zeros;
char* zerom;

static const int century_register = 0x00;                                // Set by ACPI table parsing code if possible

enum {
    cmos_address = 0x70,
    cmos_data    = 0x71
};

int get_update_in_progress_flag();

unsigned char get_RTC_register(int reg);

void read_rtc();

void gettime();

#endif /* time_h */
