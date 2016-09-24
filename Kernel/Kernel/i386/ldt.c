//
//  ldt.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/27/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/seg.h>

struct real_descriptor	master_ldt[LDTSZ] __attribute__ ((aligned (4096))) = {
    [SEL_TO_INDEX(USER_CTHREAD)] = MAKE_REAL_DESCRIPTOR(	/* user cthread segment */
                                                        0,
                                                        0xfffff,
                                                        SZ_32|SZ_G,
                                                        ACC_P|ACC_PL_U|ACC_DATA_W
                                                        ),
};