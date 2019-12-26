//
//  proc_reg.h
//  BetaOS
//
//  Created by Adam Kopeć on 4/30/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

/*
 * Processor registers for i386 and i486.
 */
#ifndef i386_proc_reg_h
#define i386_proc_reg_h
/*
 * Model Specific Registers
 */
#define	MSR_P5_TSC		0x10	/* Time Stamp Register */
#define	MSR_P5_CESR		0x11	/* Control and Event Select Register */
#define	MSR_P5_CTR0		0x12	/* Counter #0 */
#define	MSR_P5_CTR1		0x13	/* Counter #1 */

#define	MSR_P5_CESR_PC		0x0200	/* Pin Control */
#define	MSR_P5_CESR_CC		0x01C0	/* Counter Control mask */
#define	MSR_P5_CESR_ES		0x003F	/* Event Control mask */

#define	MSR_P5_CESR_SHIFT	16		/* Shift to get Counter 1 */
#define	MSR_P5_CESR_MASK          (MSR_P5_CESR_PC|\
				                   MSR_P5_CESR_CC|\
				                   MSR_P5_CESR_ES) /* Mask Counter */

#define	MSR_P5_CESR_CC_CLOCK	0x0100	/* Clock Counting (otherwise Event) */
#define	MSR_P5_CESR_CC_DISABLE	0x0000	/* Disable counter */
#define	MSR_P5_CESR_CC_CPL012	0x0040	/* Count if the CPL == 0, 1, 2 */
#define	MSR_P5_CESR_CC_CPL3     0x0080	/* Count if the CPL == 3 */
#define	MSR_P5_CESR_CC_CPL      0x00C0	/* Count regardless of the CPL */

#define	MSR_P5_CESR_ES_DATA_READ       0x000000	/* Data Read */
#define	MSR_P5_CESR_ES_DATA_WRITE      0x000001	/* Data Write */
#define	MSR_P5_CESR_ES_DATA_RW	       0x101000	/* Data Read or Write */
#define	MSR_P5_CESR_ES_DATA_TLB_MISS   0x000010	/* Data TLB Miss */
#define	MSR_P5_CESR_ES_DATA_READ_MISS  0x000011	/* Data Read Miss */
#define	MSR_P5_CESR_ES_DATA_WRITE_MISS 0x000100	/* Data Write Miss */
#define	MSR_P5_CESR_ES_DATA_RW_MISS    0x101001	/* Data Read or Write Miss */
#define	MSR_P5_CESR_ES_HIT_EM	       0x000101	/* Write (hit) to M|E state */
#define	MSR_P5_CESR_ES_DATA_CACHE_WB   0x000110	/* Cache lines written back */
#define	MSR_P5_CESR_ES_EXTERNAL_SNOOP  0x000111	/* External Snoop */
#define	MSR_P5_CESR_ES_CACHE_SNOOP_HIT 0x001000	/* Data cache snoop hits */
#define	MSR_P5_CESR_ES_MEM_ACCESS_PIPE 0x001001	/* Mem. access in both pipes */
#define	MSR_P5_CESR_ES_BANK_CONFLICTS  0x001010	/* Bank conflicts */
#define	MSR_P5_CESR_ES_MISALIGNED      0x001011	/* Misaligned Memory or I/O */
#define	MSR_P5_CESR_ES_CODE_READ       0x001100	/* Code Read */
#define	MSR_P5_CESR_ES_CODE_TLB_MISS   0x001101	/* Code TLB miss */
#define	MSR_P5_CESR_ES_CODE_CACHE_MISS 0x001110	/* Code Cache miss */
#define	MSR_P5_CESR_ES_SEGMENT_LOADED  0x001111	/* Any segment reg. loaded */
#define	MSR_P5_CESR_ES_BRANCHE	       0x010010	/* Branches */
#define	MSR_P5_CESR_ES_BTB_HIT	       0x010011	/* BTB Hits */
#define	MSR_P5_CESR_ES_BRANCHE_BTB     0x010100	/* Taken branch or BTB Hit */
#define	MSR_P5_CESR_ES_PIPELINE_FLUSH  0x010101	/* Pipeline Flushes */
#define	MSR_P5_CESR_ES_INSTRUCTION     0x010110	/* Instruction executed */
#define	MSR_P5_CESR_ES_INSTRUCTION_V   0x010111	/* Inst. executed (v-pipe) */
#define	MSR_P5_CESR_ES_BUS_CYCLE       0x011000	/* Clocks while bus cycle */
#define	MSR_P5_CESR_ES_FULL_WRITE_BUF  0x011001	/* Clocks while full wrt buf. */
#define	MSR_P5_CESR_ES_DATA_MEM_READ   0x011010	/* Pipeline waiting for read */
#define	MSR_P5_CESR_ES_WRITE_EM        0x011011	/* Stall on write E|M state */
#define	MSR_P5_CESR_ES_LOCKED_CYCLE    0x011100	/* Locked bus cycles */
#define	MSR_P5_CESR_ES_IO_CYCLE	       0x011101	/* I/O Read or Write cycles */
#define	MSR_P5_CESR_ES_NON_CACHEABLE   0x011110	/* Non-cacheable Mem. read */
#define	MSR_P5_CESR_ES_AGI             0x011111	/* Stall because of AGI */
#define	MSR_P5_CESR_ES_FLOP            0x100010	/* Floating Point operations */
#define	MSR_P5_CESR_ES_BREAK_DR0       0x100011	/* Breakpoint matches on DR0 */
#define	MSR_P5_CESR_ES_BREAK_DR1       0x100100	/* Breakpoint matches on DR1 */
#define	MSR_P5_CESR_ES_BREAK_DR2       0x100101	/* Breakpoint matches on DR2 */
#define	MSR_P5_CESR_ES_BREAK_DR3       0x100110	/* Breakpoint matches on DR3 */
#define	MSR_P5_CESR_ES_HARDWARE_IT     0x100111	/* Hardware interrupts */

