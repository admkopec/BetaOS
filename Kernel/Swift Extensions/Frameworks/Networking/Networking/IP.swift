//
//  IP.swift
//  Networking
//
//  Created by Adam Kopeć on 3/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

private func sameSubnet(IP1: IPv4, IP2: IPv4, Subnet: IPv4) -> Bool {
    return ((IP1.iIP4 & Subnet.iIP4) == (IP2.iIP4 & Subnet.iIP4))
}

public struct IPProtocol {
    let networkDevice: NetworkModule
    public init(device: NetworkModule) {
        networkDevice = device
    }
    
    func sendPacket(packet: UnsafePointer<IPv4Packet>, length: Int, IP: IPv4, features: UInt8) -> Bool {
        let destinationMAC: [UInt8] = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
        return networkDevice.send(data: packet, length: length + MemoryLayout<IPv4Packet>.size, MAC: destinationMAC, type: 0x0800, offloading: features)
    }
    
    func send(data: UnsafeRawPointer, length: Int, IP: IPv4, proto: UInt8, offloading: UInt8) {
        let packet = UnsafeMutablePointer<IPv4Packet>(OpaquePointer(malloc(MemoryLayout<IPv4Packet>.size + length)))!
        memcpy(packet.advanced(by: 1), data, length)
        
        packet.pointee.destIP.iIP4 = IP.iIP4
        packet.pointee.sourceIP.iIP4 = networkDevice.IP.iIP4
        packet.pointee.version        = 4
        packet.pointee.ipHeaderLength = UInt8(MemoryLayout<IPv4Packet>.size / 4)
        packet.pointee.typeOfService  = 0
        packet.pointee.length         = UInt16(htons_(Int16(MemoryLayout<IPv4Packet>.size + length)))
        packet.pointee.identification = 0
        packet.pointee.fragmentation  = UInt16(htons_(0x4000)) // do not fragment
        packet.pointee.ttl            = 128
        packet.pointee.protocol       = proto
        packet.pointee.checksum       = 0
        
        if (IP.iIP4 == 0 || IP.iIP4 == 0xFFFFFFFF || sameSubnet(IP1: IP, IP2: networkDevice.IP, Subnet: networkDevice.Subnet)) {
            // IP on LAN
            if (sendPacket(packet: packet, length: length, IP: IP, features: offloading) != true) {
                if (sendPacket(packet: packet, length: length, IP: networkDevice.GatewayIP, features: offloading) != true) {
                    // Failed
                }
            }
        } else {
            // Send to Server
            if (sendPacket(packet: packet, length: length, IP: networkDevice.GatewayIP, features: offloading) != true) {
                // Failed
            }
        }
        UnsafeRawPointer(packet).deallocate()
    }
}
