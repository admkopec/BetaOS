//
//  lowglobals.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef lowglobals_h
#define lowglobals_h

#include <stdint.h>

#ifndef __x86_64__
#error	Wrong architecture - this file is meant for x86_64
#endif

/*
 * Don't change these structures unless you change the corresponding assembly code
 * which is in lowmem_vectors.s
 */

#pragma pack(8)		/* Make sure the structure stays as we defined it */
typedef struct lowglo {
    
    unsigned char	lgVerCode[8];	/* 0xffffff8000002000 System verification code */
    uint64_t	lgZero;             /* 0xffffff8000002008 Double constant 0 */
    uint64_t	lgStext;            /* 0xffffff8000002010 Start of kernel text */
    uint64_t	lgRsv018;           /* 0xffffff8000002018 Reserved */
    uint64_t	lgCHUDXNUfnStart;	/* 0xffffff8000002020 CHUD XNU function glue table */
    uint64_t	lgRsv028;           /* 0xffffff8000002028 Reserved */
    uint64_t	lgVersion;          /* 0xffffff8000002030 Pointer to kernel version string */
    uint64_t	lgRsv038[280];		/* 0xffffff8000002038 Reserved */
    uint64_t	lgKmodptr;          /* 0xffffff80000028f8 Pointer to kmod, debugging aid */
    uint64_t	lgTransOff;         /* 0xffffff8000002900 Pointer to kdp_trans_off, debugging aid */
    uint64_t	lgReadIO;           /* 0xffffff8000002908 Pointer to kdp_read_io, debugging aid */
    uint64_t	lgDevSlot1;         /* 0xffffff8000002910 For developer use */
    uint64_t	lgDevSlot2;         /* 0xffffff8000002918 For developer use */
    uint64_t	lgOSVersion;		/* 0xffffff8000002920 Pointer to OS version string */
    uint64_t	lgRebootFlag;		/* 0xffffff8000002928 Pointer to debugger reboot trigger */
    uint64_t	lgManualPktAddr;	/* 0xffffff8000002930 Pointer to manual packet structure */
    
    uint64_t	lgRsv938[217];		/* 0xffffff8000002938 Reserved - push to 1 page */
} lowglo;
#pragma pack()
extern lowglo lowGlo;

#endif /* lowglobals_h */
