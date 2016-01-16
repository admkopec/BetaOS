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
    read_rtc();
	printf("\nKernel Panic: %d:%d:%d %d/%d/%d\n%s\nCPU halted", hour, minute, second, day, month, year, errormsg);
    asm("hlt");
}
