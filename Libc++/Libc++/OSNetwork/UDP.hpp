//
//  UDP.hpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef UDP_hpp
#define UDP_hpp

#include <sys/cdefs.h>
#include "IPs.hpp"

namespace UDP {
    struct Packet {
        uint16_t sourcePort;
        uint16_t destinationPort;
        uint16_t length;
        uint16_t checksum;
    } __attribute__((packed));
    void send(const void* data, uint32_t length, uint16_t sourcePort, uint16_t destinationPort, IP4_t destinationIP);
}

#endif /* UDP_hpp */
