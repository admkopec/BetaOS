//
//  machine_routines.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/26/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef machine_routines_h
#define machine_routines_h

#define AST_URGENT	0x04
#define T_PREEMPT   255
ast_t* ast_pending(void);

/* Get Interrupts Enabled */
bool ml_get_interrupts_enabled(void);
/* Set Interrupts Enabled */
bool ml_set_interrupts_enabled(bool enable);
/* Check if running at interrupt context */
bool ml_at_interrupt_context(void);
vm_offset_t ml_static_ptovirt(vm_offset_t paddr);

#endif /* machine_routines_h */
