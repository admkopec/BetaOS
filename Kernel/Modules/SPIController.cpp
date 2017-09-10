//
//  SPIController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 8/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "SPIController.hpp"

#define super Controller
#define Log(x ...) printf("SPIController: " x)
#ifdef DEBUG
#define DBG(x ...) Log(x)
#else
#define DBG(x ...)
#endif
// It's a test module for Keyboard and touchpad on MacBook12,1 and MacBookPro13,1 and newer
OSReturn
SPI::init(PCI *header) {
    for (size_t i = 0; i < sizeof(VendorIDs); i++) {
        if (header->VendorID() == VendorIDs[i]) {
            break;
        } else if (i == (sizeof(VendorIDs) - 1)) {
            return kOSReturnFailed;
        }
    }
    for (size_t i = 0; i < sizeof(DeviceIDs); i++) {
        if (header->DeviceID() == DeviceIDs[i]) {
            break;
        } else if (i == (sizeof(DeviceIDs) - 1)) {
            return kOSReturnFailed;
        }
    }
    
    DBG("Found SPI Bus (VendorID: %X, DeviceID: %X)\n", header->VendorID(), header->DeviceID());
    
    NameString = (char*)"SPIController (Beta)";
    Used_ = true;
    return kOSReturnSuccess;
}

void
SPI::start() {
    super::start();
}

const int
SPI::VendorIDs[] = { 0x8086 };

const int
SPI::DeviceIDs[] = { 0x7270, 0xA129, 0xA12A };
