//
//  Ethernet.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "Ethernet.hpp"
#include <OSRuntime.hpp>
#include <OSNetwork.hpp>

OSReturn
Ethernet::send(const void* data, uint32_t length, const uint8_t MAC[6], uint16_t type, __unused uint8_t offloading) {
    if (sizeof(Packet) + length > 0x700) {
        // Packet is too long
        return kOSReturnFailed;
    }
    Packet* packet = (Packet*)OSRuntime::OSMalloc(sizeof(Packet)+length);
    
    memcpy(packet+1, data, length);
    memcpy(packet->receverMAC, MAC, 6);
    memcpy(packet->senderMAC, OSNetwork::getController()->MAC, 6);
    packet->type_len = htons(type);
    
    OSReturn ret = OSNetwork::getController()->sendPacket((void *)packet, length + sizeof(Packet));
    
    OSRuntime::OSFree(packet);
    return ret;
}
