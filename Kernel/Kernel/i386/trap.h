//
//  trap.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/7/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef trap_h
#define trap_h

/*
 * Hardware trap vectors for i386.
 */
#define	T_DIVIDE_ERROR          0
#define	T_DEBUG                 1
#define	T_NMI                   2		/* non-maskable interrupt */
#define	T_INT3                  3		/* int 3 instruction */
#define	T_OVERFLOW              4		/* overflow test */
#define	T_OUT_OF_BOUNDS         5		/* bounds check */
#define	T_INVALID_OPCODE        6		/* invalid op code */
#define	T_NO_FPU                7		/* no floating point */
#define	T_DOUBLE_FAULT          8		/* double fault */
#define	T_FPU_FAULT             9
#define T_INVALID_TSS           10
#define	T_SEGMENT_NOT_PRESENT	11
#define	T_STACK_FAULT           12
#define	T_GENERAL_PROTECTION	13
#define	T_PAGE_FAULT            14
                             /* 15 */
#define	T_FLOATING_POINT_ERROR	16
#define	T_WATCHPOINT            17
#define T_MACHINE_CHECK         18
#define T_SSE_FLOAT_ERROR       19
                             /* 20-126 */
#define T_DTRACE_RET            127

/* The SYSENTER and SYSCALL trap numbers are software constructs.
 * These exceptions are dispatched directly to the system call handlers.
 */
#define	T_SYSENTER		0x84
#define	T_SYSCALL		0x85
#define T_PREEMPT		255

#define TRAP_NAMES  "divide error",         "debug trap",           "NMI",                  \
                    "breakpoint",           "overflow",             "bounds check",         \
                    "invalid opcode",       "no coprocessor",       "double fault",         \
                    "coprocessor overrun",  "invalid TSS",          "segment not present",  \
                    "stack bounds",         "general protection",   "page fault",           \
                    "(reserved)",           "coprocessor error",    "watchpoint",           \
                    "machine check",        "SSE floating point"
/*
 * Page-fault trap codes.
 */
#define	T_PF_PROT		0x1		/* protection violation */
#define	T_PF_WRITE		0x2		/* write access */
#define	T_PF_USER		0x4		/* from user state */

#define	T_PF_RSVD		0x8		/* reserved bit set to 1 */
#define T_PF_EXECUTE	0x10	/* instruction fetch when NX */

#ifndef __ASSEMBLY__
#include <i386/thread.h>

//extern void		i386_exception                  (int exc, mach_exception_code_t code, mach_exception_subcode_t subcode);
extern void		sync_iss_to_iks                 (x86_saved_state_t *regs);
extern void		sync_iss_to_iks_unconditionally (x86_saved_state_t *regs);
extern void		kernel_trap                     (x86_saved_state_t *regs, uintptr_t *lo_spp);
extern void		user_trap                       (x86_saved_state_t *regs);
extern void		interrupt                       (x86_saved_state_t *regs);
extern void		panic_double_fault64            (x86_saved_state_t *regs);
extern void		panic_machine_check64           (x86_saved_state_t *regs);
extern void		i386_astintr                    (int preemption);

typedef kern_return_t (*perfCallback)   (int trapno, void *regs,  uintptr_t *lo_spp, int);
typedef kern_return_t (*perfASTCallback)(ast_t reasons, ast_t *myast);

extern volatile perfCallback    perfTrapHook;
extern volatile perfASTCallback perfASTHook;
extern volatile perfCallback    perfIntHook;

//extern void	panic_i386_backtrace(void *, int, const char *, bool, x86_saved_state_t *);
//extern void   print_one_backtrace(pmap_t pmap, vm_offset_t topfp, const char *cur_marker,	bool is_64_bit, bool nvram_format);
//extern void	print_thread_num_that_crashed(task_t task);
//extern void	print_tasks_user_threads(task_t task);
//extern void	print_threads_registers(thread_t thread);
//extern void	print_uuid_info(task_t task);
//extern void	print_launchd_info(void);
#endif /* __ASSEMBLY__ */

#endif /* trap_h */
