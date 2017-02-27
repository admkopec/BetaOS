//
//  arch.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015-2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <arch.h>
#include <i386/asm.h>
#include <i386/pio.h>
#include <i386/acpi.h>

void reboot() {
    //acpireboot();  // Needs troubleshooting
    
    printf("Using legacy reboot method.\n");
    
    unsigned char good = 0x02;                    // Future: Move power () to acpi.c
    while ((good & 0x02) != 0)
            good = inb(0x64);
    outb(0x64, 0xFE);
    
    x86_triplefault();

}

void shutdown() {
    acpipoweroff();
    printf("Something went wrong!\nYou can now manually switch off your computer.");
    halt_cpu();
}
