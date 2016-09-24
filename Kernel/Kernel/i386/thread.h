//
//  thread.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/7/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef thread_h
#define thread_h

#include <i386/thread_status.h>

#include <i386/seg.h>
#include <i386/tss.h>
#include <i386/cpu_data.h>

/*
 *	x86_kernel_state:
 *	This structure corresponds to the state of kernel registers
 *	as saved in a context-switch.  It lives at the base of the stack.
 */

struct x86_kernel_state {
    uint64_t	k_rbx;	/* kernel context */
    uint64_t	k_rsp;
    uint64_t	k_rbp;
    uint64_t	k_r12;
    uint64_t	k_r13;
    uint64_t	k_r14;
    uint64_t	k_r15;
    uint64_t	k_rip;
};


#endif /* thread_h */