/*
 * CR0
 */
#define	CR0_PG	0x80000000	/*	 Enable paging */
#define	CR0_CD	0x40000000	/* i486: Cache disable */
#define	CR0_NW	0x20000000	/* i486: No write-through */
#define	CR0_AM	0x00040000	/* i486: Alignment check mask */
#define	CR0_WP	0x00010000	/* i486: Write-protect kernel access */
#define	CR0_NE	0x00000020	/* i486: Handle numeric exceptions */
#define	CR0_ET	0x00000010	/*	 Extension type is 80387 */
				                    /*	 (not official) */
#define	CR0_TS	0x00000008	/*	 Task switch */
#define	CR0_EM	0x00000004	/*	 Emulate coprocessor */
#define	CR0_MP	0x00000002	/*	 Monitor coprocessor */
#define	CR0_PE	0x00000001	/*	 Enable protected mode */

/*
 * CR4
 */
#define CR4_SEE		0x00008000	/* Secure Enclave Enable XXX */
#define CR4_SMAP	0x00200000	/* Supervisor-Mode Access Protect */
#define CR4_SMEP	0x00100000	/* Supervisor-Mode Execute Protect */
#define CR4_OSXSAVE	0x00040000	/* OS supports XSAVE */
#define CR4_PCIDE	0x00020000	/* PCID Enable */
#define CR4_RDWRFSGS	0x00010000	/* RDWRFSGS Enable */
#define CR4_SMXE	0x00004000	/* Enable SMX operation */
#define CR4_VMXE	0x00002000	/* Enable VMX operation */
#define CR4_OSXMM	0x00000400	/* SSE/SSE2 exception support in OS */
#define CR4_OSFXS	0x00000200	/* SSE/SSE2 OS supports FXSave */
#define CR4_PCE		0x00000100	/* Performance-Monitor Count Enable */
#define CR4_PGE		0x00000080	/* Page Global Enable */
#define	CR4_MCE		0x00000040	/* Machine Check Exceptions */
#define CR4_PAE		0x00000020	/* Physical Address Extensions */
#define	CR4_PSE		0x00000010	/* Page Size Extensions */
#define	CR4_DE		0x00000008	/* Debugging Extensions */
#define	CR4_TSD		0x00000004	/* Time Stamp Disable */
#define	CR4_PVI		0x00000002	/* Protected-mode Virtual Interrupts */
#define	CR4_VME		0x00000001	/* Virtual-8086 Mode Extensions */

/*
 * XCR0 - XFEATURE_ENABLED_MASK (a.k.a. XFEM) register
 */
#define XCR0_X87 	(1ULL << 0)	/* x87, FPU/MMX (always set) */
#define XCR0_SSE	(1ULL << 1)	/* SSE supported by XSAVE/XRESTORE */
#define	XCR0_YMM	(1ULL << 2)	/* YMM state available */
#define	XCR0_BNDREGS	(1ULL << 3)	/* MPX Bounds register state */
#define	XCR0_BNDCSR	(1ULL << 4)	/* MPX Bounds configuration/state  */
#define XFEM_X87	XCR0_X87
#define XFEM_SSE	XCR0_SSE
#define	XFEM_YMM	XCR0_YMM
#define	XFEM_BNDREGS	XCR0_BNDREGS
#define	XFEM_BNDCSR	XCR0_BNDCSR
#define XCR0 (0)

#define	PMAP_PCID_PRESERVE (1ULL << 63)
#define	PMAP_PCID_MASK     (0xFFF)

