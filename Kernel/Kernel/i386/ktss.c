//
//  ktss.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/30/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "tss.h"
#include "pmap.h"

/*
 * The transient stack for sysenter.
 * At its top is a 32-bit link to the PCB in legacy mode, 64-bit otherwise.
 * NB: it also must be large enough to contain a interrupt stack frame
 * due to a single-step trace trap at system call entry.
 */

struct sysenter_stack master_sstk
#ifdef __ELF__
__attribute__ ((section (".data")))
#else
__attribute__ ((section ("__DESC, master_sstk")))
#endif
__attribute__ ((aligned (16)))  = { {0}, 0 };

struct x86_64_tss master_ktss64 __attribute__ ((aligned (4096))) = {
    .io_bit_map_offset = 0x0FFF,
};
