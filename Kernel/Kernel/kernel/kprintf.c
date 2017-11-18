//
//  printf.c
//  BetaOS
//
//  Created by Adam Kopeć on 5/9/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <stdarg.h>
#include <string.h>
#include "misc_protos.h"
#include <i386/pal.h>
#include <i386/proc_reg.h>
#include <i386/machine_routines.h>

#include <stdio.h>
#include <time.h>

bool enable = true;
bool early  = false;

extern void vsputc(int ch);
extern bool experimental;

bool can_use_serial;
bool is_new_paging_enabled  = false;
int  __doprnt(const char	*fmt, va_list argp, void (*putc)(int), int radix, int is_log);

int
_doprnt_log(register const char	*fmt, va_list *argp, void (*putc)(int), int radix)/* default radix - for '%r' */;

int
_doprnt(register const char	*fmt, va_list *argp, void (*putc)(int), int radix)/* default radix - for '%r' */;

bool kRebootOnPanic = true;

extern void reboot_system(bool ispanic);
void __attribute__((noreturn))
panic(const char *errormsg, ...) {
    experimental = false;
    va_list list;
    printf("\nKERNEL PANIC: ");
    if (rdmsr64(MSR_IA32_GS_BASE) == 0 && enable == false) {
        early = true;
    }
    if (early) {
        va_start(list, errormsg);
        _doprnt_log(errormsg, &list, serial_putc, 16);
        va_end(list);
        goto panicing;
    } else {
        va_start(list, errormsg);
        _doprnt(errormsg, &list, vsputc, 16);
        va_end(list);
        goto panicing;
    }
panicing:
    if (!kRebootOnPanic) {
        printf("\nCPU Halted\n");
        pal_stop_cpu(true);
    } else {
        printf("\nRebooting in 3 seconds...\n");
        time_t start_time = time(&start_time);
        time_t tmp;
        while ((start_time + 3) > time(&tmp)) { }
        reboot_system(true);
    }
    for (; ;) { }
}

void hexdump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;
    
    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);
    
    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }
    
    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).
        
        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);
            
            // Output the offset.
            printf ("  %04X ", i);
        }
        
        // Now the hex code for the specific character.
        printf (" %02X", pc[i]);
        
        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    
    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }
    
    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}
