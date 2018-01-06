//
//  thread_status.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/30/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef thread_status_h
#define thread_status_h
#ifndef __ASSEMBLY__
#include <stdint.h>
#include <stdbool.h>
#endif /* __ASSEMBLY__ */
/*
 * THREAD_STATE_FLAVOR_LIST 0
 * 	these are the supported flavors
 */
#define x86_THREAD_STATE32		1
#define x86_FLOAT_STATE32		2
#define x86_EXCEPTION_STATE32	3
#define x86_THREAD_STATE64		4
#define x86_FLOAT_STATE64		5
#define x86_EXCEPTION_STATE64	6
#define x86_THREAD_STATE		7
#define x86_FLOAT_STATE			8
#define x86_EXCEPTION_STATE		9
#define x86_DEBUG_STATE32		10
#define x86_DEBUG_STATE64		11
#define x86_DEBUG_STATE			12
#define THREAD_STATE_NONE		13
/* 14 and 15 are used for the internal x86_SAVED_STATE flavours */
#define x86_AVX_STATE32			16
#define x86_AVX_STATE64			(x86_AVX_STATE32 + 1)
#define x86_AVX_STATE			(x86_AVX_STATE32 + 2)

#define x86_SAVED_STATE32		THREAD_STATE_NONE + 1
#define x86_SAVED_STATE64		THREAD_STATE_NONE + 2
#ifndef __ASSEMBLY__

//typedef struct x86_kernel_state x86_kernel_state_t;
//
//struct thread_kernel_state {
//    x86_kernel_state_t      machine;           /* must be first */
////    kern_allocation_name_t  allocation_name;
//} __attribute__((aligned(16)));
//
//typedef struct thread_kernel_state * thread_kernel_state_t;

#define thread_get_kernel_state(thread) ((thread_kernel_state_t) \
((thread)->kernel_stack + kernel_stack_size - sizeof(struct thread_kernel_state)))

/*
 * Default segment register values.
 */

#define USER_CODE_SELECTOR    0x0017
#define USER_DATA_SELECTOR    0x001f
#define KERN_CODE_SELECTOR    0x0008
#define KERN_DATA_SELECTOR    0x0010

//typedef struct {
//    uint32_t    flavor;
//    uint32_t    count;
//} x86_state_header_t;
//
//struct x86_thread_state32 {
//    unsigned int    eax;
//    unsigned int    ebx;
//    unsigned int    ecx;
//    unsigned int    edx;
//    unsigned int    edi;
//    unsigned int    esi;
//    unsigned int    ebp;
//    unsigned int    esp;
//    unsigned int    ss;
//    unsigned int    eflags;
//    unsigned int    eip;
//    unsigned int    cs;
//    unsigned int    ds;
//    unsigned int    es;
//    unsigned int    fs;
//    unsigned int    gs;
//};
//typedef struct x86_thread_state32 x86_thread_state32_t;
//
//struct x86_thread_state64 {
//    uint64_t    rax;
//    uint64_t    rbx;
//    uint64_t    rcx;
//    uint64_t    rdx;
//    uint64_t    rdi;
//    uint64_t    rsi;
//    uint64_t    rbp;
//    uint64_t    rsp;
//    uint64_t    r8;
//    uint64_t    r9;
//    uint64_t    r10;
//    uint64_t    r11;
//    uint64_t    r12;
//    uint64_t    r13;
//    uint64_t    r14;
//    uint64_t    r15;
//    uint64_t    rip;
//    uint64_t    rflags;
//    uint64_t    cs;
//    uint64_t    fs;
//    uint64_t    gs;
//};
//typedef struct x86_thread_state64 x86_thread_state64_t;
//
//typedef struct {
//    x86_state_header_t tsh;
//    union {
//        x86_thread_state32_t ts32;
//        x86_thread_state64_t ts64;
//    } uts;
//} x86_thread_state_t;

/*
 * The format in which thread state is saved on this machine. This state
 * flavor is most efficient for exception RPC's to kernel-loaded servers,
 * because copying can be avoided:
 */
struct x86_saved_state32 {
    uint32_t	gs;
    uint32_t	fs;
    uint32_t	es;
    uint32_t	ds;
    uint32_t	edi;
    uint32_t	esi;
    uint32_t	ebp;
    uint32_t	cr2;	/* kernel esp stored by pusha - we save cr2 here later */
    uint32_t	ebx;
    uint32_t	edx;
    uint32_t	ecx;
    uint32_t	eax;
    uint16_t	trapno;
    uint16_t	cpu;
    uint32_t	err;
    uint32_t	eip;
    uint32_t	cs;
    uint32_t	efl;
    uint32_t	uesp;
    uint32_t	ss;
};
typedef struct x86_saved_state32 x86_saved_state32_t;

#pragma pack(4)

/*
 * This is the state pushed onto the 64-bit interrupt stack
 * on any exception/trap/interrupt.
 */
struct x86_64_intr_stack_frame {
    uint16_t	trapno;
    uint16_t	cpu;
    uint32_t 	_pad;
    uint64_t	trapfn;
    uint64_t	err;
    uint64_t	rip;
    uint64_t	cs;
    uint64_t	rflags;
    uint64_t	rsp;
    uint64_t	ss;
};
typedef struct x86_64_intr_stack_frame x86_64_intr_stack_frame_t;
/* Note: sizeof(x86_64_intr_stack_frame_t) must be a multiple of 16 bytes */

/*
 * thread state format for task running in 64bit long mode
 * in long mode, the same hardware frame is always pushed regardless
 * of whether there was a change in privlege level... therefore, there
 * is no need for an x86_saved_state64_from_kernel variant
 */
struct x86_saved_state64 {
    uint64_t	rdi;	/* arg0 for system call */
    uint64_t	rsi;
    uint64_t	rdx;
    uint64_t	r10;	/* R10 := RCX prior to syscall trap */
    uint64_t	r8;
    uint64_t	r9;		/* arg5 for system call */
    uint64_t	cr2;
    uint64_t	r15;
    uint64_t	r14;
    uint64_t	r13;
    uint64_t	r12;
    uint64_t	r11;
    uint64_t	rbp;
    uint64_t	rbx;
    uint64_t	rcx;
    uint64_t	rax;
    uint32_t	gs;
    uint32_t	fs;
    uint64_t 	_pad;
    struct	x86_64_intr_stack_frame	isf;
};
typedef struct x86_saved_state64 x86_saved_state64_t;

/*
 * Unified, tagged saved state:
 */
typedef struct {
    uint32_t			flavor;
    uint32_t			_pad_for_16byte_alignment[3];
    union {
        x86_saved_state32_t	ss_32;
        x86_saved_state64_t	ss_64;
    } uss;
} x86_saved_state_t;
#define	ss_32	uss.ss_32
#define	ss_64	uss.ss_64
#pragma pack()

static inline bool
is_saved_state64(x86_saved_state_t *iss) {
    return (iss->flavor == x86_SAVED_STATE64);
}

static inline bool
is_saved_state32(x86_saved_state_t *iss) {
    return (iss->flavor == x86_SAVED_STATE32);
}

static inline x86_saved_state32_t *
saved_state32(x86_saved_state_t *iss) {
    return &iss->ss_32;
}

static inline x86_saved_state64_t *
saved_state64(x86_saved_state_t *iss) {
    return &iss->ss_64;
}
#endif /* __ASSEMBLY__ */
#endif /* thread_status_h */
