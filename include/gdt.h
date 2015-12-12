//
//  gdt.h
//  OS
//
//  Created by Adam Kopeć on 12/12/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef gdt_hpp
#define gdt_hpp

#include <stdio.cpp>
#include <stdint.h>
#define GDTSIZE 0xFF
#define GDTBASE 0x00000800
#define	KERN_STACK 0x0009FFF0

struct gdtr {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));

struct gdtdesc {
    uint16_t lim0_15;
    uint16_t base0_15;
    unsigned char base16_23;
    unsigned char access;
    unsigned char lim16_19:4;
    unsigned char other:4;
    unsigned char base24_31;
} __attribute__ ((packed));

struct tss {
    uint16_t previous_task, __previous_task_unused;
    uint32_t esp0;
    uint16_t ss0, __ss0_unused;
    uint32_t esp1;
    uint16_t ss1, __ss1_unused;
    uint32_t esp2;
    uint16_t ss2, __ss2_unused;
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t es, __es_unused;
    uint16_t cs, __cs_unused;
    uint16_t ss, __ss_unused;
    uint16_t ds, __ds_unused;
    uint16_t fs, __fs_unused;
    uint16_t gs, __gs_unused;
    uint16_t ldt_selector, __ldt_sel_unused;
    uint16_t debug_flag, io_map;
} __attribute__ ((packed));

void init_gdt_desc(uint32_t base, uint32_t limit, unsigned char access, unsigned char other, struct gdtdesc* desc);
void init_gdt(void);

#endif /* gdt_h */
