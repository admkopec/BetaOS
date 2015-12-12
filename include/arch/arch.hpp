//
//  arch.hpp
//  OS
//
//  Created by Adam Kopeć on 12/10/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef arch_hpp
#define arch_hpp
#include <stdint.h>
//#define IDTBASE 0x00000000
//#define IDTSIZE 0xFF
//#define KERN_STACK 0x0009FFF0
//#define INTGATE  0x8E00
//#define TRAPGATE 0xEF00

/*struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));



struct idtdesc {
    uint16_t offset0_15;
    uint16_t select;
    uint16_t type;
    uint16_t offset16_31;
} __attribute__ ((packed));*/

char* cpuid();
void reboot();
void shutdown();

//void init_idt(void);        //Move to different files
//void init_pic(void);

void *memcpy(char *dst, char *src, int n);

#endif /* arch_hpp */
