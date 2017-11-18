//
//  String.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

let KB = (1024)
let MB = (1024*KB)
let GB = (1024*MB)

extension String {
    init(_ rawPtr: UnsafeRawPointer, maxLength: Int) {
        if maxLength < 0 {
            self = ""
            return
        }
        let ptr = rawPtr.bindMemory(to: UInt8.self, capacity: maxLength)
        var buffer: [UInt8]
        buffer = [UInt8](UnsafeBufferPointer(start: ptr, count: maxLength))
        buffer.append(UInt8(0))
        self = String.init(cString: buffer)
    }
    
    func leftPadding(toLength: Int, withPad: String = " ") -> String {
        
        guard toLength > self.count else { return self }
        
        let padding = String(repeating: withPad, count: toLength - self.count)
        
        return padding + self
    }
}

extension Loggable {
    /**
     Writes the textual representations of the given items to default logging destination.

     - parameter items: Zero or more items to print.
     - parameter level: Level of message being logged.
     - parameter separator: A string to print between each item. The default is a single space (`" "`).
     - parameter terminator: The string to print after all items have been printed. The default is a newline (`"\n"`).

     */
    public func Log(_ items: String..., level: LogLevel, separator: String = " ", terminator: String = "\n") -> Void {
        let item = items.map {"\($0)"} .joined(separator: separator)
        switch level {
        case .Info:
            let color = get_font_color()
            change_font_color(0x0000A5FF)
            kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
            change_font_color(color)
        case .Verbose:
            #if DEBUG
                kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
            #endif
        case .Debug:
            #if DEBUG
                let color = get_font_color()
                change_font_color(0x0009D400)
                kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
                change_font_color(color)
            #endif
        case .Warning:
            let color = get_font_color()
            change_font_color(0x00FFAE42)
            kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
            change_font_color(color)
        case .Error:
            let color = get_font_color()
            change_font_color(0x00ED4337)
            kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
            change_font_color(color)
        }
    }
}


///
///   TEMP (Until fixing framework)
///

fileprivate func mapAddress(start: UInt, size: UInt) -> UInt {
    return UInt(io_map(UInt64(start & ~3), UInt((size + vm_page_mask) & ~vm_page_mask), (0x2 | 0x4 | 0x1)))
}

final class Address: Numeric, Comparable, BinaryInteger {
    static let isSigned: Bool = false
    var hashValue: Int
    var magnitude: UInt
    var words: UInt.Words
    var bitWidth: Int
    var trailingZeroBitCount: Int
    typealias Magnitude = UInt
    typealias IntegerLiteralType = UInt
    typealias Words = UInt.Words
    fileprivate var rawValue: UInt
    fileprivate var rawValueVirt: UInt
    var description: String {
        return "Physical Address is 0x\(String(physical, radix: 16)), Virtual Address is 0x\(String(virtual, radix: 16))"
    }
    /**
     Virtual address represented as a bit pattern.
     */
    var virtual: UInt {
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
    var physical: UInt {
        return rawValue
    }
    /**
     Represents last mapped physical to virtual address pair
     */
    fileprivate(set) var baseAddr: (original: UInt, mapped: UInt) = (0x0, 0x0)

    static func ==(lhs: Address, rhs: Address) -> Bool {
        return lhs.rawValue == rhs.rawValue
    }
    
    static func ==(lhs: Address, rhs: UInt) -> Bool {
        return lhs.rawValue == rhs
    }
    
    static func ==(lhs: UInt, rhs: Address) -> Bool {
        return lhs == rhs.rawValue
    }

    static func <(lhs: Address, rhs: Address) -> Bool {
        return lhs.rawValue < rhs.rawValue
    }

    static func *(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue * rhs.rawValue)
    }

    static func *=(lhs: inout Address, rhs: Address) {
        lhs.rawValue *= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }

    static func +(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue + rhs.rawValue)
    }

    static func +=(lhs: inout Address, rhs: Address) {
        lhs.rawValue += rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    static func -(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue - rhs.rawValue)
    }
    
    static func -=(lhs: inout Address, rhs: Address) {
        lhs.rawValue -= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }

    static func /(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue / rhs.rawValue)
    }

    static func /=(lhs: inout Address, rhs: Address) {
        lhs.rawValue /= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    static func %(lhs: Address, rhs: Address) -> Address {
        return Address(lhs.rawValue % rhs.rawValue)
    }

    static func %=(lhs: inout Address, rhs: Address) {
        lhs.rawValue %= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }

    static func &=(lhs: inout Address, rhs: Address) {
        lhs.rawValue &= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }

    static func |=(lhs: inout Address, rhs: Address) {
        lhs.rawValue |= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }
    
    static func ^=(lhs: inout Address, rhs: Address) {
        lhs.rawValue ^= rhs.rawValue
        lhs.magnitude = lhs.rawValue
    }

    static prefix func ~(x: Address) -> Address {
        return Address(~x.rawValue)
    }

    static func >>=<RHS>(lhs: inout Address, rhs: RHS) where RHS : BinaryInteger {
        lhs.rawValue >>= rhs
        lhs.magnitude  = lhs.rawValue
    }

    static func <<=<RHS>(lhs: inout Address, rhs: RHS) where RHS : BinaryInteger {
        lhs.rawValue <<= rhs
        lhs.magnitude  = lhs.rawValue
    }

    init(_ address: UInt, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = address
        rawValueVirt         = rawValue
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }

    init(_ address64: UInt64, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = UInt(address64)
        rawValueVirt         = rawValue
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }

    init(_ address32: UInt32, baseAddress: (UInt, UInt) = (0x0, 0x0)) {
        rawValue             = UInt(address32)
        rawValueVirt         = rawValue
        baseAddr             = baseAddress
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }

    init(integerLiteral value: UInt) {
        rawValue             = value
        rawValueVirt         = 0
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }

    init?<T>(exactly source: T) where T : BinaryInteger {
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

    init<T>(_ source: T) where T : BinaryInteger {
        rawValue             = UInt(source)
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }

    init<T>(truncatingIfNeeded source: T) where T : BinaryInteger {
        rawValue             = UInt(truncatingIfNeeded: source)
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }

    init<T>(_ source: T) where T : BinaryFloatingPoint {
        rawValue             = UInt(source)
        rawValueVirt         = 0
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
    
    init<T>(clamping source: T) where T : BinaryInteger {
        rawValue             = UInt(clamping: source)
        rawValueVirt         = rawValue
        magnitude            = rawValue
        hashValue            = rawValue.hashValue
        words                = rawValue.words
        bitWidth             = rawValue.bitWidth
        trailingZeroBitCount = rawValue.trailingZeroBitCount
    }
}

