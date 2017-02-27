//
//  machine_routines.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/26/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include "cpuid.h"
#include "cpu_data.h"
#include "proc_reg.h"
#include "pmap.h"
#include "eflags.h"
#include "machine_routines.h"

#include "misc_protos.h"

ast_t* ast_pending(void) {
    return (&current_cpu_datap()->cpu_pending_ast);
}

/* Get Interrupts Enabled */
bool ml_get_interrupts_enabled(void) {
    unsigned long flags;
    __asm__ volatile("pushf; pop	%0" :  "=r" (flags));
    return (flags & EFL_IF) != 0;
}

/* Set Interrupts Enabled */
bool ml_set_interrupts_enabled(bool enable) {
    unsigned long flags;
    boolean_t     istate;
    
    __asm__ volatile("pushf; pop	%0" :  "=r" (flags));
    
    assert(get_interrupt_level() ? (enable == FALSE) : TRUE);
    
    istate = ((flags & EFL_IF) != 0);
    
    if (enable) {
        __asm__ volatile("sti;nop");
        
        if ((get_preemption_level() == 0) && (*ast_pending() & AST_URGENT))
            __asm__ volatile ("int %0" :: "N" (T_PREEMPT));
    }
    else {
        if (istate)
            __asm__ volatile("cli");
    }
    
    return istate;
}

/* Check if running at interrupt context */
bool ml_at_interrupt_context(void) {
    return get_interrupt_level() != 0;
}
