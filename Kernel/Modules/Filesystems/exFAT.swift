//
//  exFAT.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import CustomArrays

class exFAT: Partition {
    class VolumeHeader {
        let BootJMP: (UInt8, UInt8, UInt8)
        var OemName = ""
        let SectorStart: UInt64
        let SectorCount: UInt64
        let FATSectorStart: UInt32
        let FATSectorCount: UInt32
        let ClusterSectorStart: UInt32
        let ClusterSectorCount: UInt32
        let RootCluster: UInt32
        let VolumeSerialNumber: UInt32
        let VersionMajor: UInt8
        let VersionMinor: UInt8
        let VolumeState: UInt16
        let SectorBits: UInt8 // Sector size as (1 << n)
        let SectorsPerClusterBits: UInt8 // Sectors Per Cluster as (1 << n)
        let FATCount: UInt8
        let DriveNumber: UInt8
        let PercentageAllocated: UInt8
        
        init?(data: UnsafeMutableBufferPointer<UInt8>) {
            BootJMP = (data[0], data[1], data[2])
            var i = 3
            while i < 11 {
                OemName.append(String(UnicodeScalar(data[i])))
                i += 1
            }
            guard OemName == "EXFAT   " else {
                return nil
            }
            SectorStart = ByteArray(withBytes: data[0x40], data[0x41], data[0x42], data[0x43], data[0x44], data[0x45], data[0x46], data[0x47]).asUInt64
            SectorCount = ByteArray(withBytes: data[0x48], data[0x49], data[0x4A], data[0x4B], data[0x4C], data[0x4D], data[0x4E], data[0x4F]).asUInt64
            FATSectorStart = ByteArray(withBytes: data[0x50], data[0x51], data[0x52], data[0x53]).asUInt32
            FATSectorCount = ByteArray(withBytes: data[0x54], data[0x55], data[0x56], data[0x57]).asUInt32
            ClusterSectorStart = ByteArray(withBytes: data[0x58], data[0x59], data[0x5A], data[0x5B]).asUInt32
            ClusterSectorCount = ByteArray(withBytes: data[0x5C], data[0x5D], data[0x5E], data[0x5F]).asUInt32
            RootCluster = ByteArray(withBytes: data[0x60], data[0x61], data[0x62], data[0x63]).asUInt32
            VolumeSerialNumber = ByteArray(withBytes: data[0x64], data[0x65], data[0x66], data[0x67]).asUInt32
            VersionMajor = data[0x68]
            VersionMinor = data[0x69]
            VolumeState = ByteArray(withBytes: data[0x6A], data[0x6B]).asUInt16
            SectorBits = data[0x6C]
            SectorsPerClusterBits = data[0x6D]
            FATCount = data[0x6E]
            DriveNumber = data[0x6F]
            PercentageAllocated = data[0x70]
        }
    }
    
    let Header: VolumeHeader
    let VolumeName: String
    let AlternateName: String
    let CaseSensitive: Bool
    
    init?(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        // To be implemented
        guard let head = VolumeHeader(data: disk.read(lba: UInt64(partitionEntry.FirstLBA))) else { return nil }
        Header = head
        VolumeName = partitionEntry.Name
        AlternateName = "exFAT"
        CaseSensitive = true
        return nil
    }
    
    func ReadDirectory(fromCluster: UInt32) -> [DirectoryEntry] {
        return [DirectoryEntry]()
    }
    
    func WriteDirectory(fromCluster: UInt32, entries: [DirectoryEntry]) {
        //
    }
    
    func ReadFile(fromCluster: UInt32) -> UnsafeMutableBufferPointer<UInt8>? {
        return nil
    }
    
    func WriteFile(fromCluster: UInt32, file: UnsafeMutableBufferPointer<UInt8>) {
        //
    }
}
