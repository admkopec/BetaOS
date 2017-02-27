//
//  ModulesController.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "ModulesController.hpp"
#include "IntelE1000Controller.hpp"
#include "RTL8111Controller.hpp"
#include "UHCIController.hpp"
#include "EHCIController.hpp"
#include "XHCIController.hpp"
#include "MMIOUtils.hpp"
#include "SATAController.hpp"
#include "PCIController.hpp"

bool ehc1_reserved;     // Quick Fix for doubled EHCI Devices on Intel Chipsets

UHCI*    uhci;
EHCI*    ehc1;
EHCI*    ehc2;
XHCI*    xhci;
SATA*    sata;
E1000*   e1000;
RTL8111* rtl8111;
Modules* ModulesController;

void ModulesStartController() {
    ModulesController->start();
}
// Try speeding up and not using "brute force"
void Modules::start() {
    for (int bus = 0; bus < 256; ++bus) {
        for (int slot = 0; slot < 32; ++slot) {
            for (int function = 0; function < 8; ++function) {
                PCI h;
                h.init(bus, slot, function);
                if (h.Valid) {
                    if (!uhci->init(&h)) {
                        uhci->start();
                        continue;
                    }
                    if (!ehc1_reserved) {
                        if (!ehc1->init(&h)) {
                            ehc1_reserved = true;
                            ehc1->start();
                            continue;
                        }
                    } else {
                        if (!ehc2->init(&h)) {
                            ehc2->start();
                            continue;
                        }
                    }
                    if (!xhci->init(&h)) {
                        xhci->start();
                        continue;
                    }
                    if (!sata->init(&h)) {
                        sata->start();
                        continue;
                    }
                    if (!e1000->init(&h)) {
                        e1000->start();
                        continue;
                    }
                    if (!rtl8111->init(&h)) {
                        //    rtl8111->start();
                        continue;
                    }
                }
            }
        }
    }
}
