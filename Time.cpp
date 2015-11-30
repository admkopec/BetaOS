//
//  Time.cpp
//  OS
//
//  Created by Adam Kopeć on 11/30/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include "Time.hpp"

int Time::weekday() {
    
    unsigned int DataWeekday;
    unsigned int Weekday;
    
    interrupts.outb(0x70, 0x95);
    
    interrupts.outb(0x70, 6);
    DataWeekday = interrupts.inb(0x71);
    if(DataWeekday<6) Weekday = DataWeekday + 2;
    else Weekday = DataWeekday - 5;
    return Weekday;
}

unsigned int* Time::date() {
    
    unsigned int DataYear, DataMonth, DataDay;
    unsigned int date[3];
    
    interrupts.outb(0x70, 0x95);
    
    interrupts.outb(0x70, 9);
    DataYear = interrupts.inb(0x71);
    date[2] = DataYear - ((unsigned int) DataYear/16) * 6;
    
    interrupts.outb(0x70, 8);
    DataMonth = interrupts.inb(0x71);
    date[1] = DataMonth - ((unsigned int) DataMonth/16) * 6;
    
    interrupts.outb(0x70, 7);
    DataDay = interrupts.inb(0x71);
    date[0] = DataDay - ((unsigned int) DataDay/16) * 6;
    
    return date;
}

unsigned int* Time::time() {
    
    unsigned int DataHour, DataMinute, DataSecond;
    unsigned int time[3];
    
    interrupts.outb(0x70, 0x95);
    
    interrupts.outb(0x70, 4);
    DataHour = interrupts.inb(0x71);
    time[0] = DataHour - ((unsigned int) DataHour/16) * 6;
    
    interrupts.outb(0x70, 2);
    DataMinute = interrupts.inb(0x71);
    time[1] = DataMinute - ((unsigned int) DataMinute/16) * 6;
    
    interrupts.outb(0x70, 0);
    DataSecond = interrupts.inb(0x71);
    time[2] = DataSecond - ((unsigned int) DataSecond/16) * 6;
    
    return time;
}