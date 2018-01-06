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
        uint64_t    address;
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
    uint8_t  IntLine_;
    void getVendor();
    void getDevice();
    void getHeader();
    bool ReadExtendedCapabilities(uint8_t ID, uint8_t* Value);
public:
    uint16_t VendorID();
    uint16_t DeviceID();
    uint8_t  ClassCode();
    uint8_t  SubClass();
    uint8_t  ProgIF();
    uint8_t  IntLine();
    BAR      BAR();
    void     EnableBusMastering();
    bool     TrySettingMSIVector(int IRQ);
    int      getBAR (uint16_t index);
    uint32_t Read32 (uint8_t offset);
    void     Write32(uint8_t offset, uint32_t data);
    void     Write16(uint8_t offset, uint16_t data);
    void     Write8 (uint8_t offset, uint8_t  data);
    bool     Valid = true;
    
    void init(int bus = 0, int slot = 0, int function = 0);
};

#endif /* PCIController_hpp */
