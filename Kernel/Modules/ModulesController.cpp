//
//  ModulesController.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "ModulesController.hpp"
#include "Controller.hpp"
#include "IntelE1000Controller.hpp"
#include "RTL8111Controller.hpp"
#include "UHCIController.hpp"
#include "EHCIController.hpp"
#include "XHCIController.hpp"
#include "MMIOUtils.hpp"
#include "SATAController.hpp"
#include "PCIController.hpp"

#define Log(x ...) printf("ModulesController: " x)

Modules ModulesController;

extern "C" { void * kalloc_(uint32_t size); void free_(void * data, uint32_t size); }

void ModulesStartController() {
    ModulesController.start();
}

void ModulesStopController() {
    ModulesController.stop();
}
// Try speeding up and not using "brute force"
void Modules::start() {
    for (int bus = 0; bus < 256; bus++) {
        PCI t;
        t.init(bus);
        if (!t.Valid) {
            continue;
        }
        for (int slot = 0; slot < 32; slot++) {
            for (int function = 0; function < 8; function++) {
                PCI h;
                h.init(bus, slot, function);
                if (h.Valid) {
                    for (int i = LastLoadedModule; i < MAX_LOADED_MODULES; i++) {
                        if (Controllers[i] != NULL) {
                            if (Controllers[i]->Used() == true) {
                                continue;
                            }
                        }
                        Controller* module = new XHCI;
                        //Log("Module at start: %X ", module);
                        if (!module->init(&h)) {
                            Controllers[i] = module;
                            Controllers[i]->start();
                            LastLoadedModule = i;
                            break;
                        } else {
                            delete module;
                        }
                        module = new EHCI;
                        if (!module->init(&h)) {
                            Controllers[i] = module;
                            Controllers[i]->start();
                            LastLoadedModule = i;
                            break;
                        } else {
                            delete module;
                        }
                        module = new SATA;
                        if (!module->init(&h)) {
                            Controllers[i] = module;
                            Controllers[i]->start();
                            LastLoadedModule = i;
                            break;
                        } else {
                            delete module;
                        }
                        module = new E1000;
                        if (!module->init(&h)) {
                            Controllers[i] = module;
                            Controllers[i]->start();
                            LastLoadedModule = i;
                            break;
                        } else {
                            delete module;
                        }
                        module = new RTL8111;
                        if (!module->init(&h)) {
                            Controllers[i] = module;
                            Controllers[i]->start();
                            LastLoadedModule = i;
                            break;
                        } else {
                            delete module;
                        }
                        //Log("Module at end: %X\n", module);
                        break;
                    }
                }
            }
        }
    }
}

void Modules::stop() {
    for (uint32_t i = 0; i <= LastLoadedModule; i++) {
        Controllers[i]->stop();
    }
    // Add stop code
    
    delete this;
}
