//
//  RSDP.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//import Addressing
import Loggable

struct RSDP: Loggable, CustomStringConvertible {
    let Name = "RSDP"
    fileprivate(set) var Signature:          String = ""
    fileprivate(set) var Checksum:           UInt8  = 0xFF
    fileprivate(set) var OEMID:              String = ""
    fileprivate(set) var Revision:           UInt8  = 0xFF
    fileprivate(set) var RSDTAddr:           UInt32 = 0
    fileprivate(set) var isExtended:         Bool   = false
    fileprivate(set) var Length:             UInt32? = nil
    fileprivate(set) var XSDTAddr:           UInt64? = nil
    fileprivate(set) var ExtendedChecksum:   UInt8?  = nil
    fileprivate var baseAddr: (UInt, UInt) = (0x0, 0x0)
    var RSDT: Address {
        if XSDTAddr != nil {
            return Address(XSDTAddr!, baseAddress: baseAddr)
        } else {
            return Address(RSDTAddr, baseAddress: baseAddr)
        }
    }
    
    fileprivate(set) var OriginalAddress: Address = 0
    fileprivate(set) var MappedAddress:   Address = 0
    
    var description: String {
        return "RSDP: \(Signature): \(OEMID): rev: \(Revision) ptr: \(RSDT)"
    }
    
    init?(structure: RSDP_for_Swift) {
        if !structure.foundInBios {
            OriginalAddress = Address(structure.OriginalAddress)
            if (OriginalAddress == 0) || (OriginalAddress == UInt.max) {
                Log("Original Address is not valid", level: .Error)
                return nil
            }
            MappedAddress   = Address(UInt(bitPattern: structure.RSDP))
            baseAddr = (OriginalAddress.physical, MappedAddress.physical)
        }
        Signature       = String(&structure.RSDP.pointee.Signature.0, maxLength: 8)
        if Signature != "RSD PTR " {
            Log("Original Address: \(OriginalAddress), MappedAddress: \(MappedAddress)", level: .Error)
            Log("Signature: \(Signature)", level: .Error)
            return nil
        }
        Checksum  = structure.RSDP.pointee.Checksum
        OEMID     = String(&structure.RSDP.pointee.OemId.0, maxLength: 6)
        Revision  = structure.RSDP.pointee.Revision
        RSDTAddr  = structure.RSDP.pointee.RsdtAddress
        if RSDTAddr == UInt32.max || RSDTAddr == 0 {
            Log("RSDT Address is not valid", level: .Error)
            Log(description, level: .Debug)
            return nil
        }
        if Revision >= 2 {
            isExtended       = true
            Length           = structure.RSDP.pointee.Length
            XSDTAddr         = structure.RSDP.pointee.XsdtAddress
            ExtendedChecksum = structure.RSDP.pointee.ExtendedChecksum
            if XSDTAddr == UInt64.max || XSDTAddr == 0 {
                Log("XSDT Address is not valid", level: .Error)
                Log(description, level: .Debug)
                return nil
            }
        } else {
            isExtended       = false
            Length           = nil
            XSDTAddr         = nil
            ExtendedChecksum = nil
        }
    }
}
