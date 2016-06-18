//
//  cpuid.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/18/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef cpuid_h
#define cpuid_h

#define	CPUID_VID_INTEL         "GenuineIntel"
#define	CPUID_VID_AMD           "AuthenticAMD"

#define CPUID_VMM_ID_VMWARE		"VMwareVMware"
#define CPUID_VMM_ID_PARALLELS	"Parallels\0\0\0"

#define CPUID_STRING_UNKNOWN    "Unknown CPU Typ"

#define _Bit(n)			(1ULL << n)
#define _HBit(n)		(1ULL << ((n)+32))

/*
 * The CPUID_FEATURE_XXX values define 64-bit values
 * returned in %ecx:%edx to a CPUID request with %eax of 1:
 */
#define CPUID_FEATURE_FPU       _Bit(0)   /* Floating point unit on-chip */
#define CPUID_FEATURE_VME       _Bit(1)   /* Virtual Mode Extension */
#define CPUID_FEATURE_DE        _Bit(2)   /* Debugging Extension */
#define CPUID_FEATURE_PSE       _Bit(3)   /* Page Size Extension */
#define CPUID_FEATURE_TSC       _Bit(4)   /* Time Stamp Counter */
#define CPUID_FEATURE_MSR       _Bit(5)   /* Model Specific Registers */
#define CPUID_FEATURE_PAE       _Bit(6)   /* Physical Address Extension */
#define CPUID_FEATURE_MCE       _Bit(7)   /* Machine Check Exception */
#define CPUID_FEATURE_CX8       _Bit(8)   /* CMPXCHG8B */
#define CPUID_FEATURE_APIC      _Bit(9)   /* On-chip APIC */
#define CPUID_FEATURE_SEP       _Bit(11)  /* Fast System Call */
#define CPUID_FEATURE_MTRR      _Bit(12)  /* Memory Type Range Register */
#define CPUID_FEATURE_PGE       _Bit(13)  /* Page Global Enable */
#define CPUID_FEATURE_MCA       _Bit(14)  /* Machine Check Architecture */
#define CPUID_FEATURE_CMOV      _Bit(15)  /* Conditional Move Instruction */
#define CPUID_FEATURE_PAT       _Bit(16)  /* Page Attribute Table */
#define CPUID_FEATURE_PSE36     _Bit(17)  /* 36-bit Page Size Extension */
#define CPUID_FEATURE_PSN       _Bit(18)  /* Processor Serial Number */
#define CPUID_FEATURE_CLFSH     _Bit(19)  /* CLFLUSH Instruction supported */
#define CPUID_FEATURE_DS        _Bit(21)  /* Debug Store */
#define CPUID_FEATURE_ACPI      _Bit(22)  /* Thermal monitor and Clock Ctrl */
#define CPUID_FEATURE_MMX       _Bit(23)  /* MMX supported */
#define CPUID_FEATURE_FXSR      _Bit(24)  /* Fast floating pt save/restore */
#define CPUID_FEATURE_SSE       _Bit(25)  /* Streaming SIMD extensions */
#define CPUID_FEATURE_SSE2      _Bit(26)  /* Streaming SIMD extensions 2 */
#define CPUID_FEATURE_SS        _Bit(27)  /* Self-Snoop */
#define CPUID_FEATURE_HTT       _Bit(28)  /* Hyper-Threading Technology */
#define CPUID_FEATURE_TM        _Bit(29)  /* Thermal Monitor (TM1) */
#define CPUID_FEATURE_PBE       _Bit(31)  /* Pend Break Enable */

#define CPUID_FEATURE_SSE3      _HBit(0)  /* Streaming SIMD extensions 3 */
#define CPUID_FEATURE_PCLMULQDQ _HBit(1)  /* PCLMULQDQ instruction */
#define CPUID_FEATURE_DTES64    _HBit(2)  /* 64-bit DS layout */
#define CPUID_FEATURE_MONITOR   _HBit(3)  /* Monitor/mwait */
#define CPUID_FEATURE_DSCPL     _HBit(4)  /* Debug Store CPL */
#define CPUID_FEATURE_VMX       _HBit(5)  /* VMX */
#define CPUID_FEATURE_SMX       _HBit(6)  /* SMX */
#define CPUID_FEATURE_EST       _HBit(7)  /* Enhanced SpeedsTep (GV3) */
#define CPUID_FEATURE_TM2       _HBit(8)  /* Thermal Monitor 2 */
#define CPUID_FEATURE_SSSE3     _HBit(9)  /* Supplemental SSE3 instructions */
#define CPUID_FEATURE_CID       _HBit(10) /* L1 Context ID */
#define CPUID_FEATURE_SEGLIM64  _HBit(11) /* 64-bit segment limit checking */
#define CPUID_FEATURE_FMA       _HBit(12) /* Fused-Multiply-Add support */
#define CPUID_FEATURE_CX16      _HBit(13) /* CmpXchg16b instruction */
#define CPUID_FEATURE_xTPR      _HBit(14) /* Send Task PRiority msgs */
#define CPUID_FEATURE_PDCM      _HBit(15) /* Perf/Debug Capability MSR */

