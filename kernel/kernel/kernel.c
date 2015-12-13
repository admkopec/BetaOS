#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <command.h>
#include <kernel/tty.h>

//TODO:
// * Add © to files
// * Add GDT
// * Add PIC

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(void)
{
	printf("Hello\n");
	while (1) {
		getcommand();
	}
}
