//
//  Address.swift
//  Addressing
//
//  Created by Adam Kopeć on 11/5/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Darwin

fileprivate func mapAddress(start: UInt, size: UInt) -> UInt {
    return UInt(io_map(UInt64(start & ~3), UInt((size + vm_page_mask) & ~vm_page_mask), (0x2 | 0x4 | 0x1)))
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
    fileprivate var rawValueVirt: UInt
    public var description: String {
        return "Physical Address is 0x\(String(rawValue, radix: 16)), Virtual Address is 0x\(String(virtual, radix: 16))"
    }
    /**
     Virtual address represented as a bit pattern.
     */
    public var virtual: UInt {
        if rawValueVirt >= 0xffffff8000000000 as UInt {
            return rawValueVirt
        } else {
            guard baseAddr != (0x0, 0x0) else {
                rawValueVirt = mapAddress(start: physical, size: vm_page_size)
                baseAddr = (physical, rawValueVirt)
                return rawValueVirt
            }
            guard Int(max(physical, baseAddr.original)) - Int(min(physical, baseAddr.original)) < Int(vm_page_size) else {
                rawValueVirt = mapAddress(start: physical, size: vm_page_size)
                baseAddr = (physical, rawValueVirt)
                return rawValueVirt
            }
            guard Int(Int(physical) - Int(baseAddr.original)) > Int(0) else {
                rawValueVirt = mapAddress(start: physical, size: vm_page_size)
                baseAddr = (physical, rawValueVirt)
                return rawValueVirt
            }
            guard UInt64((UInt64(physical) + UInt64(baseAddr.mapped) - UInt64(baseAddr.original))) > UInt64(0) else {
                rawValueVirt = mapAddress(start: physical, size: vm_page_size)
                baseAddr = (physical, rawValueVirt)
                return rawValueVirt
            }
            rawValueVirt  = physical - baseAddr.original + baseAddr.mapped
            return rawValueVirt
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
    fileprivate(set) public var baseAddr: (original: UInt, mapped: UInt) = (0x0, 0x0)
    
    public static func ==(lhs: Address, rhs: Address) -> Bool {
        return lhs.rawValue == rhs.rawValue
    }
    
    public static func ==(lhs: Address, rhs: UInt) -> Bool {
        return lhs.rawValue == rhs
    }
    
    public static func ==(lhs: UInt, rhs: Address) -> Bool {
        return lhs == rhs.rawValue
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
        rawValueVirt         = rawValue
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init(_ address64: UInt64, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = UInt(address64)
        rawValueVirt         = rawValue
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init(_ address32: UInt32, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = UInt(address32)
        rawValueVirt         = rawValue
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init(integerLiteral value: UInt) {
        rawValue             = value
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init?<T>(exactly source: T) where T : BinaryInteger {
        if let value = UInt(exactly: source) {
            rawValue             = value
            rawValueVirt         = rawValue
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
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init<T>(truncatingIfNeeded source: T) where T : BinaryInteger {
        rawValue             = UInt(truncatingIfNeeded: source)
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init<T>(_ source: T) where T : BinaryFloatingPoint {
        rawValue             = UInt(source)
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    public init<T>(clamping source: T) where T : BinaryInteger {
        rawValue             = UInt(clamping: source)
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
}

