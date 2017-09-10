//
//  IPs.hpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef IPs_hpp
#define IPs_hpp

#include <sys/cdefs.h>
#include <OSRuntime.hpp>

// IP templates

// own IP at start
#define IP_1    192
#define IP_2    168
#define IP_3      1
#define IP_4     22

// requested IP
#define RIP_1   192
#define RIP_2   168
#define RIP_3     1
#define RIP_4    22

// gateway IP for routing to the internet
#define GW_IP_1   192
#define GW_IP_2   168
#define GW_IP_3     1
#define GW_IP_4     1

// DNS IP for resolving name to IP
#define DNS_IP_1   208
#define DNS_IP_2    67
#define DNS_IP_3   222
#define DNS_IP_4   222

struct IP4_t {
    uint8_t IP4[4];
    uint32_t iIP4;
} __packed;

struct IP6_t {
    uint16_t IP6[8];
    uint64_t iIP6[2]; // prefix and interface ID
} __packed;

namespace IP {
    struct Packetv4 {
        uint8_t  ipHeaderLength   : 4;
        uint8_t  version          : 4;
        uint8_t  typeOfService;
        uint16_t length;
        uint16_t identification;
        uint16_t fragmentation;
        uint8_t  ttl;
        uint8_t  protocol;
        uint16_t checksum;
        IP4_t     sourceIP;
        IP4_t     destIP;
    } __packed;
    bool     sameSubnet(IP4_t IP1, IP4_t IP2, IP4_t subent);
    void     send(const void* data, uint32_t length, IP4_t IP, uint8_t protocol, uint8_t offloading);
    OSReturn sendPacket(const Packetv4* packet, uint32_t length, IP4_t IP, uint8_t features);
}

#endif /* IPs_hpp */
