//
//  FAT.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/31/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import CustomArrays

class FATHeader {
    let BootJMP: (UInt8, UInt8, UInt8)
    var OemName = ""
    let BytesPerSector: UInt16
    let SectorsPerCluster: UInt8
    let ReservedSectorCount: UInt16
    var NumberOfAllocationTables: UInt8
    let RootEntryCount: UInt16
    let MediaType: UInt8
    fileprivate let TableSize16: UInt16
    let SectorsPerTrack: UInt16
    let HeadOrSideCount: UInt16
    let HiddenSectorCount: UInt32
    
    let TotalSectors: Int
    
    init(data: MutableData) {
        BootJMP = (data[0], data[1], data[2])
        var i = 3
        while i < 11 {
            OemName.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        BytesPerSector = ByteArray(withBytes: data[11], data[12]).asUInt16
        SectorsPerCluster = data[13]
        ReservedSectorCount = ByteArray(withBytes: data[14], data[15]).asUInt16
        NumberOfAllocationTables = data[16]
        RootEntryCount = ByteArray(withBytes: data[17], data[18]).asUInt16
        let TotalSectors16 = ByteArray(withBytes: data[19], data[20]).asInt
        MediaType = data[21]
        TableSize16 = ByteArray(withBytes: data[22], data[23]).asUInt16
        SectorsPerTrack = ByteArray(withBytes: data[24], data[25]).asUInt16
        HeadOrSideCount = ByteArray(withBytes: data[26], data[27]).asUInt16
        HiddenSectorCount = ByteArray(withBytes: data[28], data[29], data[30], data[31]).asUInt32
        let TotalSectors32 = ByteArray(withBytes: data[32], data[33], data[34], data[35]).asInt
        if TotalSectors16 == 0 {
            TotalSectors = TotalSectors32
        } else {
            TotalSectors = TotalSectors16
        }
    }
}

final class FAT16Header: FATHeader { // and also FAT12
    fileprivate(set) var TableSize = 0
    let BiosDriveNumber: UInt8
    let BootSignature: UInt8
    let VolumeID: UInt32
    var VolumeLabel  = ""
    var FATTypeLabel = ""
    
