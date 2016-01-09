//
//  arch.c
//  OS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <kernel/arch/arch.h>

void reboot() {
  unsigned char good = 0x02;                    // Future: Move power () to apm.c
  while ((good & 0x02) != 0)
    good = inb(0x64);
  outb(0x64, 0xFE);
}

void shutdown() {
  printf("Will be added in version 1.1\nBeta will be available sooner\n");
}
