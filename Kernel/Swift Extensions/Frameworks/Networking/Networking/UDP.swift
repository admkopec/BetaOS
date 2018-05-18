//
//  UDP.swift
//  Networking
//
//  Created by Adam Kopeć on 3/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

public struct UDP {
    let networkDevice: NetworkModule
    public init(device: NetworkModule) {
        networkDevice = device
    }
    
    public func send(data: UnsafeRawPointer, length: Int, sourcePort: Int, destinationPort: Int, destinationIP: IPv4) {
        let packet = UnsafeMutablePointer<UDPPacket>(OpaquePointer(malloc(MemoryLayout<UDPPacket>.size + length)))!
        memcpy(packet.advanced(by: 1), data, length)
        
        packet.pointee.sourcePort = UInt16(htons_(Int16(sourcePort)))
        packet.pointee.destinationPort = UInt16(htons_(Int16(destinationPort)))
        packet.pointee.length = UInt16(htons_(Int16(MemoryLayout<UDPPacket>.size + length)))
        packet.pointee.checksum = 0
        
        networkDevice.ip.send(data: packet, length: length + MemoryLayout<UDPPacket>.size, IP: destinationIP, proto: 17, offloading: 0)
        UnsafeRawPointer(packet).deallocate()
    }
}
