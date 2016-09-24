//
//  MMIOUtils.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "MMIOUtils.hpp"

uint8_t MMIOUtils::read8 (uint64_t p_address) {
    return *((volatile uint8_t*)(p_address));
}
uint16_t MMIOUtils::read16 (uint64_t p_address) {
    return *((volatile uint16_t*)(p_address));
}
uint32_t MMIOUtils::read32 (uint64_t p_address) {
    return *((volatile uint32_t*)(p_address));
}
uint64_t MMIOUtils::read64 (uint64_t p_address) {
    return *((volatile uint64_t*)(p_address));
}
void MMIOUtils::write8 (uint64_t p_address,uint8_t p_value) {
    (*((volatile uint8_t*)(p_address)))=(p_value);
}
void MMIOUtils::write16 (uint64_t p_address,uint16_t p_value) {
    (*((volatile uint16_t*)(p_address)))=(p_value);
}
void MMIOUtils::write32 (uint64_t p_address,uint32_t p_value) {
    (*((volatile uint32_t*)(p_address)))=(p_value);
    
}
void MMIOUtils::write64 (uint64_t p_address,uint64_t p_value) {
    (*((volatile uint64_t*)(p_address)))=(p_value);
}