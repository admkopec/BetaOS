//
//  GPT.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/29/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

final class GPT: PartitionTable {
    let Header: GPTHeader
    fileprivate(set) var Partitions       = [Partition]()
    fileprivate      var PartitionEntries = [GPTPartitionEntry]()
    
    var description: String {
        var str = Header.description
        for partition in PartitionEntries {
            str += "\n\t"
            str += partition.description
        }
        return str
    }
    
    init?(disk: Disk) {
        Header = GPTHeader(data: disk.read(lba: 1))
        guard Header.Signature == "EFI PART" else {
            return nil
        }
        let data = disk.read(lba: UInt64(Header.StartingLBAOfArrayOfPartitionEntries))
        var i = 0
        while i < 512 {
            if let partitionEntry = GPTPartitionEntry(data: UnsafeMutableBufferPointer<UInt8>(start: data.baseAddress!.advanced(by: i), count: 128)) {
                PartitionEntries.append(partitionEntry)
                switch partitionEntry.PartitionType {
                case .EFS:
                    let esp = EFS(partitionEntry: partitionEntry, onDisk: disk)
                    Partitions.append(esp)
                    break
                case .HFSPlus:
                    if let hfs = HFSPlus(partitionEntry: partitionEntry, onDisk: disk) {
                        Partitions.append(hfs)
                    }
                    break
                case .WinData:
                    if let exfat = exFAT(partitionEntry: partitionEntry, onDisk: disk) {
                        Partitions.append(exfat)
                    }
                    if let fat32 = FAT32(partitionEntry: partitionEntry, onDisk: disk) {
                        Partitions.append(fat32)
                    }
                    if let fat16 = FAT16(partitionEntry: partitionEntry, onDisk: disk) {
                        Partitions.append(fat16)
                    }
                    if let fat12 = FAT12(partitionEntry: partitionEntry, onDisk: disk) {
                        Partitions.append(fat12)
                    }
                    break
                default:
                    break
                }
                i += 128
            } else {
                break
            }
        }
        System.sharedInstance.Disks.append(self)
    }
    
    enum PartitionTypes: String, CustomStringConvertible {
        case Nil     = "00000000-0000-0000-0000-000000000000"
        case EFS     = "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"
        case HFSPlus = "48465300-0000-11AA-AA11-00306543ECAC"
        case APFS    = "7C3457EF-0000-11AA-AA11-00306543ECAC"
        case WinData = "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7"
        var description: String {
            get {
                switch self {
                case .Nil:
                    return "Empty Partition"
                case .EFS:
                    return "EFI File System"
                case .HFSPlus:
                    return "Mac OS Extended"
                case .APFS:
                    return "Apple File System"
                case .WinData:
                    return "Microsoft Basic Data Partition"
                }
            }
        }
    }
}

struct GPTHeader: CustomStringConvertible {
    let Signature: String
    let Revision: Int
    let HeaderSize: Int
    let CRC32OfHeader: Int
    let CurrentLBA: Int
    let BackupLBA: Int
    let FirstUsableLBA: Int
    let LastUsableLBA: Int
    let DiskUUID: UUID
    let StartingLBAOfArrayOfPartitionEntries: Int
    let NumberOfPartitionEntriesInArray: Int
    let SizeOfPartitionEntry: Int
    let CRC32OfPartitionArray: Int
    
    var description: String {
        return "GPT (\"\(Signature)\"): Revision: \(String(Revision, radix: 16)) Current LBA: \(CurrentLBA) Disk UUID: \(DiskUUID.description)"
    }
    
    init(data: UnsafeMutableBufferPointer<UInt8>) {
        var i = 0
        var sig = ""
        while i < 8 {
            sig.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        Signature = sig
        Revision = ByteArray(withBytes: data[8], data[9], data[10], data[11]).asInt
        HeaderSize = ByteArray(withBytes: data[12], data[13], data[14], data[15]).asInt
        CRC32OfHeader = ByteArray(withBytes: data[16], data[17], data[18], data[19]).asInt
        CurrentLBA = ByteArray(withBytes: data[24], data[25], data[26], data[27], data[28], data[29], data[30], data[31]).asInt
        BackupLBA = ByteArray(withBytes: data[32], data[33], data[34], data[35], data[36], data[37], data[38], data[39]).asInt
        FirstUsableLBA = ByteArray(withBytes: data[40], data[41], data[42], data[43], data[44], data[45], data[46], data[47]).asInt
        LastUsableLBA = ByteArray(withBytes: data[48], data[49], data[50], data[51], data[52], data[53], data[54], data[55]).asInt
        DiskUUID = UUID(ptr: data.baseAddress!.advanced(by: 56))
        StartingLBAOfArrayOfPartitionEntries = ByteArray(withBytes: data[72], data[73], data[74], data[75], data[76], data[77], data[78], data[79]).asInt
        NumberOfPartitionEntriesInArray = ByteArray(withBytes: data[80], data[81], data[82], data[83]).asInt
        SizeOfPartitionEntry = ByteArray(withBytes: data[84], data[85], data[86], data[87]).asInt
        CRC32OfPartitionArray = ByteArray(withBytes: data[88], data[89], data[90], data[91]).asInt
    }
}

struct GPTPartitionEntry: CustomStringConvertible {
    let PartitionType: GPT.PartitionTypes
    let UniquePartitionUUID: UUID
    let FirstLBA: Int
    let LastLBA: Int
    let Flags: Int
    let Name: String
    
    var description: String {
        var typeStr = "\(PartitionType.description)"
        if let type = GPT.PartitionTypes(rawValue: PartitionType.description) {
            typeStr = type.description
        }
        return Name + " (\(typeStr)) First LBA: \(FirstLBA), Last LBA: \(LastLBA)"
    }
    
    init?(data: UnsafeMutableBufferPointer<UInt8>) {
        PartitionType = GPT.PartitionTypes(rawValue: UUID(ptr: data.baseAddress!).description) ?? GPT.PartitionTypes.Nil
        guard PartitionType != GPT.PartitionTypes.Nil else {
            return nil
        }
        UniquePartitionUUID = UUID(ptr: data.baseAddress!.advanced(by: 16))
        FirstLBA = ByteArray(withBytes: data[32], data[33], data[34], data[35], data[36], data[37], data[38], data[39]).asInt
        LastLBA = ByteArray(withBytes: data[40], data[41], data[42], data[43], data[44], data[45], data[46], data[47]).asInt
        Flags = ByteArray(withBytes: data[48], data[49], data[50], data[51], data[52], data[53], data[54], data[55]).asInt
        var utf16Str = [UInt16]()
        var i = 56
        while i < 56 + 72 {
            utf16Str.append(UInt16(ByteArray(withBytes: data[i], data[i + 1]).asInt))
            i += 2
        }
        if let str = String(utf16Characters: utf16Str) {
            Name = str
        } else {
            Name = "Unknown"
        }
    }
}
