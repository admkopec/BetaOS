//
//  BitArray.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

struct BitArray {
    fileprivate var rawValue: UInt64
    
    var description: String {
        return String(rawValue, radix: 2)
        
    }
    
    init() {
        rawValue = 0
    }
    
    init(_ rawValue: Int) {
        self.rawValue = UInt64(rawValue)
    }
    
    init(_ rawValue: UInt) {
        self.rawValue = UInt64(rawValue)
    }
    
    init(_ rawValue: UInt64) {
        self.rawValue = UInt64(rawValue)
    }
    
    init(_ rawValue: UInt32) {
        self.rawValue = UInt64(rawValue)
    }
    
    init(_ rawValue: UInt16) {
        self.rawValue = UInt64(rawValue)
    }
    
    init(_ rawValue: UInt8) {
        self.rawValue = UInt64(rawValue)
    }
    
    subscript(index: Int) -> Int {
        get {
            precondition(index >= 0)
            precondition(index < 64)
            
            return (rawValue & UInt64(1 << index) == 0) ? 0 : 1
        }
        
        set(newValue) {
            precondition(index >= 0)
            precondition(index < 64)
            precondition(newValue == 0 || newValue == 1)
            
            let mask: UInt64 = 1 << index
            if (newValue == 1) {
                rawValue |= mask
            } else {
                rawValue &= ~mask
            }
        }
    }
    
    subscript(index: CountableClosedRange<Int>) -> UInt64 {
        get {
            var ret: UInt64 = 0
            var bit: UInt64 = 1
            
            for i in index {
                let mask: UInt64 = 1 << i
                if rawValue & mask != 0 {
                    ret |= bit
                }
                bit <<= 1
            }
            return ret
        }
        set {
            var bit: UInt64 = 1
            for i in index {
                let mask: UInt64 = 1 << i
                if (newValue & bit) == 0 {
                    rawValue &= ~mask
                } else {
                    rawValue |= mask
                }
                bit <<= 1
            }
        }
    }
    
    var asUInt8: UInt8 {
        return UInt8(truncatingIfNeeded: rawValue)
    }
    
    var asUInt16: UInt16 {
        return UInt16(truncatingIfNeeded: rawValue)
    }
    
    var asUInt32: UInt32 {
        return UInt32(truncatingIfNeeded: rawValue)
    }
    
    var asUInt64: UInt64 {
        return UInt64(rawValue)
    }
    
    var asInt: Int {
        return Int(rawValue)
    }
}
