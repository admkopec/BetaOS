//
//  arch.cpp
//  OS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include "arch.hpp"

//idtdesc 	kidt[IDTSIZE]; 		/* IDT table */
//int_desc 	intt[IDTSIZE]; 		/* Interruptions functions tables */
//tss 		default_tss;
//idtr 		kidtr; 				/* IDTR registry */
uint32_t *		stack_ptr=0;
static char cpu_name[512] = "x86-noname";

char* cpuid() {
    return cpu_name;
}

void reboot() {
    unsigned char good = 0x02;
    while ((good & 0x02) != 0)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

void shutdown() {
    
}

/*void Initialize_arch() {
    printf("Architecture x86, cpu=%s \n", cpuid());
    
    printf("Loading GDT \n");
    init_gdt();
    asm("	movw $0x18, %%ax \n \
        movw %%ax, %%ss \n \
        movl %0, %%esp"::"i" (KERN_STACK));
    
    //printf("Loading IDT \n");
    //init_idt();
    
    
    printf("Configure PIC \n");
    init_pic();
    
    printf("Loading Task Register \n");
    asm("	movw $0x38, %ax; ltr %ax");
}*/

void *memcpy(char *dst, char *src, int n)
{
    char *p = dst;
    while (n--)
    *dst++ = *src++;
    return p;
}

/*void init_idt_desc(uint16_t select, uint32_t offset, uint16_t type, struct idtdesc *desc)
{
    desc->offset0_15 = (offset & 0xffff);
    desc->select = select;
    desc->type = type;
    desc->offset16_31 = (offset & 0xffff0000) >> 16;
}

void init_idt(void)
{
    /* Init irq */
    //int i;
    //for (i = 0; i < IDTSIZE; i++)
    //init_idt_desc(0x08, (uint32_t)_asm_schedule, INTGATE, &kidt[i]); //
    /* Vectors  0 -> 31 are for exceptions */
    //init_idt_desc(0x08, (uint32_t) _asm_exc_GP, INTGATE, &kidt[13]);        /* #GP */
    //init_idt_desc(0x08, (uint32_t) _asm_exc_PF, INTGATE, &kidt[14]);     /* #PF */
    
    /*init_idt_desc(0x08, (uint32_t) _asm_schedule, INTGATE, &kidt[32]);
    init_idt_desc(0x08, (uint32_t) _asm_int_1, INTGATE, &kidt[33]);
    
    init_idt_desc(0x08, (uint32_t) _asm_syscalls, TRAPGATE, &kidt[48]);
    init_idt_desc(0x08, (uint32_t) _asm_syscalls, TRAPGATE, &kidt[128]); //48
    
    kidtr.limit = IDTSIZE * 8;
    kidtr.base = IDTBASE;
    
    
    /* Copy the IDT to the memory */
    //memcpy((char *) kidtr.base, (char *) kidt, kidtr.limit);
    
    /* Load the IDTR registry */
    //__asm__("lidtl (kidtr)");
//}

/*void init_pic(void)
{
    /* Initialization of ICW1 */
    /*outb(0x20, 0x11);
    outb(0xA0, 0x11);
    
    /* Initialization of ICW2 */
    //outb(0x21, 0x20);    /* start vector = 32 */
    //outb(0xA1, 0x70);    /* start vector = 96 */
    
    /* Initialization of ICW3 */
    /*outb(0x21, 0x04);
    outb(0xA1, 0x02);
    
    /* Initialization of ICW4 */
    /*outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    /* mask interrupts */
    /*outb(0x21, 0x0);
    outb(0xA1, 0x0);
}*/