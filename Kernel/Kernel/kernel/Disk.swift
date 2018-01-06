//
//  Disk.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/6/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

protocol Disk: CustomStringConvertible {
    func read(lba: UInt64, count: Int) -> UnsafeMutableBufferPointer<UInt8>
    func write(lba: UInt64, count: Int, buffer: UnsafeMutablePointer<UInt8>)
}

extension Disk {
    func read(lba: UInt64) -> UnsafeMutableBufferPointer<UInt8> {
        return read(lba: lba, count: 1)
    }
    
    func write(lba: UInt64, buffer: UnsafeMutablePointer<UInt8>) {
        return write(lba: lba, count: 1, buffer: buffer)
    }
}
