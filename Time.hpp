//
//  Time.hpp
//  OS
//
//  Created by Adam Kopeć on 11/30/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#ifndef Time_hpp
#define Time_hpp

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "interrupts.hpp"

class Time {
#define CURRENT_YEAR 2015
public:
    Time(){
        read_rtc();
    }
    
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned int year;
    
private:
    
    static const int century_register = 0x00;                                // Set by ACPI table parsing code if possible
    
    enum {
        cmos_address = 0x70,
        cmos_data    = 0x71
    };
    
    int get_update_in_progress_flag() {
        outb(cmos_address, 0x0A);
        return (inb(cmos_data) & 0x80);
    }
    
    unsigned char get_RTC_register(int reg) {
        outb(cmos_address, reg);
        return inb(cmos_data);
    }
    
    void read_rtc();
    
    };

#endif /* Time_hpp */
