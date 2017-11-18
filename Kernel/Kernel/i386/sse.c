//
//  sse.c
//  Kernel
//
//  Created by Adam Kopeć on 9/10/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "misc_protos.h"
#include "pal.h"
#include "cpuid.h"
#include "proc_reg.h"

bool isSSEenabled = false;
bool canUseSSEmemcpy = false;

void enable_sse(void) {
    if (cpuid_features() & CPUID_FEATURE_SSE) {
        pal_cr_t cr0 = get_cr0();
        pal_cr_t cr4 = get_cr4();
        cr0 &= ~CR0_EM;
        cr0 |= CR0_MP;
        cr4 |= CR4_OSFXS;
        cr4 |= CR4_OSXMM;
        set_cr0(cr0);
        set_cr4(cr4);
        isSSEenabled = true;
    }
}
