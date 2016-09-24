//
//  mp.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/mp.h>
#include <i386/pmap.h>

char		mp_slave_stack[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE))); // Temp stack for slave init