//
//  InterruptController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 6/6/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "InterruptController.hpp"
#include <i386/pio.h>

int Interrupt::RegisterInterrupt(int InterruptLine, __unused void (*handler)()) {
    if (InterruptLine > 0xF) {
        printf("InterruptController: Invalid Interrupt Line value - %X!\n", InterruptLine);
        return -1;
    }
    int  port;
    char value;
    if (InterruptLine < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        InterruptLine -= 8;
    }
    value = inb(port) & ~(1 << InterruptLine);
    outb(port, value);
    return 0;
}
