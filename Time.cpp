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
    
    outb(0x70, 0x95);
    
    outb(0x70, 6);
    DataWeekday = inb(0x71);
    if(DataWeekday<6) Weekday = DataWeekday + 2;
    else Weekday = DataWeekday - 5;
    return Weekday;
}

unsigned int* Time::date() {
    
    unsigned int DataYear, DataMonth, DataDay;
    unsigned int date[3];
    
    outb(0x70, 0x95);
    
    outb(0x70, 9);
    DataYear = inb(0x71);
    date[2] = DataYear - ((unsigned int) DataYear/16) * 6;
    
    outb(0x70, 8);
    DataMonth = inb(0x71);
    date[1] = DataMonth - ((unsigned int) DataMonth/16) * 6;
    
    outb(0x70, 7);
    DataDay = inb(0x71);
    date[0] = DataDay - ((unsigned int) DataDay/16) * 6;
    
    return date;
}

unsigned int* Time::time() {
    
    unsigned int DataHour, DataMinute, DataSecond;
    unsigned int time[3];
    
    outb(0x70, 0x95);
    
    outb(0x70, 4);
    DataHour = inb(0x71);
    time[0] = DataHour - ((unsigned int) DataHour/16) * 6;
    
    outb(0x70, 2);
    DataMinute = inb(0x71);
    time[1] = DataMinute - ((unsigned int) DataMinute/16) * 6;
    
    outb(0x70, 0);
    DataSecond = inb(0x71);
    time[2] = DataSecond - ((unsigned int) DataSecond/16) * 6;
    
    return time;
}