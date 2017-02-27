//
//  SATAController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 2/25/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "SATAController.hpp"

int SATA::init(PCI *header) {
    if (!(header->ClassCode() == 0x01 && header->SubClass() == 0x06)) {
        return -1;
    }
    printf("SATAController: SATA found!\n");
    printf("SATAController: Vendor ID: %X, Device ID: %X\n", header->VendorID(), header->DeviceID());
    if (header->ProgIF() == 0x01) {
        printf("SATAController: AHCI found!\n");
    } else {
        printf("SATAController: Unknown Prog IF: %X\n", header->ProgIF());
        return -2;
    }
    
    return 0;
}

void SATA::start() {
    
}
