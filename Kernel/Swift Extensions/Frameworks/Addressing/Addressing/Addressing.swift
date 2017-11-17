//
//  Address.swift
//  Addressing
//
//  Created by Adam Kopeć on 11/5/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

fileprivate func mapAddress(start: UInt, size: Int) -> UInt {
    return UInt(io_map(UInt64(start & ~3), UInt((size + Int(4095)) & ~4095), (0x2 | 0x4 | 0x1)))
}

public final class Address: Numeric, Comparable, BinaryInteger {
    public static let isSigned: Bool = false
    public var hashValue: Int
    public var magnitude: UInt
    public var words: UInt.Words
    public var bitWidth: Int
    public var trailingZeroBitCount: Int
    public typealias Magnitude = UInt
    public typealias IntegerLiteralType = UInt
    public typealias Words = UInt.Words
    fileprivate var rawValue: UInt
    public var description: String {
        return "Physical Address is 0x\(String(rawValue, radix: 16)), Virtual Address is 0x\(String(virtual, radix: 16))"
    }
    /**
     Virtual address represented as a bit pattern.
     */
    public var virtual: UInt {
        if rawValue >= (0xffffff8000000000 as UInt) {
            return rawValue
        }
        if baseAddr != (0x0, 0x0) {
            if rawValue - baseAddr.0 + baseAddr.1 < 4096 + baseAddr.1 {
                return rawValue - baseAddr.0 + baseAddr.1
            } else {
                let mapped = mapAddress(start: rawValue, size: 4096)
                baseAddr = (rawValue, mapped)
                return mapped
            }
        } else {
            let mapped = mapAddress(start: rawValue, size: 4096)
            baseAddr = (rawValue, mapped)
            return mapped
        }
    }
    /**
     Physical address represented as a bit pattern.
     */
    public var physical: UInt {
        return rawValue
    }
    /**
     Represents last mapped physical to virtual address pair
     */
    fileprivate(set) public var baseAddr: (UInt, UInt) = (0x0, 0x0)
    
    public static func ==(lhs: Address, rhs: Address) -> Bool {
        return lhs.rawValue == rhs.rawValue
    }
    
    public static func <(lhs: Address, rhs: Address) -> Bool {
        return lhs.rawValue < rhs.rawValue
    }
    
    public static func *(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue * rhs.rawValue)
    }
    
    public static func *=(lhs: inout Address, rhs: Address) {
        lhs.rawValue *= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func +(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue + rhs.rawValue)
    }
    
    public static func +=(lhs: inout Address, rhs: Address) {
        lhs.rawValue += rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func -(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue - rhs.rawValue)
    }
    
    public static func -=(lhs: inout Address, rhs: Address) {
        lhs.rawValue -= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func /(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue / rhs.rawValue)
    }
    
    public static func /=(lhs: inout Address, rhs: Address) {
        lhs.rawValue /= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func %(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue % rhs.rawValue)
    }
    
    public static func %=(lhs: inout Address, rhs: Address) {
        lhs.rawValue %= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func &=(lhs: inout Address, rhs: Address) {
        lhs.rawValue &= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func |=(lhs: inout Address, rhs: Address) {
        lhs.rawValue |= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static func ^=(lhs: inout Address, rhs: Address) {
        lhs.rawValue ^= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    public static prefix func ~(x: Address) -> Address {
        return Address(~x.rawValue)
    }
    
    public static func >>=<RHS>(lhs: inout Address, rhs: RHS) where RHS : BinaryInteger {
        lhs.rawValue >>= rhs
        lhs.magnitude  = lhs.rawValue
    }
    
    public static func <<=<RHS>(lhs: inout Address, rhs: RHS) where RHS : BinaryInteger {
        lhs.rawValue <<= rhs
        lhs.magnitude  = lhs.rawValue
    }
    
    public init(_ address: UInt, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = address
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init(_ address64: UInt64, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = UInt(address64)
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init(_ address32: UInt32, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = UInt(address32)
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init(integerLiteral value: UInt) {
        rawValue             = value
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init?<T>(exactly source: T) where T : BinaryInteger {
        if let value = UInt(exactly: source) {
            rawValue             = value
            magnitude            = rawValue
            hashValue            = rawValue.hashValue
            words                = rawValue.words
            bitWidth             = rawValue.bitWidth
            trailingZeroBitCount = rawValue.trailingZeroBitCount
        } else {
            return nil
        }
    }
    
    public init<T>(_ source: T) where T : BinaryInteger {
        rawValue             = UInt(source)
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init<T>(truncatingIfNeeded source: T) where T : BinaryInteger {
        rawValue             = UInt(truncatingIfNeeded: source)
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init<T>(_ source: T) where T : BinaryFloatingPoint {
        rawValue             = UInt(source)
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init<T>(clamping source: T) where T : BinaryInteger {
        rawValue             = UInt(clamping: source)
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
}

