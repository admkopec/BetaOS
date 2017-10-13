//
//  panic.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/7/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Temp values

// Default FILE * for stdio, faked values
// OSX symbol names
void *__stderrp = (void *)0xF2;
void *__stdinp  = (void *)0xF0;
void *__stdoutp = (void *)0xF1;

// Linux symbol names
void *stderr = (void *)0xF2;
void *stdin  = (void *)0xF0;
void *stdout = (void *)0xF1;

//__attribute__((__noreturn__))
/*void panic(const char* errormsg) {
    gettime();
	kprintf("\nKernel Panic: %d:%02d:%02d %s %d/%d/%d\n%s\nCPU halted", hour, minute, second, pmam, month, day, year, errormsg);
    asm("hlt");
}*/
