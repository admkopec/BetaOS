//
//  kernel.c
//  BetaOS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015-2017 Adam Kopeć. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/command.h>
#include <i386/acpi.h>
#include <Modules/ModulesController.hpp>

#include "misc_protos.h"

//TODO:
// * Add APIC and enable IRQs   In progress.... (Timer - working, Rest - not)
// * Change pollchar() (start from waiting for IRQ 1 instead of polling)
// * Add PCI driver             Partialy done
// * Add SATA driver            Several tests have been conducted :)
// * Add Filesystem driver
// * Add ACPI                   Done (for now)

void kernel_main()
{
    //acpi();   Needs fix for PMAP as well as reboot, shutdown is "patched"
    ModulesStartController();
    CommandInit();
    kprintf("Hello!\nWelcome to BetaOS in x86_64 MACH-O EFI mode :) !\n");
    char * username = "root";
    while (1) {
        kprintf("%s$ ", username);
        findcommand();
	}
}
