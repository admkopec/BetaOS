//
//  fpu.h
//  Kernel
//
//  Created by Adam Kopeć on 8/26/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef fpu_h
#define fpu_h

#include <stdlib.h>

typedef enum {
    FXSAVE32 = 1,
    FXSAVE64 = 2,
    XSAVE32  = 3,
    XSAVE64  = 4,
    FP_UNUSED = 5
} fp_save_layout_t;

extern int  fp_kind;

extern void init_fpu(void);
extern void fpinit(void);
extern void fpu_module_init(void);
extern void fpnoextflt(void);
extern void fpextovrflt(void);
extern void fpexterrflt(void);
extern void fpSSEexterrflt(void);
extern void clear_fpu(void);

struct x86_fx_thread_state {
    unsigned short  fx_control;     /* control */
    unsigned short  fx_status;      /* status */
    unsigned char   fx_tag;         /* register tags */
    unsigned char   fx_bbz1;        /* better be zero when calling fxrtstor */
    unsigned short  fx_opcode;
    unsigned int    fx_eip;         /* eip  instruction */
    unsigned short  fx_cs;          /* cs instruction */
    unsigned short  fx_bbz2;        /* better be zero when calling fxrtstor */
    unsigned int    fx_dp;          /* data address */
    unsigned short  fx_ds;          /* data segment */
    unsigned short  fx_bbz3;        /* better be zero when calling fxrtstor */
    unsigned int    fx_MXCSR;
    unsigned int    fx_MXCSR_MASK;
    unsigned short  fx_reg_word[8][8];    /* STx/MMx registers */
    unsigned short  fx_XMM_reg[8][16];    /* XMM0-XMM15 on 64 bit processors */
    /* XMM0-XMM7  on 32 bit processors... unused storage reserved */
    
    unsigned char   fx_reserved[16*5];    /* reserved by intel for future
                                           * expansion */
    unsigned int    fp_valid;
    unsigned int    fp_save_layout;
    unsigned char   fx_pad[8];
} __attribute__((packed));

struct x86_avx_thread_state {
    unsigned short  fx_control;     /* control */
    unsigned short  fx_status;      /* status */
    unsigned char   fx_tag;         /* register tags */
    unsigned char   fx_bbz1;        /* reserved zero */
    unsigned short  fx_opcode;
    unsigned int    fx_eip;         /* eip  instruction */
    unsigned short  fx_cs;          /* cs instruction */
    unsigned short  fx_bbz2;        /* reserved zero */
    unsigned int    fx_dp;          /* data address */
    unsigned short  fx_ds;          /* data segment */
    unsigned short  fx_bbz3;        /* reserved zero */
    unsigned int    fx_MXCSR;
    unsigned int    fx_MXCSR_MASK;
    unsigned short  fx_reg_word[8][8];    /* STx/MMx registers */
    unsigned short  fx_XMM_reg[8][16];    /* XMM0-XMM15 on 64 bit processors */
    /* XMM0-XMM7  on 32 bit processors... unused storage reserved */
    unsigned char   fx_reserved[16*5];    /* reserved */
    unsigned int    fp_valid;
    unsigned int    fp_save_layout;
    unsigned char   fx_pad[8];
    
    struct    xsave_header {            /* Offset 512, xsave header */
        uint64_t xsbv;
        char    xhrsvd[56];
    }_xh;
    
    unsigned int    x_YMMH_reg[4][16];    /* Offset 576, high YMMs*/
} __attribute__((packed));

/*
 * Control register
 */
#define    FPC_IE        0x0001        /* enable invalid operation exception */
#define    FPC_IM        FPC_IE
#define    FPC_DE        0x0002        /* enable denormalized operation exception */
#define    FPC_DM        FPC_DE
#define    FPC_ZE        0x0004        /* enable zero-divide exception */
#define    FPC_ZM        FPC_ZE
#define    FPC_OE        0x0008        /* enable overflow exception */
#define    FPC_OM        FPC_OE
#define    FPC_UE        0x0010        /* enable underflow exception */
#define    FPC_PE        0x0020        /* enable precision exception */
#define    FPC_PC        0x0300        /* precision control: */
#define    FPC_PC_24     0x0000            /* 24 bits */
#define    FPC_PC_53     0x0200            /* 53 bits */
#define    FPC_PC_64     0x0300            /* 64 bits */
#define    FPC_RC        0x0c00        /* rounding control: */
#define    FPC_RC_RN     0x0000            /* round to nearest or even */
#define    FPC_RC_RD     0x0400            /* round down */
#define    FPC_RC_RU     0x0800            /* round up */
#define    FPC_RC_CHOP   0x0c00            /* chop */
#define    FPC_IC        0x1000        /* infinity control (obsolete) */
#define    FPC_IC_PROJ   0x0000            /* projective infinity */
#define    FPC_IC_AFF    0x1000            /* affine infinity (std) */

/*
 * Status register
 */
#define    FPS_IE        0x0001        /* invalid operation */
#define    FPS_DE        0x0002        /* denormalized operand */
#define    FPS_ZE        0x0004        /* divide by zero */
#define    FPS_OE        0x0008        /* overflow */
#define    FPS_UE        0x0010        /* underflow */
#define    FPS_PE        0x0020        /* precision */
#define    FPS_SF        0x0040        /* stack flag */
#define    FPS_ES        0x0080        /* error summary */
#define    FPS_C0        0x0100        /* condition code bit 0 */
#define    FPS_C1        0x0200        /* condition code bit 1 */
#define    FPS_C2        0x0400        /* condition code bit 2 */
#define    FPS_TOS       0x3800        /* top-of-stack pointer */
#define    FPS_TOS_SHIFT     11
#define    FPS_C3        0x4000        /* condition code bit 3 */
#define    FPS_BUSY      0x8000        /* FPU busy */

/*
 * Kind of floating-point support provided by kernel.
 */
#define    FP_NO         0        /* no floating point */
#define    FP_SOFT       1        /* software FP emulator */
#define    FP_287        2        /* 80287 */
#define    FP_387        3        /* 80387 or 80486 */
#define    FP_FXSR       4        /* Fast save/restore SIMD Extension */

#endif /* fpu_h */
