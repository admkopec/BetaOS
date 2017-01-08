//
//  pio.h
//  OS
//
//  Created by Adam Kopeć on 9/27/15 and changed name to "pio.h" (conflicting name with PICs' files) on 4/30/16.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef interupts_h
#define interupts_h

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


    /* Memory Access */
    /* Read */
    static inline unsigned long farpeekl(unsigned int sel, void* off)  {
        unsigned long ret;
        __asm__ ( "push %%fs\n\t"
                "mov  %1, %%fs\n\t"
                "mov  %%fs:(%2), %0\n\t"
                "pop  %%fs"
                : "=r"(ret) : "g"(sel), "r"(off) );
        return ret;
    }
    /* Write */
    static inline void farpokeb(unsigned int sel, void* off, unsigned char v) {
        __asm__ ( "push %%fs\n\t"
                 "mov  %0, %%fs\n\t"
                 "movb %2, %%fs:(%1)\n\t"
                 "pop %%fs"
                 : : "g"(sel), "r"(off), "r"(v) );
    }

    /* IO Access */
    /* Read */
    static inline uint8_t inb(unsigned int port) {
        uint8_t ret;
        __asm__ __volatile__ ( "inb %w1, %0" : "=a"(ret) : "Nd"(port) );
        return ret;
    }

    static inline uint16_t inw(unsigned int port) {
        uint16_t ret;
        __asm__ __volatile__("inw %w1, %0" : "=a" (ret) : "Nd" (port));
        return ret;
    }

    static inline uint32_t inl(unsigned int port) {
        uint32_t ret;
        __asm__ __volatile__("inl %w1, %0" : "=a" (ret) : "Nd" (port));
        return ret;
    }

    /* Write */
    static inline void outb(unsigned int port, uint8_t val) {
        __asm__ __volatile__ ("outb %b0, %w1" : : "a"(val), "Nd"(port));
    }

    static inline void outw(unsigned int port, uint16_t val) {
        __asm__ __volatile__("outw %w0, %w1" : : "a" (val), "Nd" (port));
    }

    static inline void outl(unsigned int port, uint32_t val) {
        __asm__ __volatile__("outl %0, %w1" : : "a" (val), "Nd" (port));
    }

    /* Wait */
    static inline void io_wait(void) {
        asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    }

#endif /* interupts_h */