    override init(data: MutableData) {
        BiosDriveNumber = data[36]
        BootSignature   = data[38]
        VolumeID        = ByteArray(withBytes: data[39], data[40], data[41], data[42]).asUInt32
        var i = 43
        while i < 54 {
            VolumeLabel.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        VolumeLabel = VolumeLabel.trim()
        i = 54
        while i < 62 {
            FATTypeLabel.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        FATTypeLabel = FATTypeLabel.trim()
        super.init(data: data)
        TableSize = Int(TableSize16)
    }
}

final class FAT32Header: FATHeader {
    fileprivate(set) var TableSize = 0
    let ExtendedFlags: UInt16
    let FATVersion: (Major: UInt8, Minor: UInt8)
    let RootCluster: UInt32
    let FATInfoLcation: UInt16
    let BackupBootSector: UInt16
    let DriveNumber: UInt8
    let BootSignature: UInt8
    let VolumeID: UInt32
    var VolumeLabel  = ""
    var FATTypeLabel = ""
    
    override init(data: MutableData) {
        let TableSize32 = ByteArray(withBytes: data[36], data[37], data[38], data[39]).asInt
        ExtendedFlags   = ByteArray(withBytes: data[40], data[41]).asUInt16
        FATVersion      = (Major: data[42], Minor: data[43])
        RootCluster     = ByteArray(withBytes: data[44], data[45], data[46], data[47]).asUInt32
        FATInfoLcation  = ByteArray(withBytes: data[48], data[49]).asUInt16
        BackupBootSector = ByteArray(withBytes: data[50], data[51]).asUInt16
        DriveNumber     = data[64]
        BootSignature   = data[66]
        VolumeID        = ByteArray(withBytes: data[67], data[68], data[69], data[70]).asUInt32
        var i = 71
        while i < 82 {
            VolumeLabel.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        VolumeLabel = VolumeLabel.trim()
        i = 82
        while i < 90 {
            FATTypeLabel.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        FATTypeLabel = FATTypeLabel.trim()
        super.init(data: data)
        if TableSize16 == 0 {
            TableSize = TableSize32
        } else {
            TableSize = Int(TableSize16)
        }
    }
}

struct DirectoryEntry {
    let Offset: Int
    var Name: String
    var Extension: String
    var Flags: UInt8
    var CreationTime: UInt8
    var TimeOfCreation: UInt16//Date
    var TimeOfModification: UInt16//Date
    var Size: Int
    var FirstCluster: UInt32
    
    init?(data: MutableData, offset: Int) {
        Offset = offset
        Name = ""
        Extension = ""
        if data[11] == 0xF {
            // Long File Name
            return nil
        }
        var i = 0
        while i < 8 {
            guard data[i] <= 170 && data[i] >= 0x20 else {
                return nil
            }
            Name.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        Name = Name.trim()
        if Name == " " {
            Name = ""
        }
        i = 8
        while i < 11 {
            Extension.append(String(UnicodeScalar(data[i])))
            i += 1
        }
        Extension = Extension.trim()
        if Extension == " " {
            Extension = ""
        }
        Flags = data[11]
        CreationTime = data[13]
        var timeBits = BitArray(ByteArray(withBytes: data[14], data[15]).asUInt16)
        TimeOfCreation = timeBits.asUInt16
        timeBits = BitArray(ByteArray(withBytes: data[16], data[17]).asUInt16)
        TimeOfModification = timeBits.asUInt16
        Size = ByteArray([data[28], data[29], data[30], data[31]]).asInt
        FirstCluster = ByteArray([data[20], data[21]]).asUInt32 << 32 + ByteArray([data[26], data[27]]).asUInt32
    }
    
    var computed: [UInt8] {
        var buffer = [UInt8](repeating: 0, count: 32)
        var bytes = ByteArray([UInt8](Name.utf8))
        var i = 0
        for byte in bytes {
            buffer[i] = byte
            i += 1
        }
        if i < 8 {
            for j in i ... 7 {
                buffer[j] = 0x20
            }
        }
        i = 8
        bytes = ByteArray([UInt8](Extension.utf8))
        for byte in bytes {
            buffer[i] = byte
            i += 1
        }
        if i < 11 {
            for j in i ... 10 {
                buffer[j] = 0x20
            }
        }
        buffer[11] = Flags
        buffer[13] = CreationTime
        i = 28
        bytes = ByteArray(UInt32(Size))
        for byte in bytes {
            buffer[i] = byte
            i += 1
        }
        bytes = ByteArray(FirstCluster)
        buffer[20] = bytes[2]
        buffer[21] = bytes[3]
        buffer[26] = bytes[0]
        buffer[27] = bytes[1]
        return buffer
    }
}

class FAT12: Partition {
    let Header: FAT16Header
    let VolumeName: String
    let AlternateName: String
    var CaseSensitive: Bool { return false }
    fileprivate let DiskDevice: Disk
    fileprivate let FirstLBA: UInt64
    fileprivate let FirstUsableCluster: Int
    fileprivate let SectorsPerCluster: Int
    fileprivate let RootCluster: UInt32
    
    init?(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        DiskDevice = disk
        RootCluster = 2
        FirstLBA   = UInt64(partitionEntry.FirstLBA)
        Header = FAT16Header(data: disk.read(lba: FirstLBA))
        guard Header.FATTypeLabel == "FAT12" else {
            return nil
        }
        if partitionEntry.Name.count < 1 {
            VolumeName = Header.VolumeLabel
        } else {
            VolumeName = partitionEntry.Name
        }
        AlternateName = Header.VolumeLabel
        FirstUsableCluster = partitionEntry.FirstLBA + Int(Header.ReservedSectorCount) + (Int(Header.NumberOfAllocationTables) * Int(Header.TableSize))
        SectorsPerCluster = Int(Header.SectorsPerCluster)
        guard Header.BytesPerSector == 512 else {
            kprint("Sector sizes do not match! Can't support it yet")
            return nil
        }
    }
    
    func ReadDirectory(fromCluster: UInt32) -> [DirectoryEntry] {
        return [DirectoryEntry]()
    }
    
    func WriteDirectory(fromCluster: UInt32, entries: [DirectoryEntry]) {
        //
    }
    
    func ReadFile(fromCluster: UInt32) -> MutableData? {
        return nil
    }
    
    func WriteFile(fromCluster: UInt32, file: MutableData) {
        //
    }
}

class FAT16: Partition {
    let Header: FAT16Header
    let VolumeName: String
    let AlternateName: String
    var CaseSensitive: Bool { return false }
    fileprivate let DiskDevice: Disk
    fileprivate let FirstLBA: UInt64
    fileprivate let FirstUsableCluster: Int
    fileprivate let SectorsPerCluster: Int
    fileprivate let RootCluster: UInt32
    
    init?(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        DiskDevice = disk
        RootCluster = 2
        FirstLBA   = UInt64(partitionEntry.FirstLBA)
        Header = FAT16Header(data: disk.read(lba: FirstLBA))
        guard Header.FATTypeLabel == "FAT16" else {
            return nil
        }
        if partitionEntry.Name.count < 1 {
            VolumeName = Header.VolumeLabel
        } else {
            VolumeName = partitionEntry.Name
        }
        AlternateName = Header.VolumeLabel
        FirstUsableCluster = partitionEntry.FirstLBA + Int(Header.ReservedSectorCount) + (Int(Header.NumberOfAllocationTables) * Int(Header.TableSize))
        SectorsPerCluster = Int(Header.SectorsPerCluster)
        guard Header.BytesPerSector == 512 else {
            kprint("Sector sizes do not match! Can't support it yet")
            return nil
        }
    }
    
    func ReadDirectory(fromCluster: UInt32) -> [DirectoryEntry] {
        return [DirectoryEntry]()
    }
    
    func WriteDirectory(fromCluster: UInt32, entries: [DirectoryEntry]) {
        //
    }
    
    func ReadFile(fromCluster: UInt32) -> MutableData? {
        return nil
    }
    
    func WriteFile(fromCluster: UInt32, file: MutableData) {
        //
    }
}

class FAT32: Partition {
    let Header: FAT32Header
    let VolumeName: String
    let AlternateName: String
    var CaseSensitive: Bool { return false }
    fileprivate let DiskDevice: Disk
    fileprivate let FirstLBA: UInt64
    fileprivate let LastLBA: UInt64
    fileprivate let FirstUsableCluster: Int
    fileprivate let SectorsPerCluster: Int
    
    init?(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        DiskDevice = disk
        FirstLBA   = UInt64(partitionEntry.FirstLBA)
        LastLBA    = UInt64(partitionEntry.LastLBA)
        Header = FAT32Header(data: disk.read(lba: FirstLBA))
        guard Header.FATTypeLabel == "FAT32" else {
            return nil
        }
        if partitionEntry.Name.count < 1 {
            VolumeName = Header.VolumeLabel
        } else {
            VolumeName = partitionEntry.Name
        }
        AlternateName = Header.VolumeLabel
        FirstUsableCluster = partitionEntry.FirstLBA + Int(Header.ReservedSectorCount) + (Int(Header.NumberOfAllocationTables) * (Header.TableSize))
        SectorsPerCluster = Int(Header.SectorsPerCluster)
        guard Header.BytesPerSector == 512 else {
            kprint("Sector sizes do not match! Can't support that yet")
            return nil
        }
    }
    
    func ReadDirectory(fromCluster: UInt32) -> [DirectoryEntry] {
        var Cluster = fromCluster
        if Cluster == 0 {
            Cluster = Header.RootCluster
        }
        
        let clusters = GetClusterChain(firstCluster: Cluster)
        
        var directoryEntries = [DirectoryEntry]()
        var j = 0
        for cluster in clusters {
            var i = 0
            let buf = DiskDevice.read(lba: GetLBA(fromCluster: cluster), count: SectorsPerCluster)
            while i < 512 {
                let buffer = UnsafeMutableBufferPointer<UInt8>(start: buf.baseAddress!.advanced(by: i), count: 32)
                if let dirEntry = DirectoryEntry(data: buffer, offset: i + j) {
                    directoryEntries.append(dirEntry)
                }
                i += 32
            }
            j += 512
        }
        return directoryEntries
    }
    
    func WriteDirectory(fromCluster: UInt32, entries: [DirectoryEntry]) {
        var Cluster = fromCluster
        if Cluster == 0 {
            Cluster = Header.RootCluster
        }
        
        let clusters = GetClusterChain(firstCluster: Cluster)
        
        var j = 0
        for cluster in clusters {
            var i = 0
            let buf = DiskDevice.read(lba: GetLBA(fromCluster: cluster), count: SectorsPerCluster)
            while i < 512 {
                for entry in entries {
                    if entry.Offset == i + j {
                        let entryComputed = entry.computed
                        for k in 0 ... 31 {
                            buf[i + k] = entryComputed[k]
                        }
                    }
                }
                i += 32
            }
            DiskDevice.write(lba: GetLBA(fromCluster: cluster), count: SectorsPerCluster, buffer: buf.baseAddress!)
            j += 512
        }
    }
    
    func ReadFile(fromCluster: UInt32) -> UnsafeMutableBufferPointer<UInt8>? {
        guard fromCluster != 0 else {
            kprint("It's a directory!")
            return nil
        }
        let clusters = GetClusterChain(firstCluster: fromCluster)
        let count = clusters.count * 512
        let File = UnsafeMutableBufferPointer<UInt8>(start: UnsafeMutablePointer<UInt8>.allocate(capacity: count), count: count)
        for (i, cluster) in clusters.enumerated() {
            _ = DiskDevice.read(lba: GetLBA(fromCluster: cluster), count: SectorsPerCluster, buffer: UnsafeMutableBufferPointer<UInt8>(start: File.baseAddress!.advanced(by: i * 512 * SectorsPerCluster), count: 512 * SectorsPerCluster))
        }
        return File
    }
    
    func WriteFile(fromCluster: UInt32, file: UnsafeMutableBufferPointer<UInt8>) {
        guard fromCluster >= 2 else {
            kprint("It's a directory!")
            return
        }
        let clusters = GetClusterChain(firstCluster: fromCluster)
        var i = 0
        for cluster in clusters {
            if i >= file.count {
                let buf = UnsafeMutableBufferPointer<UInt8>(start: UnsafeMutablePointer<UInt8>.allocate(capacity: 512), count: 512)
                DiskDevice.write(lba: GetLBA(fromCluster: cluster), count: SectorsPerCluster, buffer: buf.baseAddress!)
                buf.deallocate()
                continue
            }
            DiskDevice.write(lba: GetLBA(fromCluster: cluster), count: SectorsPerCluster, buffer: file.baseAddress!.advanced(by: i))
            i += 512
        }
    }
    
    fileprivate func GetLBA(fromCluster cluster: UInt32) -> UInt64 {
        return UInt64(Int64(FirstUsableCluster) + (Int64(cluster) - 2) * Int64(SectorsPerCluster))
    }
    
    fileprivate func GetClusterChain(firstCluster: UInt32) -> [UInt32] {
        var cluster = firstCluster
        var chain = 1 as UInt32
        var array = [UInt32]()
        
        while (chain != 0) && (chain & 0x0FFFFFFF) < 0x0FFFFFF8 {
            let FATSector = FirstLBA + UInt64(Header.ReservedSectorCount) + (UInt64(cluster * 4) / 512)
            let FATOffset = Int((cluster * 4) % 512) / 4
            let buffer = UnsafeBufferPointer<UInt32>(start: UnsafeMutablePointer<UInt32>(bitPattern: UInt(bitPattern: DiskDevice.read(lba: FATSector).baseAddress)), count: 128)
            chain = buffer[FATOffset] & 0x0FFFFFFF
            if cluster <= LastLBA {
                array.append(cluster)
            }
            cluster = chain
        }
        return array
    }
    
    fileprivate func RemoveClusterChain(firstCluster: UInt32) {
        var cluster = firstCluster
        var chain = 1 as UInt32
        
        while (chain != 0) && (chain & 0x0FFFFFFF) < 0x0FFFFFF8 {
            let FATSector = FirstLBA + UInt64(Header.ReservedSectorCount) + (UInt64(cluster * 4) / 512)
            let FATOffset = Int(cluster % 512)
            let buffer = UnsafeMutableBufferPointer<UInt32>(start: UnsafeMutablePointer<UInt32>(bitPattern: UInt(bitPattern: DiskDevice.read(lba: FATSector).baseAddress)) , count: 128)
            chain = buffer[FATOffset] & 0x0FFFFFFF
            buffer[FATOffset] = 0
            cluster = chain
        }
    }
}
