//
//  machine_routines.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/26/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef machine_routines_h
#define machine_routines_h

#include "pmap.h"

#define AST_URGENT	0x04
#define T_PREEMPT   255
ast_t* ast_pending(void);

/* Get Interrupts Enabled */
bool ml_get_interrupts_enabled(void);
/* Set Interrupts Enabled */
bool ml_set_interrupts_enabled(bool enable);
/* Check if running at interrupt context */
bool ml_at_interrupt_context(void);
vm_offset_t inline ml_static_ptovirt(vm_offset_t paddr) {
        return (vm_offset_t)(((unsigned long) paddr) | VM_MIN_KERNEL_ADDRESS);
}

#endif /* machine_routines_h */
