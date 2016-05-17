//
//  asm.h
//  BetaOS
//
//  Created by Adam Kopeć on 3/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef asm_h
#define asm_h

//#ifdef __NO_UNDERSCORES__
//#undef __NO_UNDERSCORES__ /* Just for temporary testing purposes */
//#endif

//#ifndef __STDC__
//#define __STDC__
//#endif

#define __ELF__
#define __NO_UNDERSCORES__

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

#ifndef __ASSEMBLY__

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

#endif /* __ASSEMBLY__ */

#endif /* asm_h */
