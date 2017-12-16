//
//  asmlib.s
//  BetaOS
//
//  Created by Adam Kopeć on 3/13/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <i386/asm.h>

#define PAL_RTC_TSC_BASE    0x0
#define PAL_RTC_NS_BASE     0x8
#define PAL_RTC_SCALE       0x10
#define PAL_RTC_SHIFT       0x14
#define PAL_RTC_GENERATION  0x18

Entry(x86_64_context_switch)
    /* save the old context and restore the new */

    pushq    %gs
    pushq    %fs
    pushq    %rbp
    pushq    %r15
    pushq    %r14
    pushq    %r13
    pushq    %r12
    pushq    %r11
    pushq    %r10
    pushq    %r9
    pushq    %r8
    pushq    %rdi
    pushq    %rsi
    pushq    %rdx
    pushq    %rcx
    pushq    %rbx
    pushq    %rax
    xor      %rax, %rax
    mov      %ax, %ds
    pushq    %rax
    mov      %ax, %es
    pushq    %rax

    movq  %rsp, 8(%rdi)
    movq 8(%rsi), %rsp

    popq     %rax
    mov      %es, %ax
    popq     %rax
    mov      %ds, %ax
    popq     %rax
    popq     %rbx
    popq     %rcx
    popq     %rdx
    popq     %rsi
    popq     %rdi
    popq     %r8
    popq     %r9
    popq     %r10
    popq     %r11
    popq     %r12
    popq     %r13
    popq     %r14
    popq     %r15
    popq     %rbp
    popq     %fs
    popq     %gs

    retq

Entry(x86_64_context_switch_first)
    movq 8(%rdi), %rsp

    swapgs

    popq     %rax
    mov      %es, %ax
    popq     %rax
    mov      %ds, %ax
    popq     %rax
    popq     %rbx
    popq     %rcx
    popq     %rdx
    popq     %rsi
    popq     %rdi
    popq     %r8
    popq     %r9
    popq     %r10
    popq     %r11
    popq     %r12
    popq     %r13
    popq     %r14
    popq     %r15
    popq     %rbp
    popq     %fs
    popq     %gs

    retq

Entry(x86_64_context_save_state)
    pushq    %gs
    pushq    %fs
    pushq    %rbp
    pushq    %r15
    pushq    %r14
    pushq    %r13
    pushq    %r12
    pushq    %r11
    pushq    %r10
    pushq    %r9
    pushq    %r8
    pushq    %rdi
    pushq    %rsi
    pushq    %rdx
    pushq    %rcx
    pushq    %rbx
    pushq    %rax
    xor      %rax, %rax
    mov      %ax, %ds
    pushq    %rax
    mov      %ax, %es
    pushq    %rax

    movq  %rsp, 8(%rdi)

    retq

    /* halt_cpu */

Entry(halt_cpu)
    sti
    hlt

    ret

    /* x86_init_wrapper */

Entry(x86_init_wrapper)
	xor	%rbp, %rbp
	movq	%rsi, %rsp
	callq	*%rdi
/*
 *  uint64_t tmrCvt(uint64_t time,          // %rdi
 *                  uint64_t conversion)	// %rsi
 *
 */
ENTRY(tmrCvt)
	cmpq	$1,%rsi				/* check for unity fastpath */
	je	1f
	movq	%rdi,%rax
	mulq	%rsi				/* result is %rdx:%rax */
	shrdq   $32,%rdx,%rax		/* %rdx:%rax >>= 32 */
	ret
1:
	mov	%rdi,%rax
	ret

ENTRY(_rtc_nanotime_read)
    0:
    movl        PAL_RTC_GENERATION(%rdi),%esi
	test        %esi,%esi       /* info updating? */
    jz          0b              /* - wait if so */
	lfence
	rdtsc
	lfence
	shlq    $32,%rdx
	movl    PAL_RTC_SHIFT(%rdi),%ecx
	orq     %rdx,%rax                       /* %rax := tsc */
	subq    PAL_RTC_TSC_BASE(%rdi),%rax     /* tsc - tsc_base */
	shlq    %cl,%rax
	movl    PAL_RTC_SCALE(%rdi),%ecx
	mulq    %rcx                            /* delta * scale */
	shrdq   $32,%rdx,%rax                   /* %rdx:%rax >>= 32 */
	addq    PAL_RTC_NS_BASE(%rdi),%rax      /* add ns_base */
	cmpl    PAL_RTC_GENERATION(%rdi),%esi   /* repeat if changed */
	jne     0b
    ret
/*
 * void pal_rtc_nanotime_store(
 *		uint64_t        tsc,		// %rdi
 *		uint64_t        nsec,		// %rsi
 *		uint32_t        scale,		// %rdx
 *		uint32_t        shift,		// %rcx
 *		rtc_nanotime_t  *dst);		// %r8
 */
ENTRY(pal_rtc_nanotime_store)
	movl	PAL_RTC_GENERATION(%r8),%eax	/* get current generation */
	movl	$0,PAL_RTC_GENERATION(%r8)		/* flag data as being updated */
	movq	%rdi,PAL_RTC_TSC_BASE(%r8)
	movq	%rsi,PAL_RTC_NS_BASE(%r8)
	movl	%edx,PAL_RTC_SCALE(%r8)
	movl	%ecx,PAL_RTC_SHIFT(%r8)

	incl	%eax				/* next generation */
	jnz	1f
	incl	%eax				/* skip 0, which is a flag */
1:	movl	%eax,PAL_RTC_GENERATION(%r8)	/* update generation */

	ret

    /* x86_triplefault */

Entry(x86_triplefault)
    //lidt    idt_zero
    //int $3  /* Just to interrupt, CPU won't like it */

Entry(do_mfence)
    mfence
    ret

#ifdef __ELF__
.data
#else
.section __DATA,__data
#endif
idt_zero:
.long   0, 0
#ifdef __ELF__
.text
#else
.section __TEXT,__text
#endif
