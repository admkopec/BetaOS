//
//  UHCIController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 1/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef UHCIController_hpp
#define UHCIController_hpp

#include <stdio.h>
#include <i386/pio.h>
#include "PCIController.hpp"

#define PCI_USB_CLASS    0x0C
#define PCI_USB_SUBCLASS 0x03
#define PCI_USB_UHCI     0x00
#define PCI_USB_OHCI     0x10
#define PCI_USB_EHCI     0x20
#define PCI_USB_XHCI     0x30
#define PCI_USB_OTHER    0x80

class UHCI {
    
public:
    int  init(PCI *pci);
    void start();
};

#endif /* UHCIController_hpp */
