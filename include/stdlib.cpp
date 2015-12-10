//
//  stdlib.cpp
//  OS
//
//  Created by Adam Kopeć on 12/7/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include <stdlib.h>

void kpanic(const char* errormsg) {
    Time time;
    printf("Kernel Panic: %d:%d:%d %d/%d/%d\n\%s", time.hour, time.minute, time.second, time.day, time.month, time.year, errormsg);
    printf("\nCPU halted\n");
    __asm__("hlt");
}