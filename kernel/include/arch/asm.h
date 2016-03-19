//
//  asm.h
//  BetaOS
//
//  Created by Adam Kopeć on 3/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef asm_h
#define asm_h

#if !defined(_ALIGN_TEXT) && !defined(_KERNEL)
# ifdef _STANDALONE
#  define _ALIGN_TEXT .align 1
# elif defined __ELF__
#  define _ALIGN_TEXT .align 16
# else
#  define _ALIGN_TEXT .align 4
# endif
#else
# define _ALIGN_TEXT .align 16      // Just untill not propearly implemented
#endif

#define _ENTRY(x) .text; _ALIGN_TEXT; .globl x; .type x,@function; x:
#define _LABEL(x) \
    .globl x; x:

/* #define ENTRY(y)    _ENTRY(_C_LABEL(y)); _PROF_PROLOGUE */   // To be implemented in future

/* Table sizes. */
#define IDT_SIZE            256  /* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#define KERN_CS_INDEX        1
#define KERN_DS_INDEX        2
#define USER_CS_INDEX        3
#define USER_DS_INDEX        4
#define LDT_INDEX            5
#define TSS_INDEX_FIRST      6
#define TSS_INDEX(cpu)       (TSS_INDEX_FIRST + (cpu)) /* per cpu kernel tss */
#define GDT_SIZE             (TSS_INDEX(CONFIG_MAX_CPUS))	/* LDT descriptor */

#define SEG_SELECTOR(i)     ((i)*8)
#define KERN_CS_SELECTOR    SEG_SELECTOR(KERN_CS_INDEX)
#define KERN_DS_SELECTOR    SEG_SELECTOR(KERN_DS_INDEX)
#define USER_CS_SELECTOR    (SEG_SELECTOR(USER_CS_INDEX) | USER_PRIVILEGE)
#define USER_DS_SELECTOR    (SEG_SELECTOR(USER_DS_INDEX) | USER_PRIVILEGE)
#define LDT_SELECTOR        SEG_SELECTOR(LDT_INDEX)
#define TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_INDEX(cpu))

#define DESC_SIZE           8

/* Privileges. */
#define INTR_PRIVILEGE      0	/* kernel and interrupt handlers */
#define USER_PRIVILEGE      3	/* servers and user processes */
#define RPL_MASK            0x03	/* bits in selector RPL */

void halt_cpu(void);
int  getprocessor(void);
void x86_triplefault();
void x86_lgdt(void *);
void x86_lldt(unsigned long);
void x86_ltr(unsigned long);
void x86_lidt(void *);
//void x86_load_kerncs(void); /* Implement! */
void x86_load_ds(unsigned long);
void x86_load_ss(unsigned long);
void x86_load_es(unsigned long);
void x86_load_fs(unsigned long);
void x86_load_gs(unsigned long);
void _cpuid(unsigned long *eax, unsigned long *ebx, unsigned long *ecx, unsigned long *edx); /* For now prototype'll be here */

#endif /* asm_h */
