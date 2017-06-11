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

#define CPUFAMILY_UNKNOWN   		0
#define CPUFAMILY_INTEL_YONAH		0x73d67300
#define CPUFAMILY_INTEL_MEROM		0x426f69ef
#define CPUFAMILY_INTEL_PENRYN		0x78ea4fbc
#define CPUFAMILY_INTEL_NEHALEM		0x6b5a4cd2
#define CPUFAMILY_INTEL_WESTMERE	0x573b5eec
#define CPUFAMILY_INTEL_SANDYBRIDGE	0x5490b78c
#define CPUFAMILY_INTEL_IVYBRIDGE	0x1f65e835
#define CPUFAMILY_INTEL_HASWELL		0x10b282dc
#define CPUFAMILY_INTEL_BROADWELL	0x582ed09c
#define CPUFAMILY_INTEL_SKYLAKE		0x37fc219f

#ifndef __ASSEMBLY__
#include <stdint.h>
#include <stdbool.h>

/*
 *	I386 subtypes
 */

#define CPU_SUBTYPE_INTEL(f, m)	((cpu_subtype_t) (f) + ((m) << 4))

#define	CPU_SUBTYPE_I386_ALL			CPU_SUBTYPE_INTEL(3, 0)
#define CPU_SUBTYPE_386					CPU_SUBTYPE_INTEL(3, 0)
#define CPU_SUBTYPE_486					CPU_SUBTYPE_INTEL(4, 0)
#define CPU_SUBTYPE_486SX				CPU_SUBTYPE_INTEL(4, 8)	// 8 << 4 = 128
#define CPU_SUBTYPE_586					CPU_SUBTYPE_INTEL(5, 0)
#define CPU_SUBTYPE_PENT                CPU_SUBTYPE_INTEL(5, 0)
#define CPU_SUBTYPE_PENTPRO             CPU_SUBTYPE_INTEL(6, 1)
#define CPU_SUBTYPE_PENTII_M3           CPU_SUBTYPE_INTEL(6, 3)
#define CPU_SUBTYPE_PENTII_M5           CPU_SUBTYPE_INTEL(6, 5)
#define CPU_SUBTYPE_CELERON				CPU_SUBTYPE_INTEL(7, 6)
#define CPU_SUBTYPE_CELERON_MOBILE		CPU_SUBTYPE_INTEL(7, 7)
#define CPU_SUBTYPE_PENTIUM_3			CPU_SUBTYPE_INTEL(8, 0)
#define CPU_SUBTYPE_PENTIUM_3_M			CPU_SUBTYPE_INTEL(8, 1)
#define CPU_SUBTYPE_PENTIUM_3_XEON		CPU_SUBTYPE_INTEL(8, 2)
#define CPU_SUBTYPE_PENTIUM_M			CPU_SUBTYPE_INTEL(9, 0)
#define CPU_SUBTYPE_PENTIUM_4			CPU_SUBTYPE_INTEL(10, 0)
#define CPU_SUBTYPE_PENTIUM_4_M			CPU_SUBTYPE_INTEL(10, 1)
#define CPU_SUBTYPE_ITANIUM				CPU_SUBTYPE_INTEL(11, 0)
#define CPU_SUBTYPE_ITANIUM_2			CPU_SUBTYPE_INTEL(11, 1)
#define CPU_SUBTYPE_XEON				CPU_SUBTYPE_INTEL(12, 0)
#define CPU_SUBTYPE_XEON_MP				CPU_SUBTYPE_INTEL(12, 1)

#define CPU_SUBTYPE_INTEL_FAMILY(x)     ((x) & 15)
#define CPU_SUBTYPE_INTEL_FAMILY_MAX	15

#define CPU_SUBTYPE_INTEL_MODEL(x)      ((x) >> 4)
#define CPU_SUBTYPE_INTEL_MODEL_ALL     0

/*
 *	X86 subtypes.
 */

#define CPU_SUBTYPE_X86_ALL         ((cpu_subtype_t)3)
#define CPU_SUBTYPE_X86_64_ALL		((cpu_subtype_t)3)
#define CPU_SUBTYPE_X86_ARCH1		((cpu_subtype_t)4)
#define CPU_SUBTYPE_X86_64_H		((cpu_subtype_t)8)	/* Haswell feature subset */

#define CPU_ARCH_ABI64	0x01000000		/* 64 bit ABI */
#define CPU_TYPE_X86	((cpu_type_t) 7)
#define	CPU_TYPE_X86_64	(CPU_TYPE_X86 | CPU_ARCH_ABI64)
#ifndef CPU_TYPE_DEFINED
typedef int cpu_type_t; typedef int cpu_subtype_t; // Temporary
#define CPU_TYPE_DEFINED
#endif
enum { eax, ebx, ecx, edx };
typedef enum { Lnone, L1I, L1D, L2U, L3U, LCACHE_MAX } cache_type_t ;

__attribute__((unused)) typedef struct {
    unsigned char	value;          /* Descriptor value */
    cache_type_t 	type;           /* Cache type */
    unsigned int 	size;           /* Cache size */
    unsigned int 	linesize;       /* Cache line size */
    const char*     description;    /* Cache description */
} cpuid_cache_desc_t;

/* Monitor/mwait Leaf: */
typedef struct {
    uint32_t	linesize_min;
    uint32_t	linesize_max;
    uint32_t	extensions;
    uint32_t	sub_Cstates;
} cpuid_mwait_leaf_t;

