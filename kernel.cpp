//
//  kernel.cpp
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "command.cpp"
#include <gdt.cpp>

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This kernel will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

//TODO:
// * add PIC and enable IRQ
// * change read() (start from waiting for IRQ 1 instead of polling
// * add PCI driver

void kernel_main() {
    /* Initialize terminal interface */
    Terminal();
    printf("Loading GDT \n");
    init_gdt();
    asm("	movw $0x18, %%ax \n \
        movw %%ax, %%ss \n \
        movl %0, %%esp"::"i" (KERN_STACK));
    printf("Hello!\nWelcome to BetaOS!\n");
    
    while (1) {
        //terminal.printf("BetaOS: ");*
        getcommand();
    }
}
