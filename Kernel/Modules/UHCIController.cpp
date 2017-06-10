//
//  UHCIController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 1/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "UHCIController.hpp"

int UHCI::init(PCI* pci) {
    if (!(pci->ClassCode() == PCI_USB_CLASS && pci->SubClass() == PCI_USB_SUBCLASS && pci->ProgIF() == PCI_USB_UHCI)) {
        if ((pci->ClassCode() == PCI_USB_CLASS && pci->SubClass() == PCI_USB_SUBCLASS && pci->ProgIF() == PCI_USB_OHCI)) {
            printf("UHCIController: OHCI found!\n");
            printf("UHCIController: OHCI Vendor: %X Device: %X\n", pci->VendorID(), pci->DeviceID());
        }
        return -1;
    }
    
    if (pci->BAR().size != 0) { // Fix condition
        printf("UHCIController: ERROR: This UHCIController has been already initialized with different UHCI PCI Device!\n");
        return -3;
    }
    
    pci->getBAR(4);
    if (~pci->BAR().flags & 0x01) {
        printf("UHCIController: ERROR: BAR is not IO!\n");
        return -2;
    }
    
    printf("UHCIController: UHCI found!\n");
    printf("UHCIController: UHCI Vendor: %X Device: %X\n", pci->VendorID(), pci->DeviceID());
    
    Used_ = true;
    return 0;
}

void UHCI::start() {
    
}
