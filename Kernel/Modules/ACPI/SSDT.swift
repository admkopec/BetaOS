//
//  SSDT.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/27/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct SSDT: Loggable, ACPITable {
    let Name = "SSDT"
    var Header: SDTHeader
    let AMLCode: AMLByteBuffer
    var description: String {
        return "SSDT: \(Header)"
    }
    init(ptr: Address) {
        Header  = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: ptr.virtual))!)
        let HeaderLength = MemoryLayout<ACPISDTHeader_t>.size
        let TotalLength  = Int(Header.Length)
        let AMLCodeLength = TotalLength - HeaderLength
        let AMLCodeAddress = Address(ptr.physical + UInt(HeaderLength), size: vm_size_t(AMLCodeLength), baseAddress: (ptr.physical, ptr.virtual))
        AMLCode = Data(start: UnsafePointer<UInt8>(bitPattern: AMLCodeAddress.virtual), count: AMLCodeLength)
    }
}
