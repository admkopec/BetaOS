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
#include "NetworkController.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    void testEEPRom();
#ifdef __cplusplus
}

class RTL8111 : public NetworkController {
    uint8_t     bar_type;       // Type of BOR0
    uint16_t    io_base;        // IO Base Address
    uint64_t    mem_base;       // MMIO Base Address
    bool        eeprom_exists;  // A flag indicating if eeprom exists
    uint8_t     MAC[6];         // A buffer for storing the mack address
    
    static const int VendorIDs[1];
    static const int DeviceIDs[1];
    
    // Send Commands and read results From NICs either using MMIO or IO Ports
    void     writeCommand(uint16_t p_address, uint32_t p_value);
    uint32_t readCommand(uint16_t p_address);
    
    bool        detectEEProm();             // Return true if EEProm exist, else it returns false and set the eerprom_existsdata member // NOT working!
    uint32_t    eepromRead(uint8_t addr);   // Read 4 bytes from a specific EEProm Address
    bool        readMACAddress();           // Read MAC Address
public:
    virtual int  init(PCI *h) override;
    virtual void start() override;
    virtual OSReturn sendPacket(const void* data, uint16_t length) override;
};

#endif

#endif /* RTL8111Controller_hpp */
