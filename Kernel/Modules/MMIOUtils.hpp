//
//  MMIOUtils.hpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef MMIOUtils_hpp
#define MMIOUtils_hpp

#include <stdint.h>
#include <stddef.h>
#include <OSRuntime.hpp>

class MMIOUtils {
public:
    static uint8_t read8   (uint64_t p_address);
    static uint16_t read16 (uint64_t p_address);
    static uint32_t read32 (uint64_t p_address);
    static uint64_t read64 (uint64_t p_address);
    static void write8  (uint64_t p_address,uint8_t p_value);
    static void write16 (uint64_t p_address,uint16_t p_value);
    static void write32 (uint64_t p_address,uint32_t p_value);
    static void write64 (uint64_t p_address,uint64_t p_value);
};

#endif /* MMIOUtils_hpp */
