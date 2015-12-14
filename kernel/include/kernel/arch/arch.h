//
//  arch.h
//  OS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _KERNEL_ARCH_H
#define _KERNEL_ARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <kernel/interrupts.h>

void reboot();
void shutdown();

#endif /* arch_h */