#define MSR_IA32_P5_MC_ADDR             0
#define MSR_IA32_P5_MC_TYPE             1
#define MSR_IA32_PLATFORM_ID			0x17
#define MSR_IA32_EBL_CR_POWERON			0x2a

#define MSR_IA32_APIC_BASE              0x1b
#define     MSR_IA32_APIC_BASE_BSP      (1<<8)
#define     MSR_IA32_APIC_BASE_EXTENDED	(1<<10)
#define     MSR_IA32_APIC_BASE_ENABLE	(1<<11)
#define     MSR_IA32_APIC_BASE_BASE		(0xfffff<<12)

#define MSR_CORE_THREAD_COUNT			0x35

#define MSR_IA32_FEATURE_CONTROL		0x3a
#define     MSR_IA32_FEATCTL_LOCK		(1<<0)
#define     MSR_IA32_FEATCTL_VMXON_SMX	(1<<1)
#define     MSR_IA32_FEATCTL_VMXON		(1<<2)
#define     MSR_IA32_FEATCTL_CSTATE_SMI	(1<<16)

#define MSR_IA32_UPDT_TRIG			0x79
#define MSR_IA32_BIOS_SIGN_ID		0x8b
#define MSR_IA32_UCODE_WRITE		MSR_IA32_UPDT_TRIG
#define MSR_IA32_UCODE_REV			MSR_IA32_BIOS_SIGN_ID

#define MSR_IA32_PERFCTR0			0xc1
#define MSR_IA32_PERFCTR1			0xc2
#define MSR_IA32_PERFCTR3			0xc3
#define MSR_IA32_PERFCTR4			0xc4

#define MSR_PLATFORM_INFO			0xce

#define MSR_IA32_MPERF				0xE7
#define MSR_IA32_APERF				0xE8

#define MSR_IA32_BBL_CR_CTL			0x119

#define MSR_IA32_SYSENTER_CS		0x174
#define MSR_IA32_SYSENTER_ESP		0x175
#define MSR_IA32_SYSENTER_EIP		0x176

#define MSR_IA32_MCG_CAP			0x179
#define MSR_IA32_MCG_STATUS			0x17a
#define MSR_IA32_MCG_CTL			0x17b

#define MSR_IA32_EVNTSEL0			0x186
#define MSR_IA32_EVNTSEL1			0x187
#define MSR_IA32_EVNTSEL2			0x188
#define MSR_IA32_EVNTSEL3			0x189

#define MSR_FLEX_RATIO				0x194
#define MSR_IA32_PERF_STS			0x198
#define MSR_IA32_PERF_CTL			0x199
#define MSR_IA32_CLOCK_MODULATION	0x19a

#define MSR_IA32_MISC_ENABLE		0x1a0


#define MSR_IA32_PACKAGE_THERM_STATUS		0x1b1
#define MSR_IA32_PACKAGE_THERM_INTERRUPT	0x1b2

#define MSR_IA32_DEBUGCTLMSR			0x1d9
#define MSR_IA32_LASTBRANCHFROMIP		0x1db
#define MSR_IA32_LASTBRANCHTOIP			0x1dc
#define MSR_IA32_LASTINTFROMIP			0x1dd
#define MSR_IA32_LASTINTTOIP			0x1de

#define MSR_IA32_CR_PAT                 0x277

#define MSR_IA32_MTRRCAP                0xfe
#define MSR_IA32_MTRR_DEF_TYPE			0x2ff
#define MSR_IA32_MTRR_PHYSBASE(n)		(0x200 + 2*(n))
#define MSR_IA32_MTRR_PHYSMASK(n)		(0x200 + 2*(n) + 1)
#define MSR_IA32_MTRR_FIX64K_00000		0x250
#define MSR_IA32_MTRR_FIX16K_80000		0x258
#define MSR_IA32_MTRR_FIX16K_A0000		0x259
#define MSR_IA32_MTRR_FIX4K_C0000		0x268
#define MSR_IA32_MTRR_FIX4K_C8000		0x269
#define MSR_IA32_MTRR_FIX4K_D0000		0x26a
#define MSR_IA32_MTRR_FIX4K_D8000		0x26b
#define MSR_IA32_MTRR_FIX4K_E0000		0x26c
#define MSR_IA32_MTRR_FIX4K_E8000		0x26d
#define MSR_IA32_MTRR_FIX4K_F0000		0x26e
#define MSR_IA32_MTRR_FIX4K_F8000		0x26f

#define MSR_IA32_PERF_FIXED_CTR0		0x309

#define MSR_IA32_PERF_FIXED_CTR_CTRL	0x38D
#define MSR_IA32_PERF_GLOBAL_STATUS		0x38E
#define MSR_IA32_PERF_GLOBAL_CTRL		0x38F
#define MSR_IA32_PERF_GLOBAL_OVF_CTRL	0x390

