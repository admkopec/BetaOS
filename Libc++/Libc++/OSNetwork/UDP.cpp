//
//  UDP.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <OSRuntime.hpp>
#include <string.h>
#include "UDP.hpp"

// Net defs

// hton = Host To Network
static inline short htons_(short v) {
    __asm__("xchg %h0, %b0" : "+Q"(v));
    return (v);
}
// ntoh = Network To Host
#define ntohs_(v) htons(v)
#define ntohl_(v) htonl(v)

void
UDP::send(const void* data, uint32_t length, uint16_t sourcePort, uint16_t destinationPort, IP4_t destinationIP) {
    Packet* packet = (Packet*)OSRuntime::OSMalloc(sizeof(Packet)+length);
    memcpy(packet+1, data, length);
    
    packet->sourcePort = htons_(sourcePort);
    packet->destinationPort = htons_(destinationPort);
    packet->length = htons_(length + sizeof(Packet));
    packet->checksum = 0;
    IP::send((void*)packet, sizeof(Packet)+length, destinationIP, 17, 0);
    OSRuntime::OSFree(packet);
}
