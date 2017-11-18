//
//  RSDT.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//import Addressing
import Loggable

struct RSDT: Loggable, ACPITable {
    let Name = "RSDT"
    let Header: SDTHeader
    fileprivate(set) var EntryCount = 0
    fileprivate(set) var SDTAddresses = [(physical: UInt, baseAddr: (UInt, UInt))]()
//    fileprivate(set) var SDTAddresses = [Address]()
    var description: String {
        return "RSDT: \(Header), Number of Entries: \(EntryCount)"
    }
    
    init(address: Address) {
        let ptr = UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: address.virtual)!
        Header = SDTHeader(ptr: ptr)
        if ((Header.Signature != "RSDT") && (Header.Signature != "XSDT")) {
            Log("This is an incorrect address, it's pointing to \(Header.Signature) instead of RSDT or XSDT", level: .Error)
            return
        }
        EntryCount = Int(Header.Length) - MemoryLayout<ACPISDTHeader_t>.stride / ((Header.Signature == "XSDT") ? MemoryLayout<UInt64>.size : MemoryLayout<UInt32>.size)
        if EntryCount > 10 {
            EntryCount = 10
        }
        if Header.Signature == "XSDT" {
            let addrptr: UnsafePointer<UInt64> = UnsafePointer<UInt64>(bitPattern: UInt(bitPattern: ptr.advanced(by: 1)))!
            let entryptr = UnsafeBufferPointer<UInt64>(start: addrptr, count: EntryCount)
            
            var addr = Address(address.physical, baseAddress: address.baseAddr)
            for entry in entryptr {
                addr = Address(entry, baseAddress: addr.baseAddr)
                let sdtptr = UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: addr.virtual)
                if sdtptr != nil {
                    //                SDTAddresses.append(addr)
                    SDTAddresses.append((addr.physical, addr.baseAddr))
                    let signature = String(&(sdtptr!).pointee.Signature.0, maxLength: 4)
                    if signature == "WSMT" {
                        break
                    }
                }
            }
        } else {
            let addrptr: UnsafePointer<UInt32> = UnsafePointer<UInt32>(bitPattern: UInt(bitPattern: ptr.advanced(by: 1)))!
            let entryptr = UnsafeBufferPointer<UInt32>(start: addrptr, count: EntryCount)
            
            var addr = Address(address.physical, baseAddress: address.baseAddr)
            for entry in entryptr {
                addr = Address(entry, baseAddress: addr.baseAddr)
                let sdtptr = UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: addr.virtual)
                if sdtptr != nil {
                    //                SDTAddresses.append(addr)
                    SDTAddresses.append((addr.physical, addr.baseAddr))
                    let signature = String(&(sdtptr!).pointee.Signature.0, maxLength: 4)
                    if signature == "WSMT" {
                        break
                    }
                }
            }
        }
    }
}
