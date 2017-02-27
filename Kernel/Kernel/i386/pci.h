//
//  pci.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/17/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef pci_h
#define pci_h

#include <stdint.h>

typedef struct {
    uint16_t VendorID;
    uint16_t DeviceID;
} PCIHeader;

uint32_t pciGetConfig(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pciGetVendor(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pciGetDevice(uint8_t bus, uint8_t slot, uint8_t func);
int pcidump();

#endif /* pci_h */
