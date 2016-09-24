//
//  arch.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015-2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <arch.h>
#include <i386/asm.h>
#include <i386/pio.h>

void reboot() {
    unsigned char good = 0x02;                    // Future: Move power () to apm.c
    while ((good & 0x02) != 0)
            good = inb(0x64);
    outb(0x64, 0xFE);

    /* Some checks if worked */

    if (/* DISABLES CODE */ (0)) {
        x86_triplefault();
    }

}

void shutdown() {
    printf("Will be added in version 1.0\nBeta will be available sooner\n");
}