#
#  start.s
#  BetaOS
#
#  Created by Adam Kopeć on 9/26/15 and modified for use with x86_64 CPUs on 5/2/16.
#  Copyright © 2015-2016 Adam Kopeć. All rights reserved.
#

.code32

#include <i386/asm.h>
#include <i386/seg.h>
#include <i386/proc_reg.h>

#define SWITCH_TO_64BIT_MODE					 \
	movl	$(CR4_PAE),%eax		/* enable PAE */	;\
	movl	%eax,%cr4					;\
	movl    $MSR_IA32_EFER,%ecx				;\
	rdmsr							;\
	/* enable long mode, NX */				;\
	orl	$(MSR_IA32_EFER_LME | MSR_IA32_EFER_NXE),%eax	;\
	wrmsr							;\
	movl	$EXT(BootPML4),%eax				;\
	movl	%eax,%cr3					;\
	movl	%cr0,%eax					;\
	orl	$(CR0_PG|CR0_WP),%eax	/* enable paging */	;\
	movl	%eax,%cr0					;\
	ljmpl	$KERNEL64_CS,$64f				;\
64:								;\
	.code64


# Declare constants used for creating a multiboot header.
.set FLAGS,    1<<0 | 1<<1 //| 1<<2   # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002           # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)     # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
//.section .text
/*Multiboot_Header:
.long 0xE85250D6    // Magic
.long 0             // Architecture
.long Multiboot_Header_END - Multiboot_Header // Header length
.long -(0xE85250D6 + 0 + (Multiboot_Header_END - Multiboot_Header)) // Checksum

.long 0         // Type
.long 0         // Flags
.long 8         // Size
//.long _start    // Entry
Multiboot_Header_END:*/
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long 0
//.long 1280, 768, 8

/* in the __HIB section since the hibernate restore code uses this stack. */
	//.section __HIB, __data
    //.section .bootstrap_stack, "aw", @nobits
		#ifdef __ELF__
		.section .data
		#else
        .section __DATA, __data
		#endif
	//.align	12

	.globl	EXT(low_intstack)
EXT(low_intstack):
	.globl  EXT(gIOHibernateRestoreStack)
EXT(gIOHibernateRestoreStack):

	.skip	16384 /* INTSTACK_SIZE */

	.globl	EXT(low_eintstack)
EXT(low_eintstack):
	.globl  EXT(gIOHibernateRestoreStackEnd)
EXT(gIOHibernateRestoreStackEnd):

	//.section __DATA, __data

// The kernel entry point.
.code32
    //.text
    //.section __HIB, __text
		#ifdef __ELF__
		.section .text
		#else
    .section __TEXT, __text
		#endif
    .align   ALIGN
    .globl   EXT(_start)
    .globl   EXT(pstart)
LEXT(_start)
LEXT(pstart)
	mov	%eax, %edi      /* save kernbootstruct */ /* For now doesn't make anything valuable */

	/* Use low 32-bits of address as 32-bit stack */
	movl	$EXT(low_eintstack), %esp

  movl	$EXT(protected_mode_gdtr), %eax
	lgdtl	(%eax)

	movl	$EXT(BootPML4), %eax						// Level 4:
	add	%eax, 0*8+0(%eax)									//  - 1:1
	add	%eax, /*KERNEL_PML4_INDEX*/511*8+0(%eax)	//  - kernel space

	movl	$EXT(BootPDPT), %edx						// Level 3:
	add	%eax, 0*8+0(%edx)
	add	%eax, 1*8+0(%edx)
	add	%eax, 2*8+0(%edx)
	add	%eax, 3*8+0(%edx)

	SWITCH_TO_64BIT_MODE

	xor	%eax, %eax
	mov	%ax, %ss
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs

    # Initialize the core kernel before running the global constructors.
	callq EXT(kernel_early)

	# Call the global constructors.
	//callq _init

	# Transfer control to the main kernel.
	callq EXT(kernel_main)

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang
#ifdef __ELF__
.size _start, . - _start
#endif

.code32
#ifdef __ELF__
.section .text
#else
.section __TEXT, __text
#endif
//.section __HIB, __text

Entry(protected_mode_gdtr)
	.short	160		/* limit (8*20 segs) */
	.quad	EXT(master_gdt)
