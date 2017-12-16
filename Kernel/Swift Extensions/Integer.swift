//
//  Integer.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

let KB = (1024)
let MB = (1024*KB)
let GB = (1024*MB)

extension UnsignedInteger {
    typealias Byte = UInt8
    typealias Word = UInt16
    typealias DWord = UInt32
    
    func bit(_ bit: Int) -> Bool {
        precondition(bit >= 0 && bit < MemoryLayout<Self>.size * 8,
                     "Bit must be in range 0-\(MemoryLayout<Self>.size * 8 - 1)")
        return self & Self(1 << UInt(bit)) != 0
    }
    
    init(withBytes bytes: [Byte]) {
        precondition(bytes.count > 0 && bytes.count <= MemoryLayout<Self>.size, "Array must have from 1-\(MemoryLayout<Self>.size) elements")
        
        self = 0
        var shift: UInt = 0
        for byte in bytes {
            self |= (Self(UInt(byte) << shift))
            shift += UInt(MemoryLayout<Byte>.size * 8)
        }
    }
    
    init(withBytes bytes: Byte...) {
        self.init(withBytes: bytes)
    }
}
