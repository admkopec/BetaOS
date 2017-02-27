//
//  XHCIController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 1/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "XHCIController.hpp"

int XHCI::init(PCI *pci) {
    if (!(pci->ClassCode() == PCI_USB_CLASS && pci->SubClass() == PCI_USB_SUBCLASS && pci->ProgIF() == PCI_USB_XHCI)) {
        return -1;
    }
    
    printf("XHCIController: XHCI found!\n");
    printf("XHCIController: Vendor: %X Device: %X\n", pci->VendorID(), pci->DeviceID());
    
    return 0;
}

void XHCI::start() {
    
}
