//
//  gdt.c
//  BetaOS
//
//  Created by Adam Kopeć on 4/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/seg.h>
#include <i386/pmap.h>

struct real_descriptor master_gdt[GDTSZ]
/*__attribute__((section("__HIB,__desc")))*/
__attribute__((section(".data")))
__attribute__((aligned(PAGE_SIZE))) = {
    [SEL_TO_INDEX(KERNEL32_CS)] = MAKE_REAL_DESCRIPTOR(	/* kernel 32-bit code */
                                                       0,
                                                       0xfffff,
                                                       SZ_32|SZ_G,
                                                       ACC_P|ACC_PL_K|ACC_CODE_R
                                                       ),
    [SEL_TO_INDEX(KERNEL_DS)]   =  MAKE_REAL_DESCRIPTOR( /* kernel data */
                                                        0,
                                                        0xfffff,
                                                        SZ_32|SZ_G,
                                                        ACC_P|ACC_PL_K|ACC_DATA_W
                                                        ),
    [SEL_TO_INDEX(KERNEL64_CS)] = MAKE_REAL_DESCRIPTOR(	/* kernel 64-bit code */
                                                        0,
                                                        0xfffff,
                                                        SZ_64|SZ_G,
                                                        ACC_P|ACC_PL_K|ACC_CODE_R
                                                       ),
    [SEL_TO_INDEX(KERNEL64_SS)] = MAKE_REAL_DESCRIPTOR(	/* kernel 64-bit syscall stack */
                                                        0,
                                                        0xfffff,
                                                        SZ_32|SZ_G,
                                                        ACC_P|ACC_PL_K|ACC_DATA_W
                                                       ),
    [SEL_TO_INDEX(USER_CS)]     = MAKE_REAL_DESCRIPTOR(	/* 32-bit user code segment */
                                                        0,
                                                        0xfffff,
                                                        SZ_32|SZ_G,
                                                        ACC_P|ACC_PL_U|ACC_CODE_R
                                                       ),
    [SEL_TO_INDEX(USER_DS)]     = MAKE_REAL_DESCRIPTOR(	/* 32-bit user data segment */
                                                        0,
                                                        0xfffff,
                                                        SZ_32|SZ_G,
                                                        ACC_P|ACC_PL_U|ACC_DATA_W
                                                       ),
    [SEL_TO_INDEX(USER64_CS)]   = MAKE_REAL_DESCRIPTOR( /* user 64-bit code segment */
                                                        0,
                                                        0xfffff,
                                                        SZ_64|SZ_G,
                                                        ACC_P|ACC_PL_U|ACC_CODE_R
                                                       ),
};
