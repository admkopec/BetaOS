//
//  arch.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <arch/arch.h>
#include <arch/asm.h>

void reboot() {
    unsigned char good = 0x02;                    // Future: Move power () to apm.c
    while ((good & 0x02) != 0)
        good = inb(0x64);
    outb(0x64, 0xFE);
    
    /* Some checks if worked */
    
    if (0) {
        x86_triplefault();
    }
    
}

void shutdown() {
    printf("Will be added in version 1.1\nBeta will be available sooner\n");
}
