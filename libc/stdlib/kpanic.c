#include <stdio.h>
#include <stdlib.h>
#include <time.h>

__attribute__((__noreturn__))
void kpanic(const char* errormsg) {
  read_rtc();
	printf("\nKernel Panic: %d:%d:%d %d/%d/%d\n%s\nCPU halted", hour, minute, second, day, month, year, errormsg);
    asm("hlt");
}
