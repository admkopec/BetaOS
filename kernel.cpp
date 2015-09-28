//
//  kernel.cpp
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//
//

#include <stdbool.h> /* C doesn't have booleans by default. */
#include <stddef.h>
#include <stdint.h>
#include "terminal.cpp"
#include "interrupts.cpp"

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main() {
    /* Initialize terminal interface */
    Terminal terminal;
    Interrupts interrupts;
    
    /* Since there is no support for newlines in terminal_putchar
     * yet, '\n' will produce some VGA specific character instead.
     * This is normal.
     */
    //terminal.clearScreen();
    terminal.printf("Hello, kernel World!\nThat's an end of line\n");
    if (interrupts.are_interrupts_enabled()==true) {
        terminal.printf("Interrupts are enabled");
    }
    else if(interrupts.are_interrupts_enabled()==false) {
        terminal.printf("Interrupts are disabled");
    }
    else {
        terminal.printf("Interrupts are errored");
    }
    while (1) {
        const char* ch=terminal.read();
        /*terminal.printf("/n");
        terminal.printf(ch);*/
    }
}
