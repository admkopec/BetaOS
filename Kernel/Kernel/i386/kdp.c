//
//  kdp.c
//  Kernel
//
//  Created by Adam Kopeć on 2/9/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "misc_protos.h"
#include "pmap.h"


bool kdp_read_io;
bool kdp_trans_off;

pmap_t kdp_pmap = 0;

uint64_t
kdp_vtophys(pmap_t pmap, uint64_t va) {
    ppnum_t  pp;
    
    pp = pmap_find_phys(pmap, va);
    if (!pp) {
        return 0;
    }
    
    return (((uint64_t)pp << PAGE_SHIFT) | (va &PAGE_MASK));
}
