//
//  RTL8111Controller.hpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef RTL8111Controller_hpp
#define RTL8111Controller_hpp

#include <stdio.h>
#include <stdint.h>
#include "Controller.hpp"

#define Realtek_Vendor 0x10EC
#define RTL8168_ID     0x8168

#ifdef __cplusplus
extern "C" {
#endif
    void testEEPRom();
#ifdef __cplusplus
}

class RTL8111 : public Controller {
    uint8_t     bar_type;       // Type of BOR0
    uint16_t    io_base;        // IO Base Address
    uint64_t    mem_base;       // MMIO Base Address
    bool        eeprom_exists;  // A flag indicating if eeprom exists
    uint8_t     mac[6];         // A buffer for storing the mack address
    
    // Send Commands and read results From NICs either using MMIO or IO Ports
    void     writeCommand(uint16_t p_address, uint32_t p_value);
    uint32_t readCommand(uint16_t p_address);
    
    bool        detectEEProm();             // Return true if EEProm exist, else it returns false and set the eerprom_existsdata member // NOT working!
    uint32_t    eepromRead(uint8_t addr);   // Read 4 bytes from a specific EEProm Address
    bool        readMACAddress();           // Read MAC Address
public:
    virtual int  init(PCI *h) override;
    virtual void start() override;
};

#endif

#endif /* RTL8111Controller_hpp */
