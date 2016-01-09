//
//  kernel.c
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <command.h>
#include <kernel/tty.h>

//TODO:
// * Add GDT
// * Add IDT
// * Add PIC and enable IRQ
// * Change read() (start from waiting for IRQ 1 instead of polling)
// * Change read() (use keymap instead of switch)
// * Add APM
// * Add PCI driver
// * Add SATA driver
// * Add Filesystem driver
// * Try replacing APM with ACPI

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(void)
{
	printf("Hello!\nWelcome to BetaOS!\n");
	while (1) {
		getcommand();
	}
}