/* Thermal and Power Management Leaf: */
typedef struct {
    bool        sensor;
    bool        dynamic_acceleration;
    bool        invariant_APIC_timer;
    bool        core_power_limits;
    bool        fine_grain_clock_mod;
    bool        package_thermal_intr;
    uint32_t	thresholds;
    bool        ACNT_MCNT;
    bool        hardware_feedback;
    bool        energy_policy;
} cpuid_thermal_leaf_t;


/* XSAVE Feature Leaf: */
typedef struct {
    uint32_t	extended_state[4];	/* eax .. edx */
} cpuid_xsave_leaf_t;

/* Architectural Performance Monitoring Leaf: */
typedef struct {
    uint8_t		version;
    uint8_t		number;
    uint8_t		width;
    uint8_t		events_number;
    uint32_t	events;
    uint8_t		fixed_number;
    uint8_t		fixed_width;
} cpuid_arch_perf_leaf_t;

/* The TSC to Core Crystal (RefCLK) Clock Information leaf */
typedef struct {
    uint32_t	numerator;
    uint32_t	denominator;
} cpuid_tsc_leaf_t;

typedef struct {
    char		cpuid_vendor[16];
    char		cpuid_brand_string[48];
    
    cpu_type_t	cpuid_type;
    uint8_t		cpuid_family;
    uint8_t		cpuid_model;
    uint8_t		cpuid_extmodel;
    uint8_t		cpuid_extfamily;
    uint8_t		cpuid_stepping;
    uint64_t	cpuid_features;
    uint64_t	cpuid_extfeatures;
    uint32_t	cpuid_signature;
    uint8_t   	cpuid_brand;
    uint8_t		cpuid_processor_flag;
    
    uint32_t	cache_size[LCACHE_MAX];
    uint32_t	cache_linesize;
    
    uint8_t		cache_info[64];    /* list of cache descriptors */
    
    uint32_t	cpuid_cores_per_package;
    uint32_t	cpuid_logical_per_package;
    uint32_t	cache_sharing[LCACHE_MAX];
    uint32_t	cache_partitions[LCACHE_MAX];
    
    cpu_type_t	  cpuid_cpu_type;
    cpu_subtype_t cpuid_cpu_subtype;
    
    /* Per-vendor info */
    cpuid_mwait_leaf_t	cpuid_mwait_leaf;
#define cpuid_mwait_linesize_max	cpuid_mwait_leaf.linesize_max
#define cpuid_mwait_linesize_min	cpuid_mwait_leaf.linesize_min
#define cpuid_mwait_extensions		cpuid_mwait_leaf.extensions
#define cpuid_mwait_sub_Cstates		cpuid_mwait_leaf.sub_Cstates
    cpuid_thermal_leaf_t	cpuid_thermal_leaf;
    cpuid_arch_perf_leaf_t	cpuid_arch_perf_leaf;
    uint32_t unused[4];	 /* cpuid_xsave_leaf */
    
    /* Cache details: */
    uint32_t	cpuid_cache_linesize;
    uint32_t	cpuid_cache_L2_associativity;
    uint32_t	cpuid_cache_size;
    
    /* Virtual and physical address aize: */
    uint32_t	cpuid_address_bits_physical;
    uint32_t	cpuid_address_bits_virtual;
    
    uint32_t	cpuid_microcode_version;
    
    /* Numbers of tlbs per processor [i|d, small|large, level0|level1] */
    uint32_t	cpuid_tlb[2][2][2];
#define	TLB_INST	0
#define	TLB_DATA	1
#define	TLB_SMALL	0
#define	TLB_LARGE	1
    uint32_t	cpuid_stlb;
    
    uint32_t	core_count;
    uint32_t	thread_count;
    
    /* Max leaf ids available from CPUID */
    uint32_t	cpuid_max_basic;
    uint32_t	cpuid_max_ext;
    
    /* Family-specific info links */
    uint32_t                 cpuid_cpufamily;
    cpuid_mwait_leaf_t      *cpuid_mwait_leafp;
    cpuid_thermal_leaf_t    *cpuid_thermal_leafp;
    cpuid_arch_perf_leaf_t	*cpuid_arch_perf_leafp;
    cpuid_xsave_leaf_t      *cpuid_xsave_leafp;
    uint64_t                 cpuid_leaf7_features;
    cpuid_tsc_leaf_t         cpuid_tsc_leaf;
    cpuid_xsave_leaf_t       cpuid_xsave_leaf[2];
} i386_cpu_info_t;

typedef struct {
    char		cpuid_vmm_vendor[16];
    uint32_t	cpuid_vmm_family;
    uint32_t	cpuid_vmm_bus_frequency;
    uint32_t	cpuid_vmm_tsc_frequency;
} i386_vmm_info_t;

extern void cpuid(uint32_t *data);
extern void do_cpuid(uint32_t selector, uint32_t *data);
extern void cpuid_fn(uint32_t selector, uint32_t *result);
extern void cpuid_init_vmm_info(i386_vmm_info_t *info_p);
extern void cpuid_set_info(void);
extern i386_vmm_info_t *cpuid_vmm_info(void);
extern i386_cpu_info_t *cpuid_info(void);
extern bool cpuid_vmm_present(void);
extern uint64_t cpuid_features(void);
extern uint64_t cpuid_leaf7_features(void);

#endif /* __ASSEMBLY__ */

#endif /* cpuid_h */
