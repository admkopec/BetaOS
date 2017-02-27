//
//  trap.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/7/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
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

/*
 *      Machine-independent exception definitions.
 */

#define EXC_BAD_ACCESS          1       /* Could not access memory */
                                        /* Code contains kern_return_t describing error. */
                                        /* Subcode contains bad memory address. */

#define EXC_BAD_INSTRUCTION     2       /* Instruction failed */
                                        /* Illegal or undefined instruction or operand */
#define EXC_ARITHMETIC          3       /* Arithmetic exception */
                                        /* Exact nature of exception is in code field */
#define EXC_EMULATION           4       /* Emulation instruction */
                                        /* Emulation support instruction encountered */
                                        /* Details in code and subcode fields   */

#define EXC_SOFTWARE            5       /* Software generated exception */
                                        /* Exact exception is in code field. */
                                        /* Codes 0 - 0xFFFF reserved to hardware */
                                        /* Codes 0x10000 - 0x1FFFF reserved for OS emulation (Unix) */
#define EXC_BREAKPOINT          6       /* Trace, breakpoint, etc. */
                                        /* Details in code field. */
#define EXC_SYSCALL             7       /* System calls. */
#define EXC_MACH_SYSCALL        8       /* Mach system calls. */
#define EXC_RPC_ALERT           9       /* RPC alert */
#define EXC_CRASH               10      /* Abnormal process exit */

/*
 *      Machine-independent exception behaviors
 */

# define EXCEPTION_DEFAULT              1
/*      Send a catch_exception_raise message including the identity.
 */

# define EXCEPTION_STATE                2
/*      Send a catch_exception_raise_state message including the
 *      thread state.
 */

# define EXCEPTION_STATE_IDENTITY       3
/*      Send a catch_exception_raise_state_identity message including
 *      the thread identity and state.
 */

#define EXCEPTION_CODES            0x80000000
/*      Send 64-bit code and subcode in the exception header */

/*
 * Masks for exception definitions, above
 * bit zero is unused, therefore 1 word = 31 exception types
 */

#define EXC_MASK_BAD_ACCESS             (1 << EXC_BAD_ACCESS)
#define EXC_MASK_BAD_INSTRUCTION        (1 << EXC_BAD_INSTRUCTION)
#define EXC_MASK_ARITHMETIC             (1 << EXC_ARITHMETIC)
#define EXC_MASK_EMULATION              (1 << EXC_EMULATION)
#define EXC_MASK_SOFTWARE               (1 << EXC_SOFTWARE)
#define EXC_MASK_BREAKPOINT             (1 << EXC_BREAKPOINT)
#define EXC_MASK_SYSCALL                (1 << EXC_SYSCALL)
#define EXC_MASK_MACH_SYSCALL           (1 << EXC_MACH_SYSCALL)
#define EXC_MASK_RPC_ALERT              (1 << EXC_RPC_ALERT)
#define EXC_MASK_CRASH                  (1 << EXC_CRASH)

#define EXC_MASK_ALL    (EXC_MASK_BAD_ACCESS |                  \
                         EXC_MASK_BAD_INSTRUCTION |             \
                         EXC_MASK_ARITHMETIC |                  \
                         EXC_MASK_EMULATION |                   \
                         EXC_MASK_SOFTWARE |                    \
                         EXC_MASK_BREAKPOINT |                  \
                         EXC_MASK_SYSCALL |                     \
                         EXC_MASK_MACH_SYSCALL |                \
                         EXC_MASK_RPC_ALERT |                   \
                         EXC_MASK_MACHINE)

#define EXC_MASK_VALID  (EXC_MASK_ALL | EXC_MASK_CRASH)

#define FIRST_EXCEPTION         1       /* ZERO is illegal */

/*
 * Machine independent codes for EXC_SOFTWARE
 * Codes 0x10000 - 0x1FFFF reserved for OS emulation (Unix)
 * 0x10000 - 0x10002 in use for unix signals
 */
#define EXC_SOFT_SIGNAL         0x10003 /* Unix signal exceptions */

#define EXC_TYPES_COUNT         11      /* incl. illegal exception 0 */

/*
 *      Codes and subcodes for 80386 exceptions.
 */

#define EXCEPTION_CODE_MAX      2       /* currently code and subcode */

/*
 *      EXC_BAD_INSTRUCTION
 */

#define EXC_I386_INVOP                  1

/*
 *      EXC_ARITHMETIC
 */

#define EXC_I386_DIV                    1
#define EXC_I386_INTO                   2
#define EXC_I386_NOEXT                  3
#define EXC_I386_EXTOVR                 4
#define EXC_I386_EXTERR                 5
#define EXC_I386_EMERR                  6
#define EXC_I386_BOUND                  7
#define EXC_I386_SSEEXTERR              8
#define EXC_I386_SGL                    1
#define EXC_I386_BPT                    2
#define EXC_I386_DIVERR                 0       /* divide by 0 eprror           */
#define EXC_I386_SGLSTP                 1       /* single step                  */
#define EXC_I386_NMIFLT                 2       /* NMI                          */
#define EXC_I386_BPTFLT                 3       /* breakpoint fault             */
#define EXC_I386_INTOFLT                4       /* INTO overflow fault          */
#define EXC_I386_BOUNDFLT               5       /* BOUND instruction fault      */
#define EXC_I386_INVOPFLT               6       /* invalid opcode fault         */
#define EXC_I386_NOEXTFLT               7       /* extension not available fault*/
#define EXC_I386_DBLFLT                 8       /* double fault                 */
#define EXC_I386_EXTOVRFLT              9       /* extension overrun fault      */
#define EXC_I386_INVTSSFLT              10      /* invalid TSS fault            */
#define EXC_I386_SEGNPFLT               11      /* segment not present fault    */
#define EXC_I386_STKFLT                 12      /* stack fault                  */
#define EXC_I386_GPFLT                  13      /* general protection fault     */
#define EXC_I386_PGFLT                  14      /* page fault                   */
#define EXC_I386_EXTERRFLT              16      /* extension error fault        */
#define EXC_I386_ALIGNFLT               17      /* Alignment fault */
#define EXC_I386_ENDPERR                33      /* emulated extension error flt */
#define EXC_I386_ENOEXTFLT              32      /* emulated ext not present     */

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
