//
//  IPs.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "IPs.hpp"
#include "Ethernet.hpp"
#include <Modules/NetworkController.hpp>
#include <OSRuntime.hpp>
#include <OSNetwork.hpp>

// Net defs

// hton = Host To Network
static inline short htons_(short v) {
    __asm__("xchg %h0, %b0" : "+Q"(v));
    return (v);
}
// ntoh = Network To Host
#define ntohs_(v) htons(v)
#define ntohl_(v) htonl(v)

bool
IP::sameSubnet(IP4_t IP1, IP4_t IP2, IP4_t subnet) {
    return ((IP1.iIP4 & subnet.iIP4) == (IP2.iIP4 & subnet.iIP4));
}

OSReturn
IP::sendPacket(const Packetv4* packet, uint32_t length, __unused IP4_t IP, uint8_t features) {
    
    const uint8_t destMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    return Ethernet::send(packet, length + sizeof(Packetv4), destMAC, 0x0800, features);
}

void
IP::send(const void* data, uint32_t length, IP4_t IP, uint8_t protocol, uint8_t offloading) {
    Packetv4* packet = (Packetv4*)OSRuntime::OSMalloc(sizeof(Packetv4)+length);
    memcpy(packet+1, data, length);
    
    packet->destIP.iIP4    = IP.iIP4;
    packet->sourceIP.iIP4  = OSNetwork::getController()->IP.iIP4;
    packet->version        = 4;
    packet->ipHeaderLength = sizeof(Packetv4) / 4;
    packet->typeOfService  = 0;
    packet->length         = htons_(sizeof(Packetv4) + length);
    packet->identification = 0;
    packet->fragmentation  = htons_(0x4000); // do not fragment
    packet->ttl            = 128;
    packet->protocol       = protocol;
    packet->checksum       = 0;
    
    if (IP.iIP4 == 0 || IP.iIP4 == 0xFFFFFFFF || sameSubnet(IP, OSNetwork::getController()->IP, OSNetwork::getController()->Subnet)) {
        // IP on LAN
        if (sendPacket(packet, length, IP, offloading) != kOSReturnSuccess) {
            if (sendPacket(packet, length, OSNetwork::getController()->Gateway_IP, offloading) != kOSReturnSuccess) {
                // Failed
            }
        }
    } else {
        // Send to Server
        if (sendPacket(packet, length, OSNetwork::getController()->Gateway_IP, offloading) != kOSReturnSuccess) {
            // Failed
        }
    }
    OSRuntime::OSFree(packet);
}
