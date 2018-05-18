//
//  Integer.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/18/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

public let KB = (1024)
public let MB = (1024*KB)
public let GB = (1024*MB)

public extension BinaryInteger {
    func bit(_ bit: Int) -> Bool {
        precondition(bit >= 0 && bit < MemoryLayout<Self>.size * 8,
                     "Bit must be in range 0-\(MemoryLayout<Self>.size * 8 - 1)")
        return self & Self(1 << UInt(bit)) != 0
    }
}

public extension UnsignedInteger {
    public typealias Byte = UInt8
    public typealias Word = UInt16
    public typealias DWord = UInt32
    
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
    
    init(withWords words: [Word]) {
        let maxElements = MemoryLayout<Self>.size / MemoryLayout<Word>.size
        precondition(words.count > 0 && words.count <= maxElements,
                     "Array must have from 1-\(maxElements) elements")
        
        self = 0
        var shift: UInt = 0
        for word in words {
            self |= (Self(UInt(word) << shift))
            shift += UInt(MemoryLayout<Word>.size * 8)
        }
    }
    
    init(withWords words: Word...) {
        self.init(withWords: words)
    }
    
    init(withDWords dwords: [DWord]) {
        let maxElements = MemoryLayout<Self>.size / MemoryLayout<DWord>.size
        precondition(dwords.count > 0 && dwords.count <= maxElements,
                     "Array must have from 1-\(maxElements) elements")
        
        self = 0
        var shift: UInt = 0
        for dword in dwords {
            self |= (Self(UInt(dword) << shift))
            shift += UInt(MemoryLayout<DWord>.size * 8)
        }
    }
    
    init(withDWords dwords: DWord...) {
        self.init(withDWords: dwords)
    }
}