#define MSR_IA32_PKG_C3_RESIDENCY		0x3F8
#define MSR_IA32_PKG_C6_RESIDENCY		0x3F9
#define MSR_IA32_PKG_C7_RESIDENCY		0x3FA

#define MSR_IA32_CORE_C3_RESIDENCY 		0x3FC
#define MSR_IA32_CORE_C6_RESIDENCY 		0x3FD
#define MSR_IA32_CORE_C7_RESIDENCY 		0x3FE

#define MSR_IA32_MC0_CTL			0x400
#define MSR_IA32_MC0_STATUS			0x401
#define MSR_IA32_MC0_ADDR			0x402
#define MSR_IA32_MC0_MISC			0x403

#define MSR_IA32_VMX_BASE					0x480
#define MSR_IA32_VMX_BASIC					MSR_IA32_VMX_BASE
#define MSR_IA32_VMX_PINBASED_CTLS			MSR_IA32_VMX_BASE+1
#define MSR_IA32_VMX_PROCBASED_CTLS			MSR_IA32_VMX_BASE+2
#define MSR_IA32_VMX_EXIT_CTLS				MSR_IA32_VMX_BASE+3
#define MSR_IA32_VMX_ENTRY_CTLS				MSR_IA32_VMX_BASE+4
#define MSR_IA32_VMX_MISC					MSR_IA32_VMX_BASE+5
#define MSR_IA32_VMX_CR0_FIXED0				MSR_IA32_VMX_BASE+6
#define MSR_IA32_VMX_CR0_FIXED1				MSR_IA32_VMX_BASE+7
#define MSR_IA32_VMX_CR4_FIXED0				MSR_IA32_VMX_BASE+8
#define MSR_IA32_VMX_CR4_FIXED1				MSR_IA32_VMX_BASE+9
#define MSR_IA32_VMX_VMCS_ENUM				MSR_IA32_VMX_BASE+10
#define MSR_IA32_VMX_PROCBASED_CTLS2		MSR_IA32_VMX_BASE+11
#define MSR_IA32_VMX_EPT_VPID_CAP			MSR_IA32_VMX_BASE+12
#define	MSR_IA32_VMX_EPT_VPID_CAP_AD_SHIFT                    21
#define MSR_IA32_VMX_TRUE_PINBASED_CTLS		MSR_IA32_VMX_BASE+13
#define MSR_IA32_VMX_TRUE_PROCBASED_CTLS	MSR_IA32_VMX_BASE+14
#define MSR_IA32_VMX_TRUE_VMEXIT_CTLS		MSR_IA32_VMX_BASE+15
#define MSR_IA32_VMX_TRUE_VMENTRY_CTLS		MSR_IA32_VMX_BASE+16
#define MSR_IA32_VMX_VMFUNC					MSR_IA32_VMX_BASE+17

#define MSR_IA32_DS_AREA                0x600

#define MSR_IA32_PKG_POWER_SKU_UNIT		0x606
#define MSR_IA32_PKG_C2_RESIDENCY		0x60D
#define MSR_IA32_PKG_ENERGY_STATUS		0x611
#define MSR_IA32_DDR_ENERGY_STATUS		0x619
#define MSR_IA32_LLC_FLUSHED_RESIDENCY_TIMER	0x61D
#define MSR_IA32_RING_PERF_STATUS		0x621

#define MSR_IA32_PKG_C8_RESIDENCY		0x630
#define MSR_IA32_PKG_C9_RESIDENCY		0x631
#define MSR_IA32_PKG_C10_RESIDENCY		0x632

#define MSR_IA32_PP0_ENERGY_STATUS		0x639
#define MSR_IA32_PP1_ENERGY_STATUS		0x641
#define MSR_IA32_IA_PERF_LIMIT_REASONS_SKL	0x64F

#define MSR_IA32_IA_PERF_LIMIT_REASONS		0x690
#define MSR_IA32_GT_PERF_LIMIT_REASONS		0x6B0

#define MSR_IA32_TSC_DEADLINE			0x6e0

#define	MSR_IA32_EFER                   0xC0000080
#define	MSR_IA32_EFER_SCE			    0x00000001
#define	MSR_IA32_EFER_LME			    0x00000100
#define	MSR_IA32_EFER_LMA			    0x00000400
#define MSR_IA32_EFER_NXE			    0x00000800

#define	MSR_IA32_STAR                   0xC0000081
#define	MSR_IA32_LSTAR				    0xC0000082
#define	MSR_IA32_CSTAR				    0xC0000083
#define	MSR_IA32_FMASK				    0xC0000084

#define MSR_IA32_FS_BASE			    0xC0000100
#define MSR_IA32_GS_BASE			    0xC0000101
#define MSR_IA32_KERNEL_GS_BASE         0xC0000102
#define MSR_IA32_TSC_AUX			    0xC0000103

#endif