//
//  Ethernet.hpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef Ethernet_hpp
#define Ethernet_hpp

#include <sys/cdefs.h>
#include <OSRuntime.hpp>
#include <stdint.h>

namespace Ethernet {
    struct Packet {
        uint8_t  receverMAC[6];
        uint8_t  senderMAC[6];
        uint16_t type_len;
    } __attribute__((packed));
    
    OSReturn send(const void* data, uint32_t length, const uint8_t MAC[6], uint16_t type, uint8_t offloading);
}

#endif /* Ethernet_hpp */
