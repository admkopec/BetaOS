//
//  kernel.c
//  BetaOS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015-2016 Adam Kopeć. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <i386/asm.h>

#include <kernel/command.h>
#include <kernel/tty.h>

#include <modules/PS2Controller.h>

//TODO:
// * Add IDT
// * Add PIC and enable IRQ
// * Change read() (start from waiting for IRQ 1 instead of polling)
// * Add APM
// * Add PCI driver
// * Add SATA driver
// * Add Filesystem driver
// * Try replacing APM with ACPI

void kernel_early(void)
{
    terminal_initialize();
    CommandInit();
}

void kernel_main(void)
{
	printf("Hello!\nWelcome to BetaOS in x86_64 ELF mode :) !\n");
    char * username = "root";
    while (1) {
        printf("%s$ ", username);
        findcommand();
	}
}
