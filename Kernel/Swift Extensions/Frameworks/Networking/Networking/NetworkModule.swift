//
//  NetworkModule.swift
//  Networking
//
//  Created by Adam Kopeć on 3/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

public protocol NetworkModule: AnyObject {
    var dhcp: DHCP!  { get }
    var udp: UDP!    { get }
    var ip: IPProtocol! { get }
    var MAC: [UInt8] { get }
    var IP: IPv4 { get set }
    var Subnet: IPv4 { get set }
    var GatewayIP: IPv4 { get set }
    func sendPacket(data: UnsafeRawPointer, size: Int)
}

public extension NetworkModule {
    func send(data: UnsafeRawPointer, length: Int, MAC: [UInt8], type: Int, offloading: UInt8) -> Bool {
        if MemoryLayout<EthernetPacket>.size + length > 0x700 {
            return false
        }
        let packet = UnsafeMutablePointer<EthernetPacket>(OpaquePointer(malloc(MemoryLayout<EthernetPacket>.size + length)))!
        memcpy(packet.advanced(by: 1), data, length)
        guard MAC.count >= 6 && self.MAC.count >= 6 else {
            return false
        }
        packet.pointee.receverMAC.0 = MAC[0]
        packet.pointee.receverMAC.1 = MAC[1]
        packet.pointee.receverMAC.2 = MAC[2]
        packet.pointee.receverMAC.3 = MAC[3]
        packet.pointee.receverMAC.4 = MAC[4]
        packet.pointee.receverMAC.5 = MAC[5]
        
        packet.pointee.senderMAC.0 = self.MAC[0]
        packet.pointee.senderMAC.1 = self.MAC[1]
        packet.pointee.senderMAC.2 = self.MAC[2]
        packet.pointee.senderMAC.3 = self.MAC[3]
        packet.pointee.senderMAC.4 = self.MAC[4]
        packet.pointee.senderMAC.5 = self.MAC[5]
        
        packet.pointee.type_len = UInt16(htons_(Int16(type)))
        
        sendPacket(data: packet, size: MemoryLayout<EthernetPacket>.size + length)
        UnsafeRawPointer(packet).deallocate()
        return true
    }
}
