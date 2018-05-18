//
//  MemoryBuffer.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/17/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

typealias Data = UnsafeBufferPointer<UInt8>
typealias MutableData = UnsafeMutableBufferPointer<UInt8>

final class MemoryBuffer {
    let ptr:    UnsafeRawPointer
    let buffer: UnsafeBufferPointer<UInt8>
    var offset: Int = 0
    var bytesRemaining: Int {
        return (buffer.count - offset)
    }

    init(_ basePtr: UnsafeRawPointer, size: Int) {
        ptr = basePtr
        let bufferPtr = ptr.bindMemory(to: UInt8.self, capacity: size)
        buffer = UnsafeBufferPointer(start: bufferPtr, count: size)
    }

    init(_ baseAddr: UInt, size: Int) {
        ptr = UnsafeRawPointer(bitPattern: baseAddr)!
        let bufferPtr = ptr.bindMemory(to: UInt8.self, capacity: size)
        buffer = UnsafeBufferPointer(start: bufferPtr, count: size)
    }

    func read<T>() /*throws*/ -> T? {
        guard bytesRemaining > 0 else {
            return nil
//            throw ReadError.InvalidOffset
        }

        guard bytesRemaining >= MemoryLayout<T>.size else {
            return nil
//            throw ReadError.InvalidOffset
        }
        let result = ptr.advanced(by: offset).assumingMemoryBound(to: T.self).pointee//ptr.load(fromByteOffset: offset, as: T.self)
        offset += MemoryLayout<T>.size

        return result
    }


    func readAtIndex<T>(_ index: Int) /*throws*/ -> T? {
        guard index + MemoryLayout<T>.size <= buffer.count else {
            return nil
//            throw ReadError.InvalidOffset
        }
        return ptr.load(fromByteOffset: index, as: T.self)
    }

    func subBufferAtOffset(_ start: Int, size: Int) -> MemoryBuffer {
        return MemoryBuffer(ptr.advanced(by: start), size: size)
    }


    func subBufferAtOffset(_ start: Int) -> MemoryBuffer {
        var size = buffer.count - start
        if size < 0 {
            size = 0
        }
        return subBufferAtOffset(start, size: size)
    }

    // Functions to convert ASCII strings in memory to String. Inefficient
    // conversions because Foundation isnt available
    // Only really works for 7bit ASCII as it assumes the code is valid UTF-8
    func readASCIIZString(maxSize: Int) /*throws*/ -> String? {
        guard maxSize > 0 else {
            return nil
//            throw ReadError.InvalidOffset
        }

        guard bytesRemaining > 0 else {
            return nil
//            throw ReadError.InvalidOffset
        }

        guard bytesRemaining >= maxSize else {
            return nil
//            throw ReadError.InvalidOffset
        }

        var result = ""
        for _ in 0...maxSize-1 {
            guard let ch: UInt8 = /*try*/ read() else { return nil }
            if ch == 0 {
                break
            }
            result += String(Character(UnicodeScalar(ch)))
        }

        return result
    }


    // read from the current offset until the first nul byte is found
    func scanASCIIZString() /*throws*/ -> String? {
        var result = ""

        var ch: UInt8// = try read()
        guard let ch_: UInt8 = read() else { return nil }
        ch = ch_
        while ch != 0 {
            result += String(Character(UnicodeScalar(ch)))
            guard let Ch_: UInt8 = read() else { return nil }
            ch = Ch_//try read()
        }

        return result
    }
}

