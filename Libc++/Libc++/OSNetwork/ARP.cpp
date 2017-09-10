//
//  ARP.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/20/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "ARP.hpp"
#include "Ethernet.hpp"
#include <OSNetwork.hpp>
#include <OSRuntime.hpp>

void
ARP::SendRequest(IP4_t searchedIP) {
    Packet request;
    
    request.operation = htons(1);
    request.hardware_addresstype = htons(1);
    request.protocol_addresstype = htons(0x0800);
    request.hardware_addresssize = 6;
    request.protocol_addresssize = 4;
    
    for (uint8_t i = 0; i < 6; i++) {
        request.dest_mac[i]   = 0x00;
        request.source_mac[i] = OSNetwork::getController()->MAC[i];
    }
    
    request.destIP.iIP4   = searchedIP.iIP4;
    request.sourceIP.iIP4 = OSNetwork::getController()->IP.iIP4;
    
    const uint8_t destMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    Ethernet::send((void *)&request, sizeof(Packet), destMAC, 0x0806, 0);
}
