//
//  UUID.swift
//  UUID
//
//  Created by Adam Kopeć on 1/23/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import CustomArrays

public struct UUID: CustomStringConvertible, Equatable, ExpressibleByStringLiteral {
    public typealias StringLiteralType = String
    private let Data1: Int // (32-bit)
    private let Data2: Int // (16-bit)
    private let Data3: Int // (16-bit)
    private let Data4: Int // (16-bit)
    private let Data5: Int // (48-bit)
    public var description: String {
        return ("\(String(Data1, radix: 16))".leftPadding(toLength: 8, withPad: "0") + "-" + "\(String(Data2, radix: 16))".leftPadding(toLength: 4, withPad: "0") + "-" + "\(String(Data3, radix: 16))".leftPadding(toLength: 4, withPad: "0") + "-" + "\(String(Data4, radix: 16))".leftPadding(toLength: 4, withPad: "0") + "-" + "\(String(Data5, radix: 16))".leftPadding(toLength: 12, withPad: "0")).uppercased()
    }
    ///
    /// A generic Nil UUID filled with 0s.
    ///
    public static var Nil: UUID {
        return UUID(data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
    }
    
    public init<T: Collection>(data: T) where T.Element == UInt8, T.Index == Int {
        guard data.count == 16 else {
            self = UUID.Nil
            return
        }
        Data1 = ByteArray(withBytes: data[0], data[1], data[2], data[3]).asInt
        Data2 = ByteArray(withBytes: data[4], data[5]).asInt
        Data3 = ByteArray(withBytes: data[6], data[7]).asInt
        Data4 = ByteArray(withBytes: data[8], data[9]).asInt
        Data5 = ByteArray(withBytes: data[10], data[11], data[12], data[13], data[14], data[15]).asInt
    }
    
    public init<T: Collection>(dataLittleEndian data: T) where T.Element == UInt8, T.Index == Int {
        self = UUID(data: data)
    }
    
    public init<T: Collection>(dataBigEndian data: T) where T.Element == UInt8, T.Index == Int {
        guard data.count == 16 else {
            self = UUID.Nil
            return
        }
        Data1 = ByteArray(withBytes: data[3], data[2], data[1], data[0]).asInt
        Data2 = ByteArray(withBytes: data[5], data[4]).asInt
        Data3 = ByteArray(withBytes: data[7], data[6]).asInt
        Data4 = ByteArray(withBytes: data[9], data[8]).asInt
        Data5 = ByteArray(withBytes: data[15], data[14], data[13], data[12], data[11], data[10]).asInt
    }
    
    public init<T: Collection>(dataMixedEndian data: T) where T.Element == UInt8, T.Index == Int {
        guard data.count == 16 else {
            self = UUID.Nil
            return
        }
        Data1 = ByteArray(withBytes: data[0], data[1], data[2], data[3]).asInt
        Data2 = ByteArray(withBytes: data[4], data[5]).asInt
        Data3 = ByteArray(withBytes: data[6], data[7]).asInt
        Data4 = ByteArray(withBytes: data[9], data[8]).asInt
        Data5 = ByteArray(withBytes: data[15], data[14], data[13], data[12], data[11], data[10]).asInt
    }
    ///
    /// Creates a UUID from String representation of the value.
    ///
    /// - parameter asString: String representation of UUID.
    ///
    public init?(asString value: String) {
        let a = UUID(stringLiteral: value)
        if a == UUID.Nil && value != "00000000-0000-0000-0000-000000000000" {
            return nil
        }
        self = a
    }
    // FIXME: Fix String to Int conversion (Exclusivity Checking)
    public init(stringLiteral value: UUID.StringLiteralType) {
        guard value.count == "00000000-0000-0000-0000-000000000000".count else {
            self = UUID.Nil
            return
        }
        let separated = value.split(separator: "-")
        guard separated.count == 5 else {
            self = UUID.Nil
            return
        }
        #if DEBUG
        Data1 = 0
        Data2 = 0
        Data3 = 0
        Data4 = 0
        Data5 = 0
        #else
        Data1 = Int(separated[0], radix: 16) ?? 0
        Data2 = Int(separated[1], radix: 16) ?? 0
        Data3 = Int(separated[2], radix: 16) ?? 0
        Data4 = Int(separated[3], radix: 16) ?? 0
        Data5 = Int(separated[4], radix: 16) ?? 0
        #endif
    }
    
    public static func ==(lhs: UUID, rhs: UUID) -> Bool {
        return (lhs.Data1 == rhs.Data1) && (lhs.Data2 == rhs.Data2) && (lhs.Data3 == rhs.Data3) && (lhs.Data4 == rhs.Data4) && (lhs.Data5 == rhs.Data5)
    }
}
