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

int pcidump() {
    //clearScreen();
    kprintf("=========================PCI DUMP========================\n");
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
            if (pciGetVendor(bus, slot, func)==0xFFFF) {
                /* ... */
            } else {
                kprintf("Vendor ID = %x Device ID = %x\n", pciGetVendor(bus, slot, func), pciGetDevice(bus, slot, func));
                //if (/*(bus==0&&slot==0)||(bus==2&&slot==0)*/0) {
                kprintf("%x",pciGetConfig(bus, slot, func, 0x0));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, func, 0x4));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, func, 0x8));
                kprintf("\n");
                /*kprintf("%x",pciGetConfig(bus, slot, 0, 0xC));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x10));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x14));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x18));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x1C));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x20));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x24));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x28));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x2C));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x30));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x34));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x38));
                kprintf("\n");
                kprintf("%x",pciGetConfig(bus, slot, 0, 0x3C));
                kprintf("\n");*/
                //}
                for (char c = '\0'; ;) {
                    c = getchar();
                    if (c=='\n') {
                        //clearScreen();
                        break;
                    }
                }
            }
            }
        }
    }
    kprintf("+++++++++++++++++++++++END OF DUMP+++++++++++++++++++++++\n");
    return 0;
}