#define CPUID_FEATURE_PCID      _HBit(17) /* ASID-PCID support */
#define CPUID_FEATURE_DCA       _HBit(18) /* Direct Cache Access */
#define CPUID_FEATURE_SSE4_1    _HBit(19) /* Streaming SIMD extensions 4.1 */
#define CPUID_FEATURE_SSE4_2    _HBit(20) /* Streaming SIMD extensions 4.2 */
#define CPUID_FEATURE_x2APIC    _HBit(21) /* Extended APIC Mode */
#define CPUID_FEATURE_MOVBE     _HBit(22) /* MOVBE instruction */
#define CPUID_FEATURE_POPCNT    _HBit(23) /* POPCNT instruction */
#define CPUID_FEATURE_TSCTMR    _HBit(24) /* TSC deadline timer */
#define CPUID_FEATURE_AES       _HBit(25) /* AES instructions */
#define CPUID_FEATURE_XSAVE     _HBit(26) /* XSAVE instructions */
#define CPUID_FEATURE_OSXSAVE   _HBit(27) /* XGETBV/XSETBV instructions */
#define CPUID_FEATURE_AVX1_0	_HBit(28) /* AVX 1.0 instructions */
#define CPUID_FEATURE_F16C      _HBit(29) /* Float16 convert instructions */
#define CPUID_FEATURE_RDRAND	_HBit(30) /* RDRAND instruction */
#define CPUID_FEATURE_VMM       _HBit(31) /* VMM (Hypervisor) present */

/*
 * Leaf 7, subleaf 0 additional features.
 * Bits returned in %ebx:%ecx to a CPUID request with {%eax,%ecx} of (0x7,0x0}:
 */
#define CPUID_LEAF7_FEATURE_RDWRFSGS _Bit(0)	/* FS/GS base read/write */
#define CPUID_LEAF7_FEATURE_TSCOFF   _Bit(1)	/* TSC thread offset */
#define CPUID_LEAF7_FEATURE_BMI1     _Bit(3)	/* Bit Manipulation Instrs, set 1 */
#define CPUID_LEAF7_FEATURE_HLE      _Bit(4)	/* Hardware Lock Elision*/
#define CPUID_LEAF7_FEATURE_AVX2     _Bit(5)	/* AVX2 Instructions */
#define CPUID_LEAF7_FEATURE_SMEP     _Bit(7)	/* Supervisor Mode Execute Protect */
#define CPUID_LEAF7_FEATURE_BMI2     _Bit(8)	/* Bit Manipulation Instrs, set 2 */
#define CPUID_LEAF7_FEATURE_ERMS     _Bit(9)	/* Enhanced Rep Movsb/Stosb */
#define CPUID_LEAF7_FEATURE_INVPCID  _Bit(10)	/* INVPCID intruction, TDB */
#define CPUID_LEAF7_FEATURE_RTM      _Bit(11)	/* RTM */
#define CPUID_LEAF7_FEATURE_RDSEED   _Bit(18)	/* RDSEED Instruction */
#define CPUID_LEAF7_FEATURE_ADX      _Bit(19)	/* ADX Instructions */
#define CPUID_LEAF7_FEATURE_SMAP     _Bit(20)	/* Supervisor Mode Access Protect */
#define CPUID_LEAF7_FEATURE_SGX      _Bit(2)	/* Software Guard eXtensions */
#define CPUID_LEAF7_FEATURE_PQM      _Bit(12)	/* Platform Qos Monitoring */
#define CPUID_LEAF7_FEATURE_FPU_CSDS _Bit(13)	/* FPU CS/DS deprecation */
#define CPUID_LEAF7_FEATURE_MPX      _Bit(14)	/* Memory Protection eXtensions */
#define CPUID_LEAF7_FEATURE_PQE      _Bit(15)	/* Platform Qos Enforcement */
#define CPUID_LEAF7_FEATURE_CLFSOPT  _Bit(23)	/* CLFSOPT */
#define CPUID_LEAF7_FEATURE_IPT      _Bit(25)	/* Intel Processor Trace */
#define CPUID_LEAF7_FEATURE_SHA      _Bit(29)	/* SHA instructions */

#define CPUID_LEAF7_FEATURE_PREFETCHWT1 _HBit(0)/* Prefetch Write/T1 hint */

/*
 * The CPUID_EXTFEATURE_XXX values define 64-bit values
 * returned in %ecx:%edx to a CPUID request with %eax of 0x80000001:
 */
