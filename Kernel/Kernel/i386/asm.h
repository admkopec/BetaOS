//
//  asm.h
//  BetaOS
//
//  Created by Adam Kopeć on 3/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef asm_h
#define asm_h

#ifdef __NO_UNDERSCORES__
#undef __NO_UNDERSCORES__ /* Just for temporary testing purposes */
#endif

#ifdef __ELF__
#undef __ELF__
#endif

#ifndef __STDC__
#define __STDC__
#endif

#ifdef __ASSEMBLY__
#define ALIGN 4,0x90
#endif /* __ASSEMBLY__ */

#ifndef FALIGN
#define FALIGN ALIGN
#endif

#define LB(x,n) n
#if	__STDC__
#ifndef __NO_UNDERSCORES__
#define	LCL(x)	L ## x
#define EXT(x) _ ## x
#define LEXT(x) _ ## x ## :
#else
#define	LCL(x)	.L ## x
#define EXT(x) x
#define LEXT(x) x ## :
#endif
#define LBc(x,n) n ## :
#define LBb(x,n) n ## b
#define LBf(x,n) n ## f
#else /* __STDC__ */
#ifndef __NO_UNDERSCORES__
#define LCL(x) L/**/x
#define EXT(x) _/**/x
#define LEXT(x) _/**/x/**/:
#else /* __NO_UNDERSCORES__ */
#define	LCL(x)	.L/**/x
#define EXT(x) x
#define LEXT(x) x/**/:
#endif /* __NO_UNDERSCORES__ */
#define LBc(x,n) n/**/:
#define LBb(x,n) n/**/b
#define LBf(x,n) n/**/f
#endif /* __STDC__ */

#if !GPROF
#define MCOUNT

#elif defined(__SHARED__)
#define MCOUNT		; .data;\
                    .align ALIGN;\
                    LBc(x, 8) .long 0;\
                    .text;\
                    Gpush;\
                    Gload;\
                    leal Gotoff(LBb(x,8)),%edx;\
                    Egaddr(%eax,_mcount_ptr);\
                    Gpop;\
                    call *(%eax);

#else	/* !GPROF, !__SHARED__ */
#define MCOUNT		; call mcount;
#endif /* GPROF */

#ifdef __ELF__
#define ELF_FUNC(x)	  .type x,@function
#define ELF_DATA(x)	  .type x,@object
#define ELF_SIZE(x,s)	.size x,s
#else /* __ELF__ */
#define ELF_FUNC(x)
#define ELF_DATA(x)
#define ELF_SIZE(x,s)
#endif

#define Entry(x)    .globl EXT(x); ELF_FUNC(EXT(x)); .align FALIGN; LEXT(x)
#define	ENTRY(x)	Entry(x) MCOUNT
#define	ENTRY2(x,y)	.globl EXT(x); .globl EXT(y);           \
                    ELF_FUNC(EXT(x)); ELF_FUNC(EXT(y));     \
                    .align FALIGN; LEXT(x); LEXT(y)         \
                    MCOUNT


/* This variant exists to permit adjustment of the stack by "dtrace" */
#define CCALLWITHSP(fn) \
                        mov	%rsp, %r12  ;\
                        sub	$8, %rsp    ;\
                        and	$0xFFFFFFFFFFFFFFF0, %rsp   ;\
                        mov	%r12, (%rsp)    ;\
                        leaq    (%rsp), %rsi    ;\
                        call    EXT(fn)				;\
                        mov	(%rsp), %rsp

#define CCALL(fn) \
                  mov  %rsp, %r12  ;\
                  and  $0xFFFFFFFFFFFFFFF0, %rsp   ;\
                  call EXT(fn) ;\
                  mov  %r12, %rsp

#define CCALL1(fn, arg1) \
                        mov	arg1, %rdi  ;\
                        CCALL(fn)

#define CCALL2(fn, arg1, arg2) \
                                mov	arg1, %rdi  ;\
                                mov	arg2, %rsi  ;\
                                CCALL(fn)

#define CCALL3(fn, arg1, arg2, arg3) \
                                    mov	arg1, %rdi  ;\
                                    mov	arg2, %rsi  ;\
                                    mov	arg3, %rdx  ;\
                                    CCALL(fn)

#ifndef __ASSEMBLY__

void halt_cpu(void);
void x86_triplefault(void);
void x86_init_wrapper(unsigned long, unsigned long) __attribute__((noreturn));
void bzero(void * addr, size_t length);
void bcopy(const void * from, void * to, unsigned int count);

#endif /* __ASSEMBLY__ */
#endif /* asm_h */
