//
//  pci.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/17/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/pci.h>
#include <i386/pio.h>
#include <stdio.h>
#include <kernel/tty.h>

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

uint16_t pciGetVendor(uint8_t bus, uint8_t slot) {
    return (uint16_t)(pciGetConfig(bus,slot,0,0));
}

uint16_t pciGetDevice(uint8_t bus, uint8_t slot) {
    return (uint16_t)(pciGetConfig(bus,slot,0,0) >> 16);
}

int pcidump() {
    clearScreen();
    printf("=========================PCI DUMP========================\n");
    for (int bus = 0; bus<=2; bus++) {
        for (int slot = 0; slot<=3; slot++) {
            if (pciGetVendor(bus, slot)==0xFFFF) {
                /* ... */
            } else {
                printf("Vendor ID = %x Device ID = %x\n", pciGetVendor(bus, slot), pciGetDevice(bus, slot));
                //if (/*(bus==0&&slot==0)||(bus==2&&slot==0)*/0) {
                printf("%x",pciGetConfig(bus, slot, 0, 0x0));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x4));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x8));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0xC));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x10));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x14));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x18));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x1C));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x20));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x24));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x28));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x2C));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x30));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x34));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x38));
                printf("\n");
                printf("%x",pciGetConfig(bus, slot, 0, 0x3C));
                printf("\n");
                //}
                for (char c = '\0'; ;) {
                    c = getchar();
                    if (c=='\n') {
                        clearScreen();
                        break;
                    }
                }
            }
        }
    }
    printf("+++++++++++++++++++++++END OF DUMP+++++++++++++++++++++++\n");
    return 0;
}