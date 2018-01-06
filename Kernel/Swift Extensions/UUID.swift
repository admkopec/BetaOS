//
//  UUID.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/29/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

struct UUID: CustomStringConvertible, Equatable {
//    typealias StringLiteralType = String
    fileprivate let Data1: Int // (32-bit)
    fileprivate let Data2: Int // (16-bit)
    fileprivate let Data3: Int // (16-bit)
    fileprivate let Data4: Int // (16-bit)
    fileprivate let Data5: Int // (48-bit)
    var description: String {
        return ("\(String(Data1, radix: 16))".leftPadding(toLength: 8, withPad: "0") + "-" + "\(String(Data2, radix: 16))".leftPadding(toLength: 4, withPad: "0") + "-" + "\(String(Data3, radix: 16))".leftPadding(toLength: 4, withPad: "0") + "-" + "\(String(Data4, radix: 16))".leftPadding(toLength: 4, withPad: "0") + "-" + "\(String(Data5, radix: 16))".leftPadding(toLength: 12, withPad: "0")).uppercased()
    }
    
    static var Nil: UUID {
        return UUID(data: ByteArray([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]))
    }
    
    init(ptr: UnsafeMutablePointer<UInt8>) {
        Data1 = ByteArray(withBytes: ptr.advanced(by: 0).pointee, ptr.advanced(by: 1).pointee, ptr.advanced(by: 2).pointee, ptr.advanced(by: 3).pointee).asInt
        Data2 = ByteArray(withBytes: ptr.advanced(by: 4).pointee, ptr.advanced(by: 5).pointee).asInt
        Data3 = ByteArray(withBytes: ptr.advanced(by: 6).pointee, ptr.advanced(by: 7).pointee).asInt
//        Data4 = ByteArray(withBytes: ptr.advanced(by: 8).pointee, ptr.advanced(by: 9).pointee).asInt
//        Data5 = ByteArray(withBytes: ptr.advanced(by: 10).pointee, ptr.advanced(by: 11).pointee, ptr.advanced(by: 12).pointee, ptr.advanced(by: 13).pointee, ptr.advanced(by: 14).pointee, ptr.advanced(by: 15).pointee).asInt
        Data4 = ByteArray(withBytes: ptr.advanced(by: 9).pointee, ptr.advanced(by: 8).pointee).asInt
        Data5 = ByteArray(withBytes: ptr.advanced(by: 15).pointee, ptr.advanced(by: 14).pointee, ptr.advanced(by: 13).pointee, ptr.advanced(by: 12).pointee, ptr.advanced(by: 11).pointee, ptr.advanced(by: 10).pointee).asInt
}
    
    init(data: ByteArray) {
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
    
//    init(stringLiteral value: UUID.StringLiteralType) {
//        // Fix Me!
//        guard value.count == "00000000-0000-0000-0000-000000000000".count else {
//            self = UUID.Nil
//            return
//        }
//        var bytes = [UInt8]()
//        var lastChars = ""
//        for char in value {
//            if char == "-" {
//                continue
//            }
//            lastChars += String(char)
//            if lastChars.count == 2 {
//                bytes.append(UInt8(/*Int(lastChars, radix: 16) != nil ? Int(lastChars, radix: 16)! :*/ 0))
//                lastChars = ""
//            }
//        }
//        self = UUID.init(data: ByteArray(bytes))
//    }
    
    static func ==(lhs: UUID, rhs: UUID) -> Bool {
        return (lhs.Data1 == rhs.Data1) && (lhs.Data2 == rhs.Data2) && (lhs.Data3 == rhs.Data3) && (lhs.Data4 == rhs.Data4) && (lhs.Data5 == rhs.Data5)
    }
}
