//
//  DHCP.swift
//  Networking
//
//  Created by Adam Kopeć on 3/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

public struct DHCP {
    let networkDevice: NetworkModule
//    static var xid = 0x00E0FF0A as UInt32 // Transaction Code for Identification
    public init(device: NetworkModule) {
        networkDevice = device
    }

    public func Discover() {
//        DHCP.xid += (1 << 24)
        var packet = DHCPPacket()
        preparePacket(packet: &packet)
        
        packet.options.4  =  53
        packet.options.5  =   1  // Length
        packet.options.6  =   1  // DISCOVER
        
        packet.options.7  =  57  // MAX MESSAGE SIZE
        packet.options.8  =   2  // Length
        packet.options.9  =   2  // (data) 2*256 //
        packet.options.10 =  64  // (data)    64 // max size: 576
        
        packet.options.11 =  50  // Requested IP
        packet.options.12 =   4  // Length
        packet.options.13 = UInt8(RIP_1)
        packet.options.14 = UInt8(RIP_2)
        packet.options.15 = UInt8(RIP_3)
        packet.options.16 = UInt8(RIP_4)
        
        packet.options.17  = 55  // Parameter request list
        packet.options.18  =  4  // Length
        packet.options.19  =  1  // SUBNET MASK
        packet.options.20 =   3  // ROUTERS
        packet.options.21 =   6  // DOMAIN NAME SERVER
        packet.options.22 =  15  // DOMAIN NAME
        
        packet.options.23 =  12  // Hostname
        packet.options.24 =   6  // Length
        packet.options.25 = UInt8(UnicodeScalar("B")!.value)  // B
        packet.options.26 = UInt8(UnicodeScalar("e")!.value)  // e
        packet.options.27 = UInt8(UnicodeScalar("t")!.value)  // t
        packet.options.28 = UInt8(UnicodeScalar("a")!.value)  // a
        packet.options.29 = UInt8(UnicodeScalar("O")!.value)  // O
        packet.options.30 = UInt8(UnicodeScalar("S")!.value)  // S
        
        let destinationIP = IPv4(IP4: (0, 0, 0, 0), iIP4: 0xFFFFFFFF)
        networkDevice.udp.send(data: &packet.op, length: MemoryLayout<DHCPPacket>.size, sourcePort: 68, destinationPort: 67, destinationIP: destinationIP)
    }
    
    private func preparePacket(packet: inout DHCPPacket) {
        packet.op       = 1
        packet.htype    = 1  // For Ethernet and 802.11 Wireless Clients, the hardware type is always 01
        packet.hlen     = 6
        packet.hops     = 0
        packet.xid      = 0x00E0FF0A//DHCP.xid
        packet.secs     = UInt16(htons_(0))
        packet.flags    = UInt16(BROADCAST)
        packet.ciaddr.iIP4 = 0
        packet.yiaddr.iIP4 = 0
        packet.siaddr.iIP4 = 0
        packet.giaddr.iIP4 = 0
        
        guard networkDevice.MAC.count >= 6 else {
            return
        }
        
        packet.chaddr.0  = networkDevice.MAC[0]
        packet.chaddr.1  = networkDevice.MAC[1]
        packet.chaddr.2  = networkDevice.MAC[2]
        packet.chaddr.3  = networkDevice.MAC[3]
        packet.chaddr.4  = networkDevice.MAC[4]
        packet.chaddr.5  = networkDevice.MAC[5]
        memset(&packet.chaddr.6, 0, 10)
        memset(&packet.sname.0, 0, 64)
        memset(&packet.file.0, 0, 128)
        
        packet.options.0 = 99   // MAGIC
        packet.options.1 = 130  // MAGIC
        packet.options.2 = 83   // MAGIC
        packet.options.3 = 99   // MAGIC
        
        memset(&packet.options.4, 255, Int(OPTIONSIZE) - 4)
    }
}
