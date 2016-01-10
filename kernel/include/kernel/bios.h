//
//  bios.h
//  OS
//
//  Created by Adam Kopeć on 1/10/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef bios_h
#define bios_h

#include <stdio.h>
#include <kernel/boot.h>

typedef struct Regs {
    /*
     * Subset of segment registers
     */
    
    uint16  ds;
    uint16  es;
    
    /*
     * CPU flags (Saved on BIOS exit, ignored on entry)
     */
    
    union {
        uint16 flags;
        uint32 eflags;
        struct {
            uint32 cf : 1;
            uint32 reserved_0 : 1;
            uint32 pf : 1;
            uint32 reserved_1 : 1;
            uint32 af : 1;
            uint32 reserved_2 : 1;
            uint32 zf : 1;
            uint32 sf : 1;
            uint32 tp : 1;
            uint32 intf : 1;
            uint32 df : 1;
            uint32 of : 1;
            uint32 iopl : 2;
            uint32 nt : 1;
            uint32 reserved_3 : 1;
            uint32 rf : 1;
            uint32 vm : 1;
            uint32 vif : 1;
            uint32 vip : 1;
            uint32 id : 1;
            uint32 reserved_4 : 10;
        };
    };
    
    /*
     * General purpose 32-bit registers, in the order expected by
     * pushad/popad.  Note that while most BIOS routines need only the
     * 16-bit portions of these registers, some 32-bit-aware routines
     * use them even in real mode.
     */
    
    union {
        uint32 edi;
        uint16 di;
    };
    union {
        uint32 esi;
        uint16 si;
    };
    union {
        uint32 ebp;
        uint16 bp;
    };
    union {           // Saved on BIOS exit, ignored on entry
        uint32 esp;
        uint16 sp;
    };
    
    union {
        uint32  ebx;
        uint16  bx;
        struct {
            uint8 bl;
            uint8 bh;
        };
    };
    union {
        uint32  edx;
        uint16  dx;
        struct {
            uint8 dl;
            uint8 dh;
        };
    };
    union {
        uint32  ecx;
        uint16  cx;
        struct {
            uint8 cl;
            uint8 ch;
        };
    };
    union {
        uint32  eax;
        uint16  ax;
        struct {
            uint8 al;
            uint8 ah;
        };
    };
} PACKED Regs;

/*
 * This is the communication area between the real-mode BIOS
 * and protected mode. Parts of it are used internally by this
 * module, but the 'userdata' area is available to the caller.
 */

struct BIOSShared {
    uint8 trampoline[512];
    uint8 stack[4096];
    uint8 stackTop[0];
    uint32 esp;
    struct {
        uint16 limit;
        uint32 base;
    } PACKED idtr16, idtr32;
    uint8 userdata[1024];
} PACKED;

#define BIOS_SHARED  ((struct BIOSShared*) BOOT_REALMODE_SCRATCH)

/*
 * Macros for converting between 32-bit and 16-bit near/far pointers.
 */

typedef uint32 far_ptr_t;

#define PTR_32_TO_NEAR(p, seg)   ((uint16)((uint32)(p) - ((seg) << 4)))
#define PTR_NEAR_TO_32(seg, off) ((void*)((((uint32)(seg)) << 4) + ((uint32)(off))))
#define PTR_FAR_TO_32(p)         PTR_NEAR_TO_32(p >> 16, p & 0xFFFF)

/*
 * Public entry point.
 */

fastcall void BIOS_Call(uint8 vector, Regs *regs);

#endif /* bios_h */
