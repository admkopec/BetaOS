//
//  interupts.cpp
//  OS
//
//  Created by Adam Kopeć on 9/27/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include "interrupts.hpp"

inline uint32_t Interrupts::farpeekl(uint16_t sel, void* off) {
    uint32_t ret;
    __asm__ ( "push %%fs\n\t"
         "mov  %1, %%fs\n\t"
         "mov  %%fs:(%2), %0\n\t"
         "pop  %%fs"
         : "=r"(ret) : "g"(sel), "r"(off) );
    return ret;
}

inline void Interrupts::farpokeb(uint16_t sel, void* off, uint8_t v) {
    __asm__ ( "push %%fs\n\t"
         "mov  %0, %%fs\n\t"
         "movb %2, %%fs:(%1)\n\t"
         "pop %%fs"
         : : "g"(sel), "r"(off), "r"(v) );
    /* TODO: Should "memory" be in the clobber list here? */
}

inline uint8_t Interrupts::inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__ ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
    return ret;
}

inline void Interrupts::outb(uint16_t port, uint8_t val)
{
    __asm__ __volatile__ ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
}

inline bool Interrupts::are_interrupts_enabled()
{
    unsigned long flags;
    __asm__ __volatile__ ( "pushf\n\t"
                  "pop %0"
                  : "=g"(flags) );
    return flags & (1 << 9);
}