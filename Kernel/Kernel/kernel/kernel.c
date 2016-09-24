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

#include <kernel/command.h>

#include "misc_protos.h"

//TODO:
// * Add IDT                    In progress....
// * Add PIC and enable IRQs    In progress....
// * Change pollchar() (start from waiting for IRQ 1 instead of polling)
// * Add APM or use ACPI if you want :)
// * Add PCI driver             Partialy done
// * Add SATA driver            Several tests have been conducted :)
// * Add Filesystem driver
// * Try replacing APM with ACPI if you haven't used ACPI option above

bool returnn;

void kernel_main()
{
    CommandInit();
    kprintf("Hello!\nWelcome to BetaOS in x86_64 MACH-O EFI mode :) !\n");
//#include <Modules/ModulesController.hpp>
    //ModulesStartController();
    char * username = "root";
    while (1) {
        if (returnn) {
            return;
        }
        kprintf("%s$ ", username);
        findcommand();
	}
}
