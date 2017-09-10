//
//  DHCP.hpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef DHCP_hpp
#define DHCP_hpp

#include "IPs.hpp"
#include <sys/cdefs.h>

#define UNICAST      0
#define BROADCAST  128 // 1....... ........
#define OPTIONSIZE 340 // results in size of 576

typedef enum { DHCP_START, DHCP_OFFER, DHCP_ACK, DHCP_NAK } DHCP_state;

namespace DHCP {
    struct Packet {        // complete length: 576 (0x0240)
        uint8_t  op;        // DHCP_BOOTREQEUST or DHCP_BOOTREPLY
        uint8_t  htype;     // DHCP_HTYPE10MB
        uint8_t  hlen;      // DHCP_HLENETHERNET
        uint8_t  hops;      // DHCP_HOPS
        uint32_t xid;       // DHCP_XID
        uint16_t secs;      // DHCP_SECS
        uint16_t flags;     // DHCP_FLAGSBROADCAST
        IP4_t    ciaddr;
        IP4_t    yiaddr;
        IP4_t    siaddr;
        IP4_t    giaddr;
        uint8_t  chaddr[16];
        char     sname[64];
        char     file[128];
        uint8_t  options[OPTIONSIZE];
    } __packed;
    
    void Discover();
    void Request(IP4_t requestedIP);
    void Inform();
    void Release();
    void AnalyzeMsg(const Packet* dhcp, IP4_t sourceIP);
    void preparePacket(Packet* packet);
}

#endif /* DHCP_hpp */
