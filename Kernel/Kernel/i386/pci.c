//
//  pci.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/17/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <i386/pci.h>
#include <i386/pio.h>
#include <stdio.h>
#include "misc_protos.h"

uint32_t pciGetConfig(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    /* write out the address */
    outl (0xCF8, address);
    return inl(0xCFC);
}

uint16_t pciGetVendor(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint16_t)(pciGetConfig(bus,slot, func,0));
}

uint16_t pciGetDevice(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint16_t)(pciGetConfig(bus,slot, func,0) >> 16);
}
