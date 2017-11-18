//
//  ByteArray.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

struct ByteArray: Collection, Sequence, CustomStringConvertible {
    typealias Index   =  Int
    typealias Element = UInt8
    
    fileprivate var rawValue: UInt64
    var count: Int    = 2
    var isEmpty: Bool = true
    var startIndex: Index = 0
    var endIndex: Index   = 2
    var description: String {
        return "ByteArray (size: \(count)) value: 0x\(String(rawValue, radix: 16))"
    }
    
    init() {
        rawValue = 0
    }
    
    init(_ rawValue: Int) {
        self.rawValue = UInt64(rawValue)
        isEmpty  = false
        count    = MemoryLayout<Int>.size / MemoryLayout<UInt8>.size
        endIndex = count
    }
    
    init(_ rawValue: UInt16) {
        self.rawValue = UInt64(rawValue)
        isEmpty  = false
        count    = MemoryLayout<UInt16>.size / MemoryLayout<UInt8>.size
        endIndex = count
    }
    
    init(_ rawValue: UInt32) {
        self.rawValue = UInt64(rawValue)
        isEmpty  = false
        count    = MemoryLayout<UInt32>.size / MemoryLayout<UInt8>.size
        endIndex = count
    }
    
    init(_ rawValue: UInt64) {
        self.rawValue = rawValue
        isEmpty  = false
        count    = MemoryLayout<UInt64>.size / MemoryLayout<UInt8>.size
        endIndex = count
    }
    
    init(withBytes bytes: Element...) {
        precondition(bytes.count <= 8)
        
        self.rawValue = 0
        var shift: UInt16 = 0
        for byte in bytes {
            rawValue |= (UInt64(byte) << shift)
            shift += 8
        }
    }
    
    init(_ bytes: [Element]) {
        precondition(bytes.count <= 8)
        
        self.rawValue = 0
        var shift: UInt16 = 0
        for byte in bytes {
            rawValue |= (UInt64(byte) << shift)
            shift += 8
        }
    }
    
    subscript(index: Int) -> Element {
        get {
            precondition(index >= 0)
            precondition(index < endIndex)
            
            let shift = UInt16(index * 8)
            return Element((rawValue >> shift) & UInt64(Element.max))
        }
        
        set(newValue) {
            precondition(index >= 0)
            precondition(index < endIndex)
            precondition(newValue >= 0 || newValue <= Element.max)
            
            let shift = UInt16(index * 8)
            let mask = ~(UInt64(Element.max) << shift)
            let newValue = UInt64(newValue) << shift
            rawValue &= mask
            rawValue |= newValue
        }
    }
    
    struct Iterator: IteratorProtocol {
        var index = 0
        let array: ByteArray
        
        init(_ value: ByteArray) {
            array = value
        }
        
        mutating func next() -> Element? {
            if index < array.endIndex {
                defer { index += 1 }
                return array[index]
            } else {
                return nil
            }
        }
    }
    
    func makeIterator() -> Iterator {
        return Iterator(self)
    }
    
    func index(after i: Index) -> Index {
        precondition(i >= 0)
        precondition(i < endIndex)
        return i + 1
    }
    
    func toInt() -> Int {
        return Int(rawValue)
    }
}
