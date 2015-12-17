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
// * add PIC and enable IRQ
// * change read() (start from waiting for IRQ 1 instead of polling
// * add PCI driver

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
