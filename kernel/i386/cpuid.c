//
//  cpuid.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/18/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/cpuid.h>

/* Only for 32bit values */
#define bit32(n)		(1U << (n))
#define bitmask32(h,l)		((bit32(h)|(bit32(h)-1)) & ~(bit32(l)-1))
#define bitfield32(x,h,l)	((((x) & bitmask32(h,l)) >> l))

void
cpuid(uint32_t *data) {
    __asm__ volatile ("cpuid"
                      : "=a" (data[eax]),
                      "=b"   (data[ebx]),
                      "=c"   (data[ecx]),
                      "=d"   (data[edx])
                      : "a"  (data[eax]),
                      "b"    (data[ebx]),
                      "c"    (data[ecx]),
                      "d"    (data[edx]));
}

void
do_cpuid(uint32_t selector, uint32_t *data) {
    __asm__ volatile ("cpuid"
                      : "=a" (data[0]),
                      "=b"   (data[1]),
                      "=c"   (data[2]),
                      "=d"   (data[3])
                      : "a"  (selector),
                      "b"    (0),
                      "c"    (0),
                      "d"    (0));
}

char* getBrand() {
    uint32_t reg[4];
    cpuid(reg);
    return "Unkown";
}