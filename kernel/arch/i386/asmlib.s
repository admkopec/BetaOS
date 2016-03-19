//
//  asmlib.s
//  BetaOS
//
//  Created by Adam Kopeć on 3/13/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <arch/asm.h>


    /* halt_cpu */

.text
.globl halt_cpu
.type  halt_cpu,@function
halt_cpu:
    sti
    hlt

    ret

    /* _cpuid */

.text
.globl _cpuid
.type  _cpuid,@function
_cpuid:
/* save work registers */
	push	%ebp
	push	%ebx

/* set eax parameter to cpuid and execute cpuid */
	movl	12(%esp), %ebp
	mov	(%ebp), %eax
	movl	16(%esp), %ebp
	mov	(%ebp), %ebx
	movl	20(%esp), %ebp
	mov	(%ebp), %ecx
	movl	24(%esp), %ebp
	mov	(%ebp), %edx

.byte	0x0F, 0xA2	/* CPUID */

/* store results in pointer arguments */
	movl	12(%esp), %ebp
	movl	%eax, (%ebp)
	movl	16(%esp), %ebp
	movl	%ebx, (%ebp)
	movl	20(%esp), %ebp
	movl	%ecx, (%ebp)
	movl	24(%esp), %ebp
	movl	%edx, (%ebp)

/* restore registers */
	pop	%ebx
	pop	%ebp

	ret

    /* getprocessor */

.text
.globl getprocessor
.type  getprocessor,@function
getprocessor:
    push	%ebp
	movl	%esp, %ebp
	andl	$0xFFFFFFFC, %esp	/* Align stack to avoid AC fault */
	movl	$0x00040000, %ecx	/* Try to flip the AC bit introduced on the 486 */
	call	flip
	movl	$386, %eax	/* 386 if it didn't react to "flipping" */
	je	gotprocessor
	movl	$0x00200000, %ecx	/* Try to flip the ID bit introduced on the 586 */
	call	flip
	movl	$486, %eax	/* 486 if it didn't react */
	je	gotprocessor
	pushf
	pusha	/* Save the world */
	movl	$1, %eax
.byte	0x0F, 0xA2	/* CPUID instruction tells the processor type */
	andb	$0x0F, %ah	/* Extract the family (5, 6, ...) */
	movzbl	%ah, %eax
	cmpl	$15, %eax	/* 15: extended family */
	jne	direct
	movl	$6, %eax	/* Make it 686 */
direct:
	imull	$100, %eax	/* 500, 600, ... */
	addl	$86, %eax	/* 586, 686, ... */
	movl	%eax, 7*4(%esp)	/* Pass eax through */
	popa
	popf
gotprocessor:
	leave
	ret

flip:
	pushf	/* Push eflags */
	pop	%eax	/* eax = eflags */
	movl	%eax, %edx	/* Save original eflags */
	xorl	%ecx, %eax	/* Flip the bit to test */
	push	%eax	/* Push modified eflags value */
	popf	/* Load modified eflags register */
	pushf
	pop	%eax	/* Get it again */
	push	%edx
	popf	/* Restore original eflags register */
	xorl	%edx, %eax	/* See if the bit changed */
	testl	%ecx, %eax
	ret

    /* x86_triplefault */

.text
.globl x86_triplefault
.type  x86_triplefault,@function
x86_triplefault:
    lidt    idt_zero
    int $3  /* Just to interrupt, CPU won't like it */
.data
idt_zero:
.long   0, 0
.text

    /* x86_lgdt */

.text
.globl x86_lgdt
.type  x86_lgdt,@function
x86_lgdt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    lgdt    (%eax)
    pop     %ebp
    ret

    /* x86_lidt */

.text
.globl x86_lidt
.type  x86_lgit,@function
x86_lidt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    lidtl   (%eax)
    pop     %ebp
    ret

    /* x86_ltr */

.text
.globl x86_ltr
.type  x86_ltr,@function
x86_ltr:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    ltr     8(%ebp)
    pop     %ebp
    ret

    /* x86_lldt */

.text
.globl x86_lldt
.type  x86_lldt,@function
x86_lldt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    lldt    8(%ebp)
    pop     %ebp
    ret

    /* x86_sgdt */

.text
.globl x86_sgdt
.type  x86_sgdt,@function
x86_sgdt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    sgdt    (%eax)
    pop     %ebp
    ret

    /* x86_sidt */

.text
.globl x86_sidt
.type  x86_sidt,@function
x86_sidt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    sidt    (%eax)
    pop     %ebp
    ret

    /* x86_load_ds */

.text
.globl x86_load_ds
.type  x86_load_ds,@function
x86_load_ds:
    push    %ebp
    mov     %esp, %ebp
	mov     8(%ebp), %eax
    mov     %eax, %ds
	jmp     0f /* a jump is required for some sets */
0: 	pop     %ebp
    ret

    /* x86_load_es */

.text
.globl x86_load_es
.type  x86_load_es,@function
x86_load_es:
    push    %ebp
    mov     %esp, %ebp
	mov     8(%ebp), %eax
    mov     %eax, %es
	jmp     0f /* a jump is required for some sets */
0: 	pop     %ebp
    ret

    /* x86_load_fs */

.text
.globl x86_load_fs
.type  x86_load_fs,@function
x86_load_fs:
    push    %ebp
    mov     %esp, %ebp
	mov     8(%ebp), %eax
    mov     %eax, %fs
	jmp     0f /* a jump is required for some sets */
0: 	pop     %ebp
    ret

    /* x86_load_gs */

.text
.globl x86_load_gs
.type  x86_load_gs,@function
x86_load_gs:
    push    %ebp
    mov     %esp, %ebp
	mov     8(%ebp), %eax
    mov     %eax, %gs
	jmp     0f /* a jump is required for some sets */
0: 	pop     %ebp
    ret

    /* x86_load_ss */

.text
.globl x86_load_ss
.type  x86_load_ss,@function
x86_load_ss:
    push    %ebp
    mov     %esp, %ebp
	mov     8(%ebp), %eax
    mov     %eax, %ss
	jmp     0f /* a jump is required for some sets */
0: 	pop     %ebp
    ret

    /* x86_load_kerncs */

/*.text
.globl x86_load_kerncs
.type  x86_load_kerncs,@function
x86_load_kerncs:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %eax
    jmp     $KERN_CS_SELECTOR, $newcs*/   /* Some problems here */
/*newcs:
    pop     %ebp
    ret*/
