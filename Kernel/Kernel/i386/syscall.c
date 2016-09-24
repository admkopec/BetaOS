//
//  syscall.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/29/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
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
    
}

