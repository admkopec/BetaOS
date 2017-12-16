//
//  HPET.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct HPET: Loggable, ACPITable {
    let Name = "HPET"
    var Header: SDTHeader
    fileprivate let tablePointer: UnsafeMutablePointer<ACPIHPET>
    
    var HPETNumber: Int {
        return Int(tablePointer.pointee.hpet_number)
    }
    
    var isMainCounter64Bit: Bool {
        let val = BitArray(tablePointer.pointee.timer_block_id)
        return (val[13] != 0)
    }
    
    var VendorID: UInt16 {
        let val = BitArray(tablePointer.pointee.timer_block_id)
        return UInt16(val[16...31])
    }
    
    var legacyIRQReplacement: Bool {
        let val = BitArray(tablePointer.pointee.timer_block_id)
        return (val[15] != 0)
    }
    
    var comparatorCount: Int {
        let val = BitArray(tablePointer.pointee.timer_block_id)
        return Int(val[8...12])
    }
    
    var hardwareRevisionID: UInt8 {
        let val = BitArray(tablePointer.pointee.timer_block_id)
        return UInt8(val[0...7])
    }
    
    var description: String {
        return "HPET: \(Header) Vendor: 0x\(String(VendorID, radix: 16)) LegacyIRQ: \(legacyIRQReplacement) is64Bit: \(isMainCounter64Bit) NumberOfComparators: \(comparatorCount) rev: \(hardwareRevisionID) HPET Number: \(HPETNumber)"
    }
    
    init(ptr: Address) {
        Header = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: ptr.virtual))!)
        tablePointer = UnsafeMutablePointer<ACPIHPET>(bitPattern: ptr.virtual)!
        
    }
}
