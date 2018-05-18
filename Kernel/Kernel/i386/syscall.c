//
//  syscall.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/29/16.
//  Copyright © 2016-2018 Adam Kopeć. All rights reserved.
//

#include <stdint.h>
#include <stdbool.h>
#include "syscall.h"
#include "cpu_topology.h"
#include "cpu_data.h"
#include "cpuid.h"

#include "misc_protos.h"

void
unix_syscall64(__unused x86_saved_state_t *state) {
    serial_putc('S'); serial_putc('Y'); serial_putc('S'); serial_putc('C'); serial_putc('A'); serial_putc('L'); serial_putc('L'); serial_putc('\n');
}

void
mach_call_munger64(__unused x86_saved_state_t *state) {
    serial_putc('S'); serial_putc('Y'); serial_putc('S'); serial_putc('C'); serial_putc('A'); serial_putc('L'); serial_putc('L'); serial_putc('2'); serial_putc('\n');
}

void machdep_syscall64(__unused x86_saved_state_t *state) {
    serial_putc('S'); serial_putc('Y'); serial_putc('S'); serial_putc('C'); serial_putc('A'); serial_putc('L'); serial_putc('L'); serial_putc('3'); serial_putc('\n');
}
