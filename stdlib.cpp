//
//  stdlib.cpp
//  OS
//
//  Created by Adam Kopeć on 12/7/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include "stdlib.h"
#include "stdio.cpp"
#include "terminal.cpp"
#include "Time.cpp"

void kpanic(const char* errormsg) {
    Time time;
    printf("Kernel Panic: %d:%d:%d %d/%d/%d\n\%s", time.hour, time.minute, time.second, time.day, time.month, time.year, errormsg);
    __asm__("hlt");
}