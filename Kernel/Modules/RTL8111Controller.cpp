//
//  RTL8111Controller.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/20/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "RTL8111Controller.hpp"
#include "MMIOUtils.hpp"
#include <i386/pci.h>
#include <i386/pio.h>


uint8_t     bar_type = 0;       // Type of BOR0
uint16_t    io_base  = 0xD001;        // IO Base Address
uint64_t    mem_base = 0xEA10000C;       // MMIO Base Address
bool        eeprom_exists = false;

RTL8111::RTL8111() {
    bar_type = 1;
    io_base  = 0xD001;
    eeprom_exists = false;
    //detectEEProm();
}

#define REG_EEPROM      0x0014

void /*RTL8111::*/writeCommand( uint16_t p_address, uint32_t p_value) {
    if ( bar_type == 0 ) {
        MMIOUtils::write32(mem_base+p_address,p_value);
    } else {
        outl(io_base, p_address);
        outl(io_base + 4, p_value);
    }
}
uint32_t /*RTL8111::*/readCommand( uint16_t p_address) {
    if ( bar_type == 0 ) {
        return MMIOUtils::read32(mem_base+p_address);
    } else {
        outl(io_base, p_address);
        return inl(io_base + 4);
    }
}

bool /*RTL8111::*/detectEEProm() {
    uint32_t val = 0;
    writeCommand(REG_EEPROM, 0x1);
    
    for(int i = 0; i < 1000 && !eeprom_exists; i++)
    {
        val = readCommand(REG_EEPROM);
        if(val & 0x10)
            eeprom_exists = true;
        else
            eeprom_exists = false;
    }
    return eeprom_exists;
}

void testEEPRom() {
    if (detectEEProm()) {
        printf("EEPRom detected!\n");
    } else printf("EEPRom NOT detected!\n");
}
