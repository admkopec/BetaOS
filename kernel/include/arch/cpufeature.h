//
//  cpufeature.h
//  BetaOS
//
//  Created by Adam Kopeć on 3/18/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef cpufeature_h
#define cpufeature_h

#define _CPUF_I386_FPU          0           /* FPU-x87 FPU on Chip */
#define _CPUF_I386_PSE          1           /* Page Size Extension */
#define _CPUF_I386_PGE          2           /* Page Global Enable */
#define _CPUF_I386_APIC_ON_CHIP	3           /* APIC is present on the chip */
#define _CPUF_I386_TSC          4           /* Timestamp counter present */
#define _CPUF_I386_SSE1234_12	5           /* Support for SSE/SSE2/SSE3/SSSE3/SSE4 Extensions and FXSR */
#define _CPUF_I386_FXSR         6
#define _CPUF_I386_SSE          7
#define _CPUF_I386_SSE2         8
#define _CPUF_I386_SSE3         9
#define _CPUF_I386_SSSE3        10
#define _CPUF_I386_SSE4_1       11
#define _CPUF_I386_SSE4_2       12

#define _CPUF_I386_HTT          13          /* Supports HTT */
#define _CPUF_I386_HTT_MAX_NUM	14          /* Maximal num of threads */

#define _CPUF_I386_MTRR         15
#define _CPUF_I386_SYSENTER     16          /* Intel SYSENTER instrs */
#define _CPUF_I386_SYSCALL      17          /* AMD SYSCALL instrs */

/* CPUID flags */
#define CPUID1_EDX_FPU		(1L)            /* FPU presence */
#define CPUID1_EDX_PSE 		(1L <<  3)      /* Page Size Extension */
#define CPUID1_EDX_SYSENTER	(1L << 11)      /* Intel SYSENTER */
#define CPUID1_EDX_PGE 		(1L << 13)      /* Page Global (bit) Enable */
#define CPUID1_EDX_APIC_ON_CHIP (1L << 9)	/* APIC is present on the chip */
#define CPUID1_EDX_TSC		(1L << 4)       /* Timestamp counter present */
#define CPUID1_EDX_HTT		(1L << 28)      /* Supports HTT */
#define CPUID1_EDX_FXSR		(1L << 24)
#define CPUID1_EDX_SSE		(1L << 25)
#define CPUID1_EDX_SSE2		(1L << 26)
#define CPUID1_ECX_SSE3		(1L)
#define CPUID1_ECX_SSSE3	(1L << 9)
#define CPUID1_ECX_SSE4_1	(1L << 19)
#define CPUID1_ECX_SSE4_2	(1L << 20)

#define CPUID_EF_EDX_SYSENTER	(1L << 11)	/* Intel SYSENTER */

int _cpufeature(int featurenum);

#endif /* cpufeature_h */
