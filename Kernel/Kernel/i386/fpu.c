//
//  fpu.c
//  Kernel
//
//  Created by Adam Kopeć on 8/26/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "fpu.h"
#include "proc_reg.h"
#include "cpuid.h"
#include "cpu_data.h"
#include "misc_protos.h"

int fp_kind = FP_NO;

//static unsigned int mxcsr_capability_mask;

#define    fninit() \
__asm__ volatile("fninit")

#define    fnstcw(control) \
__asm__("fnstcw %0" : "=m" (*(unsigned short *)(control)))

#define    fldcw(control) \
__asm__ volatile("fldcw %0" : : "m" (*(unsigned short *) &(control)) )

#define    fnclex() \
__asm__ volatile("fnclex")

#define    fnsave(state)  \
__asm__ volatile("fnsave %0" : "=m" (*state))

#define    frstor(state) \
__asm__ volatile("frstor %0" : : "m" (state))

#define fwait() \
__asm__("fwait");

#define fxrstor(addr) __asm__ __volatile__("fxrstor %0" : : "m" (*(addr)))
#define fxsave(addr)  __asm__ __volatile__("fxsave  %0" :  "=m" (*(addr)))

static uint32_t    fp_register_state_size = 0;
static uint32_t    fpu_YMM_present        = false;

/*static void fpu_store_registers(void *, boolean_t);
static void fpu_load_registers(void *);*/

extern    void xsave64o(void);
extern    void xrstor64o(void);

#define XMASK ((uint32_t) (XFEM_X87 | XFEM_SSE | XFEM_YMM))

/*static inline void xsetbv(uint32_t mask_hi, uint32_t mask_lo) {
    __asm__ __volatile__("xsetbv" :: "a"(mask_lo), "d"(mask_hi), "c" (XCR0));
}

static inline void xsave(struct x86_fx_thread_state *a) {
    __asm__ __volatile__("xsave %0" :"=m" (*a) : "a"(XMASK), "d"(0));
}

static inline void xrstor(struct x86_fx_thread_state *a) {
    __asm__ __volatile__("xrstor %0" ::  "m" (*a), "a"(XMASK), "d"(0));
}*/

#ifdef DEBUG
static inline unsigned short
fnstsw(void) {
    unsigned short status;
    __asm__ volatile("fnstsw %0" : "=ma" (status));
    return (status);
}
#endif

/*
 * Look for FPU and initialize it.
 * Called on each CPU.
 */
void
init_fpu(void) {
#ifdef DEBUG
    unsigned short    status;
    unsigned short     control;
#endif
    /*
     * Check for FPU by initializing it,
     * then trying to read the correct bit patterns from
     * the control and status registers.
     */
    set_cr0((get_cr0() & ~(CR0_EM|CR0_TS)) | CR0_NE);    /* allow use of FPU */
    fninit();
#ifdef DEBUG
    status = fnstsw();
    fnstcw(&control);
    
    assert(((status & 0xff) == 0) && ((control & 0x103f) == 0x3f));
#endif
    /* Advertise SSE support */
    if (cpuid_features() & CPUID_FEATURE_FXSR) {
        fp_kind = FP_FXSR;
        set_cr4(get_cr4() | CR4_OSFXS);
        /* And allow SIMD exceptions if present */
        if (cpuid_features() & CPUID_FEATURE_SSE) {
            set_cr4(get_cr4() | CR4_OSXMM);
        }
        fp_register_state_size = sizeof(struct x86_fx_thread_state);
        
    } else
        panic("fpu is not FP_FXSR");
    
    /* Configure the XSAVE context mechanism if the processor supports
     * AVX/YMM registers
     */
    /*if (cpuid_features() & CPUID_FEATURE_XSAVE) {
        cpuid_xsave_leaf_t *xsp = &cpuid_info()->cpuid_xsave_leaf[0];
        if (xsp->extended_state[0] & (uint32_t)XFEM_YMM) {
            assert(xsp->extended_state[0] & (uint32_t) XFEM_SSE);
            // XSAVE container size for all features
            fp_register_state_size = sizeof(struct x86_avx_thread_state);
            fpu_YMM_present = true;
            set_cr4(get_cr4() | CR4_OSXSAVE);
            xsetbv(0, XMASK);
            // Verify that now selected state can be accommodated
            assert(xsp->extended_state[1] == fp_register_state_size);
        }
    }
    else*/
        fpu_YMM_present = false;
    
    fpinit();
}

void
clear_fpu(void) {
    set_ts();
}

void
fpinit(void) {
    unsigned short    control;
    
    clear_ts();
    fninit();
    fnstcw(&control);
    control &= ~(FPC_PC|FPC_RC); /* Clear precision & rounding control */
    control |= (FPC_PC_64 |        /* Set precision */
                FPC_RC_RN |     /* round-to-nearest */
                FPC_ZE |    /* Suppress zero-divide */
                FPC_OE |    /*  and overflow */
                FPC_UE |    /*  underflow */
                FPC_IE |    /* Allow NaNQs and +-INF */
                FPC_DE |    /* Allow denorms as operands  */
                FPC_PE);    /* No trap for precision loss */
    fldcw(control);
    
    /* Initialize SSE/SSE2 */
    __builtin_ia32_ldmxcsr(0x1f80);
}
