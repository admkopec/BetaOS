//
//  DHCP.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "DHCP.hpp"
#include "UDP.hpp"
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

static uint32_t xid = 0x00E0FF0A; // Transaction Code for Identification

void
DHCP::preparePacket(Packet* packet) {
    packet->op         = 1;
    packet->htype      = 1;         // type: for ethernet and 802.11 wireless clients, the hardware type is always 01
    packet->hlen       = 6;
    packet->hops       = 0;
    packet->xid        = xid;       // AFFExx
    packet->secs       = htons_(0);  // must use the same value in Discover and Request
    packet->flags      = BROADCAST;
    packet->ciaddr.iIP4 = 0;
    packet->yiaddr.iIP4 = 0;
    packet->siaddr.iIP4 = 0;
    packet->giaddr.iIP4 = 0;
    
    memcpy(packet->chaddr, OSNetwork::getController()->MAC, 6);
    memset(packet->chaddr + 6, 0, 10);
    memset(packet->sname, 0, 64);
    memset(packet->file, 0, 128);
    
    // options
    packet->options[0]  =  99;  // MAGIC
    packet->options[1]  = 130;  // MAGIC
    packet->options[2]  =  83;  // MAGIC
    packet->options[3]  =  99;  // MAGIC
    
    memset(packet->options + 4, 255, OPTIONSIZE - 4); // fill with end token
}

void
DHCP::Discover() {
    xid += (1 << 24);
    Packet packet;
    preparePacket(&packet);
    
    packet.options[4]  =  53;  // MESSAGE TYPE  // must option
    packet.options[5]  =   1;  // Length
    packet.options[6]  =   1;  // DISCOVER
    
    packet.options[7]  =  57;  // MAX MESSAGE SIZE
    packet.options[8]  =   2;  // Length
    packet.options[9]  =   2;  // (data) 2*256 //
    packet.options[10] =  64;  // (data)    64 // max size: 576
    
    packet.options[11] =  50;  // Requested IP
    packet.options[12] =   4;  // Length
    packet.options[13] = RIP_1;
    packet.options[14] = RIP_2;
    packet.options[15] = RIP_3;
    packet.options[16] = RIP_4;
    
    packet.options[17]  = 55;  // Parameter request list
    packet.options[18]  =  4;  // Length
    packet.options[19]  =  1;  // SUBNET MASK
    packet.options[20] =   3;  // ROUTERS
    packet.options[21] =   6;  // DOMAIN NAME SERVER
    packet.options[22] =  15;  // DOMAIN NAME

    packet.options[23] =  12;  // Hostname
    packet.options[24] =   6;  // Length
    packet.options[25] = 'B';  // B
    packet.options[26] = 'e';  // e
    packet.options[27] = 't';  // t
    packet.options[28] = 'a';  // a
    packet.options[29] = 'O';  // O
    packet.options[30] = 'S';  // S
    
    IP4_t destinationIP = {.iIP4 = 0xFFFFFFFF};
    UDP::send(&packet, sizeof(Packet), 68, 67, destinationIP);
}
