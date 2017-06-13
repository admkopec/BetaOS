//
//  RTL8111Controller.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "RTL8111Controller.hpp"
#include "MMIOUtils.hpp"
#include "PCIController.hpp"
#include <i386/pio.h>

#define super Controller
#define Log(x ...) printf("RTL8111Controller: " x)

//uint8_t     bar_type = 0;                 // Type of BAR0
//uint16_t    io_base  = 0xD001;            // IO Base Address
//uint64_t    mem_base = 0xEA10000C;        // MMIO Base Address
//bool        eeprom_exists = false;

OSReturn
RTL8111::init(PCI *h) {
    if (h->VendorID() != Realtek_Vendor || h->DeviceID() != RTL8168_ID) {
        return kOSReturnFailed;
    }
    
    bar_type = h->getBAR(0);
    Log("BAR type     %X\n", bar_type);
    if (bar_type == 0x1) {
        io_base  = h->BAR().u.port;
        Log("BAR Port:    %X\n", io_base);
    } else {
        mem_base = (uintptr_t)h->BAR().u.address;
        Log("BAR Address: %X\n", mem_base);
    }
    Used_ = true;
    return kOSReturnSuccess;
}

void RTL8111::start() {
    detectEEProm();
    Log("EEProm = %d\n", eeprom_exists);
}

#define REG_EEPROM      0x0014

void RTL8111::writeCommand( uint16_t p_address, uint32_t p_value) {
    if ( bar_type == 0 ) {
        MMIOUtils::write32(mem_base+p_address,p_value);
    } else {
        outl(io_base, p_address);
        outl(io_base + 4, p_value);
    }
}
uint32_t RTL8111::readCommand( uint16_t p_address) {
    if ( bar_type == 0 ) {
        return MMIOUtils::read32(mem_base+p_address);
    } else {
        outl(io_base, p_address);
        return inl(io_base + 4);
    }
}

bool RTL8111::detectEEProm() {
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
