//
//  tss.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef tss_h
#define tss_h

#include <stdint.h>

/*
 *	i386 Task State Segment
 */
struct i386_tss {
    uint32_t	back_link;	/* segment number of previous task, if nested */
    uint32_t	esp0;		/* initial stack pointer ... */
    uint32_t	ss0;		/* and segment for ring 0 */
    uint32_t	esp1;		/* initial stack pointer ... */
    uint32_t	ss1;		/* and segment for ring 1 */
    uint32_t	esp2;		/* initial stack pointer ... */
    uint32_t	ss2;		/* and segment for ring 2 */
    uint32_t	cr3;		/* CR3 - page table directory physical address */
    uint32_t	eip;
    uint32_t	eflags;
    uint32_t	eax;
    uint32_t	ecx;
    uint32_t	edx;
    uint32_t	ebx;
    uint32_t	esp;		/* current stack pointer */
    uint32_t	ebp;
    uint32_t	esi;
    uint32_t	edi;
    uint32_t	es;
    uint32_t	cs;
    uint32_t	ss;         /* current stack segment */
    uint32_t	ds;
    uint32_t	fs;
    uint32_t	gs;
    uint32_t	ldt;		/* local descriptor table segment */
    uint16_t	trace_trap;	/* trap on switch to this task */
    uint16_t	io_bit_map_offset;
    /* offset to start of IO permission bit map */
};

/*
 * Temporary stack used on kernel entry via the sysenter instruction.
 * Its top points on to the PCB save area. It must contain space for
 * a single interrupt stack frame in case of single-stepping over the sysenter.
 * Although this is defined as a 64-bit stack, the space is also used in
 * 32-bit legacy mode. For 64-bit the stack is 16-byte aligned.
 */
struct sysenter_stack {
    uint64_t	stack[16];	/* Space for a 64-bit frame and some */
    uint64_t	top;		/* Top and pointer to ISS in PCS */
};

#pragma pack(4)
struct x86_64_tss {
    uint32_t	reserved1;
    uint64_t	rsp0;		/* stack pointer for CPL0 */
    uint64_t	rsp1;		/* stack pointer for CPL1 */
    uint64_t	rsp2;		/* stack pointer for CPL2 */
    uint32_t	reserved2;
    uint32_t	reserved3;
    uint64_t	ist1;		/* interrupt stack table 1 */
    uint64_t	ist2;		/* interrupt stack table 2 */
    uint64_t	ist3;		/* interrupt stack table 3 */
    uint64_t	ist4;		/* interrupt stack table 4 */
    uint64_t	ist5;		/* interrupt stack table 5 */
    uint64_t	ist6;		/* interrupt stack table 6 */
    uint64_t	ist7;		/* interrupt stack table 7 */
    uint32_t	reserved4;
    uint32_t	reserved5;
    uint16_t	reserved6;
    uint16_t	io_bit_map_offset;
    /* offset to IO permission bit map */
};
#pragma pack()

#endif /* tss_h */
