//
//  InterruptController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 6/6/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "InterruptController.hpp"
#include <i386/pio.h>
#include <i386/lapic.h>

#define Log(x ...) printf("InterruptController: " x)
#ifdef DEBUG
#define DBG(x ...) Log(x)
#else
#define DBG(x ...)
#endif

OSReturn
Interrupt::Register(int InterruptLine, Controller* handler) {
    if (InterruptLine > 0xF) {
        Log("Invalid Interrupt Line value - %x!\n", InterruptLine);
        return kOSReturnError;
    }
    if (handler == NULL) {
        Log("Handler not set!\n");
        return kOSReturnError;
    }
    if (Table[InterruptLine].isSet) {
        Log("Interrupt is already registered!\n");
        return kOSReturnError;
    }
    Table[InterruptLine].handler = handler;
    int  port;
    char value;
    if (InterruptLine <= 7) {
        port = 0x21;
    } else {
        port = 0xA1;
        InterruptLine -= 7;
    }
    value = inb(port) & ~(1 << InterruptLine);
    outb(port, value);
    DBG("IRQ: %d registered!\n", InterruptLine);
    Table[InterruptLine].isSet = true;
    return kOSReturnSuccess;
}

OSReturn
Interrupt::Disable(int InterruptLine) {
    if (InterruptLine > 0xF) {
        Log("Invalid Interrupt Line\n");
        return kOSReturnError;
    }
    Table[InterruptLine].handler = NULL;
    int  port;
    char value;
    if (InterruptLine <= 7) {
        port = 0x21;
    } else {
        port = 0xA1;
        InterruptLine -= 7;
    }
    value = inb(port) | (1 << InterruptLine);
    outb(port, value);
    Table[InterruptLine].isSet = false;
    return kOSReturnSuccess;
}

OSReturn
Interrupt::Incomming(int InterruptLine) {
    if (InterruptLine == 1 || InterruptLine == 0) {
        // Just go through to get the CPU out of the hlt in getchar()
        return kOSReturnSuccess;
    }
    if (!Table[InterruptLine].isSet) {
        Log("Interrupt Handler not set (Line: %d, Number: 0x%X)!\n", InterruptLine, InterruptLine + LAPIC_DEFAULT_INTERRUPT_BASE);
        return kOSReturnError;
    }
    Table[InterruptLine].handler->handleInterrupt();
    return kOSReturnSuccess;
}

void
IncommingInterrupt(int InterruptNumber) {
    InterruptNumber -= LAPIC_DEFAULT_INTERRUPT_BASE;
    Interrupt::Incomming(InterruptNumber);
}

TableEntry Interrupt::Table[20];
