//
//  Networking.h
//  Networking
//
//  Created by Adam Kopeć on 3/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

#include <stdint.h>

// hton = Host To Network
static inline short htons_(short v) {
    __asm__("xchg %h0, %b0" : "+Q"(v));
    return (v);
}
// ntoh = Network To Host
#define ntohs_(v) htons(v)
#define ntohl_(v) htonl(v)

#define UNICAST      0
#define BROADCAST  128 // 1....... ........
#define OPTIONSIZE 340 // results in size of 576

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

struct IPv4 {
    uint8_t IP4[4];
    uint32_t iIP4;
} __attribute__((packed));

struct IP6v6 {
    uint16_t IP6[8];
    uint64_t iIP6[2]; // prefix and interface ID
} __attribute__((packed));

struct IPv4Packet {
    uint8_t  ipHeaderLength   : 4;
    uint8_t  version          : 4;
    uint8_t  typeOfService;
    uint16_t length;
    uint16_t identification;
    uint16_t fragmentation;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    struct IPv4     sourceIP;
    struct IPv4     destIP;
} __attribute__((packed));

struct DHCPPacket {        // complete length: 576 (0x0240)
    uint8_t  op;        // DHCP_BOOTREQEUST or DHCP_BOOTREPLY
    uint8_t  htype;     // DHCP_HTYPE10MB
    uint8_t  hlen;      // DHCP_HLENETHERNET
    uint8_t  hops;      // DHCP_HOPS
    uint32_t xid;       // DHCP_XID
    uint16_t secs;      // DHCP_SECS
    uint16_t flags;     // DHCP_FLAGSBROADCAST
    struct IPv4    ciaddr;
    struct IPv4    yiaddr;
    struct IPv4    siaddr;
    struct IPv4    giaddr;
    uint8_t  chaddr[16];
    char     sname[64];
    char     file[128];
    uint8_t  options[OPTIONSIZE];
} __attribute__((packed));

struct ARPPacket {
    uint16_t hardware_addresstype;
    uint16_t protocol_addresstype;
    uint8_t  hardware_addresssize;
    uint8_t  protocol_addresssize;
    uint16_t operation;
    uint8_t  source_mac[6];
    struct IPv4    sourceIP;
    uint8_t  dest_mac[6];
    struct IPv4    destIP;
} __attribute__((packed));

struct UDPPacket {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed));

struct EthernetPacket {
    uint8_t  receverMAC[6];
    uint8_t  senderMAC[6];
    uint16_t type_len;
} __attribute__((packed));
