//
//  Disk.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/6/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

protocol Disk: CustomStringConvertible {
    func read(lba: UInt64, count: Int, buffer: UnsafeMutableBufferPointer<UInt8>) -> MutableData
    func write(lba: UInt64, count: Int, buffer: UnsafeMutablePointer<UInt8>)
    func PutDriveToSleep()
}

extension Disk {
    func read(lba: UInt64, count: Int) -> MutableData {
        let LBA = UnsafeMutableBufferPointer<UInt8>(start: UnsafeMutablePointer<UInt8>.allocate(capacity: 512 * count), count: 512 * count)
        return read(lba: lba, count: count, buffer: LBA)
    }
    
    func read(lba: UInt64) -> MutableData {
        return read(lba: lba, count: 1)
    }
    
    func write(lba: UInt64, buffer: UnsafeMutablePointer<UInt8>) {
        return write(lba: lba, count: 1, buffer: buffer)
    }
}
