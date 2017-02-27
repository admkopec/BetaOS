//
//  PCIController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 1/11/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef PCIController_hpp
#define PCIController_hpp

#include <stdio.h>
#include <stdint.h>

struct BAR {
    union {
        void*       address;
        uint16_t    port;
    } u;
    uint64_t size = 0;
    uint16_t flags;
};

class PCI {
    int Bus;
    int Slot;
    int Function;
    uint16_t VendorID_;
    uint16_t DeviceID_;
    BAR      BAR_;
    uint8_t  HeaderType_;
    uint8_t  ClassCode_;
    uint8_t  SubClass_;
    uint8_t  ProgIF_;
    void getVendor();
    void getDevice();
    void getHeader();
public:
    uint16_t VendorID();
    uint16_t DeviceID();
    uint8_t  ClassCode();
    uint8_t  SubClass();
    uint8_t  ProgIF();
    BAR      BAR();
    int      getBAR(uint16_t index);
    bool     Valid = true;
    
    void init(int bus, int slot, int function);
};

#endif /* PCIController_hpp */
