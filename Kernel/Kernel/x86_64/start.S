#
#  start.s
#  BetaOS
#
#  Created by Adam Kopeć on 9/26/15 and modified for use with x86_64 CPUs on 5/2/16.
#  Copyright © 2015-2016 Adam Kopeć. All rights reserved.
#

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

.code32

# Declare constants used for creating a multiboot header.
.set FLAGS,    1<<0 | 1<<1 //| 1<<2   # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002           # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)     # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
//Multiboot_Header:
//.long 0xE85250D6    // Magic
//.long 0             // Architecture
//.long Multiboot_Header_END - Multiboot_Header // Header length
//.long -(0xE85250D6 + 0 + (Multiboot_Header_END - Multiboot_Header)) // Checksum

//.long 0         // Type
//.long 0         // Flags
//.long 8         // Size
//.long __start    // Entry
//Multiboot_Header_END:
//.align 4
//Multiboot_Header:
//.long MAGIC
//.long FLAGS
//.long CHECKSUM
//.long Multiboot_Header, 0x100000, 0x10000000, 0, _start
//.long 0
//Multiboot_Header_END:
//.long 0
//.long 1280, 768, 8

#include <i386/asm.h>
#include <i386/seg.h>
#include <i386/proc_reg.h>
#include <i386/pmap.h>
#include <i386/vm_param.h>

/* in the __HIB section since the hibernate restore code uses this stack. */
    //.section .bootstrap_stack, "aw", @nobits
		#ifdef __ELF__
		.section .data
		#else
        .section __HIB,__data
		#endif
	//.align	12

	.globl	EXT(low_intstack)
EXT(low_intstack):
	.globl  EXT(gIOHibernateRestoreStack)
EXT(gIOHibernateRestoreStack):

	.space	INTSTACK_SIZE

	.globl	EXT(low_eintstack)
EXT(low_eintstack):
	.globl  EXT(gIOHibernateRestoreStackEnd)
EXT(gIOHibernateRestoreStackEnd):

	#ifdef __ELF__

    #else
    .section __DATA,__data
    #endif

/*
 * Stack for machine-check handler.
 */
	//.align	12
	.globl	EXT(mc_task_stack)
EXT(mc_task_stack):
	.space	INTSTACK_SIZE
	.globl	EXT(mc_task_stack_end)
EXT(mc_task_stack_end):

#define SWITCH_TO_64BIT_MODE                             \
	movl	$(CR4_PAE),%eax		/* enable PAE */        ;\
	movl	%eax,%cr4                                   ;\
	movl    $MSR_IA32_EFER,%ecx                         ;\
	rdmsr                                               ;\
	/* enable long mode, NX */                          ;\
	orl	$(MSR_IA32_EFER_LME | MSR_IA32_EFER_NXE),%eax	;\
	wrmsr                                               ;\
	movl	$EXT(BootPML4),%eax                         ;\
	movl	%eax,%cr3                                   ;\
	movl	%cr0,%eax                                   ;\
	orl	$(CR0_PG|CR0_WP),%eax	/* enable paging */     ;\
	movl	%eax,%cr0                                   ;\
	ljmpl	$KERNEL64_CS,$64f                           ;\
64:                                                     ;\
	.code64

// The kernel entry point.
.code32
		#ifdef __ELF__
		.section .text
		#else
    .section __HIB,__text
		#endif
    .align   ALIGN
    .globl   EXT(_start)
    .globl   EXT(pstart)
LEXT(_start)
LEXT(pstart)
	mov	%eax, %edi      /* save kernbootstruct */

	/* Use low 32-bits of address as 32-bit stack */
	movl	$EXT(low_eintstack), %esp

    movl	$EXT(protected_mode_gdtr), %eax
	lgdtl	(%eax)

	movl	$EXT(BootPML4), %eax						// Level 4:
	add	%eax, 0*8+0(%eax)								//  - 1:1
	add	%eax, KERNEL_PML4_INDEX*8+0(%eax)               //  - kernel space

	movl	$EXT(BootPDPT), %edx						// Level 3:
	add	%eax, 0*8+0(%edx)
	add	%eax, 1*8+0(%edx)
	add	%eax, 2*8+0(%edx)
	add	%eax, 3*8+0(%edx)

/* the following code is shared by the master CPU and all slave CPUs */
L_pstart_common:
    /* Switch to 64 bit mode */
	SWITCH_TO_64BIT_MODE
    /* Flush data segment registors */
	xor	%eax, %eax
	mov	%ax, %ss
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs

    test	%edi, %edi  /* Populate stack canary on BSP */
	jz	Lvstartshim

	mov	$1, %eax
	cpuid
	test	$(1 << 30), %ecx
	jz	Lnon_rdrand
	rdrand	%rax		/* RAX := 64 bits of DRBG entropy */
	jnc	Lnon_rdrand

Lstore_random_guard:
	xor	%ah, %ah	/* Security: zero second byte of stack canary */
	movq	%rax, ___stack_chk_guard(%rip)
                    /* %edi = boot_args_start if BSP */
Lvstartshim:

	/* %edi = boot_args_start */
	#ifdef __ELF__
    leaq	vstart(%rip), %rcx
    #else
	leaq	_vstart(%rip), %rcx
    #endif
	movq	$0xffffff8000000000, %rax	/* adjust pointer up high */
	or      %rax, %rsp                  /* and stack pointer up there */
	or      %rcx, %rax
	andq	$0xfffffffffffffff0, %rsp	/* align stack */
	xorq	%rbp, %rbp                  /* zero frame pointer */
	callq	*%rax

Lnon_rdrand:
	rdtsc /* EDX:EAX := TSC */
	/* Distribute low order bits */
	mov	%eax, %ecx
	xor	%al, %ah
	shl	$16, %rcx
	xor	%rcx, %rax
	xor	%eax, %edx

	/* Incorporate ASLR entropy, if any */
	lea	(%rip), %rcx
	shr	$21, %rcx
	movzbl	%cl, %ecx
	shl	$16, %ecx
	xor	%ecx, %edx

	mov	%ah, %cl
	ror	%cl, %edx /* Right rotate EDX (TSC&0xFF ^ (TSC>>8 & 0xFF))&1F */
	shl	$32, %rdx
	xor	%rdx, %rax
	mov	%cl, %al
	jmp	Lstore_random_guard

#ifdef __ELF__
.size _start, . - _start
#endif

/* Slave CPUs entry point */
	.align	ALIGN
	.globl	EXT(slave_pstart)
LEXT(slave_pstart)
	.code32
	cli                     /* disable interrupts, so we don`t need IDT for a while */
	movl	$EXT(mp_slave_stack) + PAGE_SIZE, %esp
	xor 	%edi, %edi		/* AP, no "kernbootstruct" */
	jmp	L_pstart_common		/* hop a ride to vstart() */
#ifdef __ELF__
.size slave_pstart, . - slave_pstart
#endif

.code32
#ifdef __ELF__
.section .text
#else
.section __HIB,__text
#endif

Entry(protected_mode_gdtr)
	.short	160		/* limit (8*20 segs) */
	.quad	EXT(master_gdt)
