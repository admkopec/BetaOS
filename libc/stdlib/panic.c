//
//  panic.c
//  OS
//
//  Created by Adam Kopeć on 12/7/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//__attribute__((__noreturn__))
void panic(const char* errormsg) {
    gettime();
	printf("\nKernel Panic: %d:%s%d:%s%d %s %d/%d/%d\n%s\nCPU halted", hour, zerom, minute, zeros, second, pmam, month, day, year, errormsg);
    asm("hlt");
}
