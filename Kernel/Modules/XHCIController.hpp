//
//  XHCIController.h
//  Kernel
//
//  Created by Adam Kopeć on 1/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef XHCIController_hpp
#define XHCIController_hpp

#include <stdio.h>
#include "PCIController.hpp"
#include "MMIOUtils.hpp"

#define PCI_USB_CLASS    0x0C
#define PCI_USB_SUBCLASS 0x03
#define PCI_USB_XHCI     0x30

class XHCI {
    
public:
    int  init(PCI *pci);
    void start();
};

#endif /* XHCIController_hpp */
