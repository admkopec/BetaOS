//
//  MBR.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/30/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

final class MBR: PartitionTable {
    let Header: MBRHeader
    fileprivate(set) var Partitions = [Partition]()
    var description: String {
        return "MBR" + ": " + Header.description
    }
    
    init?(disk: Disk) {
        Header = MBRHeader(data: disk.read(lba: 0))
        guard Header.Signature == (0x55, 0xAA) else { return nil }
    }
    
    enum PartitionTypes: Int, CustomStringConvertible {
        case Nil     = 0
        case EFS     = 0xEF
        case HFSPlus = 0xAF
        case NTFS    = 0x07
        case FAT12   = 0x01
        case FAT16   = 0x04; case FAT16LB = 0x0E; case FAT16B = 0x06
        case FAT32   = 0x0C; case FAT32C = 0x0B
        var description: String {
            get {
                switch self {
                case .Nil:
                    return "Empty Partition"
                case .EFS:
                    return "EFI File System"
                case .HFSPlus:
                    return "Mac OS Extended"
                case .NTFS:
                    return "NT File System"
                case .FAT12:
                    return "FAT12"
                case .FAT16:
                    return "FAT16"
                case .FAT16B:
                    return "FAT16"
                case .FAT16LB:
                    return "FAT16"
                case .FAT32:
                    return "FAT32"
                case .FAT32C:
                    return "FAT32"
                }
            }
        }
    }
}

struct MBRHeader: CustomStringConvertible {
    let Signature: (UInt8, UInt8)
    var description: String {
        return "MBRHeader: Signature \(String(Signature.0, radix: 16)) \(String(Signature.1, radix: 16))"
    }
    
    init(data: UnsafeMutableBufferPointer<UInt8>) {
        Signature = (data[510], data[511])
    }
}
