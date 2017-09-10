//
//  UDP.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <OSRuntime.hpp>
#include "UDP.hpp"

void
UDP::send(const void* data, uint32_t length, uint16_t sourcePort, uint16_t destinationPort, IP4_t destinationIP) {
    Packet* packet = (Packet*)OSRuntime::OSMalloc(sizeof(Packet)+length);
    memcpy(packet+1, data, length);
    
    packet->sourcePort = htons(sourcePort);
    packet->destinationPort = htons(destinationPort);
    packet->length = htons(length + sizeof(Packet));
    packet->checksum = 0;
    IP::send((void*)packet, sizeof(Packet)+length, destinationIP, 17, 0);
    OSRuntime::OSFree(packet);
}
