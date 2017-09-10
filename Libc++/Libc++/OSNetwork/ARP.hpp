//
//  ARP.hpp
//  Libc++
//
//  Created by Adam Kopeć on 8/20/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef ARP_hpp
#define ARP_hpp

#include <sys/cdefs.h>
#include "IPs.hpp"

namespace ARP {
    struct TableEntry {
        uint32_t seconds;
        uint8_t  MAC[6];
        IP4_t    IP;
        bool     dynamic;
    };
    struct Packet {
        uint16_t hardware_addresstype;
        uint16_t protocol_addresstype;
        uint8_t  hardware_addresssize;
        uint8_t  protocol_addresssize;
        uint16_t operation;
        uint8_t  source_mac[6];
        IP4_t    sourceIP;
        uint8_t  dest_mac[6];
        IP4_t    destIP;
    } __packed;
    
    TableEntry* FindEntry(IP4_t IP);
    void SendRequest(IP4_t searchedIP);
    void WaitForReply(IP4_t searchedIP);
}

#endif /* ARP_hpp */
