//
//  interupts.hpp
//  OS
//
//  Created by Adam Kopeć on 9/27/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#ifndef interupts_hpp
#define interupts_hpp

#include <stdbool.h> /* C doesn't have booleans by default. */
#include <stddef.h>
#include <stdint.h>

class Interrupts {
public:
    /* Memory Access */
    /* Read */
    static inline uint32_t farpeekl(uint16_t sel, void* off);
    /* Write */
    static inline void farpokeb(uint16_t sel, void* off, uint8_t v);
    
    /* IO Access */
    /* Read */
    static inline uint8_t inb(uint16_t port);
    /* Write */
    static inline void outb(uint16_t port, uint8_t val);
    
    /* Enabled */
    static inline bool are_interrupts_enabled();
};

#endif /* interupts_hpp */