#define CPUID_EXTFEATURE_SYSCALL   _Bit(11)	/* SYSCALL/sysret */
#define CPUID_EXTFEATURE_XD	       _Bit(20)	/* eXecute Disable */

#define CPUID_EXTFEATURE_1GBPAGE   _Bit(26)	/* 1GB pages */
#define CPUID_EXTFEATURE_RDTSCP	   _Bit(27)	/* RDTSCP */
#define CPUID_EXTFEATURE_EM64T	   _Bit(29)	/* Extended Mem 64 Technology */

#define CPUID_EXTFEATURE_LAHF	   _HBit(0)	/* LAFH/SAHF instructions */
#define CPUID_EXTFEATURE_LZCNT     _HBit(5)	/* LZCNT instruction */
#define CPUID_EXTFEATURE_PREFETCHW _HBit(8)	/* PREFETCHW instruction */

/*
 * The CPUID_EXTFEATURE_XXX values define 64-bit values
 * returned in %ecx:%edx to a CPUID request with %eax of 0x80000007:
 */
#define CPUID_EXTFEATURE_TSCI      _Bit(8)	/* TSC Invariant */

/*
 * CPUID_X86_64_H_FEATURE_SUBSET and CPUID_X86_64_H_LEAF7_FEATURE_SUBSET
 * indicate the bitmask of features that must be present before the system
 * is eligible to run the "x86_64h" "Haswell feature subset" slice.
 */
#define CPUID_X86_64_H_FEATURE_SUBSET ( CPUID_FEATURE_FMA    | \
                                        CPUID_FEATURE_SSE4_2 | \
                                        CPUID_FEATURE_MOVBE  | \
                                        CPUID_FEATURE_POPCNT | \
                                        CPUID_FEATURE_AVX1_0   \
                                      )

#define CPUID_X86_64_H_EXTFEATURE_SUBSET ( CPUID_EXTFEATURE_LZCNT )

#define CPUID_X86_64_H_LEAF7_FEATURE_SUBSET ( CPUID_LEAF7_FEATURE_BMI1 | \
                                              CPUID_LEAF7_FEATURE_AVX2 | \
                                              CPUID_LEAF7_FEATURE_BMI2   \
                                            )

#define	CPUID_CACHE_SIZE            16	/* Number of descriptor values */

#define CPUID_MWAIT_EXTENSION	_Bit(0)	/* enumeration of WMAIT extensions */
#define CPUID_MWAIT_BREAK       _Bit(1)	/* interrupts are break events	   */

#define CPUID_MODEL_YONAH           0x0E
#define CPUID_MODEL_MEROM           0x0F
#define CPUID_MODEL_PENRYN          0x17
#define CPUID_MODEL_NEHALEM         0x1A
#define CPUID_MODEL_FIELDS          0x1E	/* Lynnfield, Clarksfield */
#define CPUID_MODEL_DALES           0x1F	/* Havendale, Auburndale */
#define CPUID_MODEL_NEHALEM_EX		0x2E
#define CPUID_MODEL_DALES_32NM		0x25	/* Clarkdale, Arrandale */
#define CPUID_MODEL_WESTMERE		0x2C	/* Gulftown, Westmere-EP/-WS */
#define CPUID_MODEL_WESTMERE_EX		0x2F
#define CPUID_MODEL_SANDYBRIDGE		0x2A
#define CPUID_MODEL_JAKETOWN		0x2D
#define CPUID_MODEL_IVYBRIDGE		0x3A
#define CPUID_MODEL_IVYBRIDGE_EP	0x3E
#define CPUID_MODEL_CRYSTALWELL		0x46
#define CPUID_MODEL_HASWELL         0x3C
#define CPUID_MODEL_HASWELL_EP		0x3F
#define CPUID_MODEL_HASWELL_ULT		0x45
#define CPUID_MODEL_BROADWELL		0x3D
#define CPUID_MODEL_BROADWELL_ULX	0x3D
#define CPUID_MODEL_BROADWELL_ULT	0x3D
#define CPUID_MODEL_BRYSTALWELL		0x47
#define CPUID_MODEL_SKYLAKE         0x4E
#define CPUID_MODEL_SKYLAKE_ULT		0x4E
#define CPUID_MODEL_SKYLAKE_ULX		0x4E
#define CPUID_MODEL_SKYLAKE_DT		0x5E

#define CPUID_VMM_FAMILY_UNKNOWN	0x0
#define CPUID_VMM_FAMILY_VMWARE		0x1
#define CPUID_VMM_FAMILY_PARALLELS	0x2

#ifndef __ASSEMBLY__
#include <stdint.h>

typedef enum { eax, ebx, ecx, edx } cpuid_register_t;
void cpuid(uint32_t *data);

void do_cpuid(uint32_t selector, uint32_t *data);

#endif /* __ASSEMBLY__ */

#endif /* cpuid_h */
