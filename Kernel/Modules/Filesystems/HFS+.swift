//
//  HFS+.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/30/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import CustomArrays

struct HFSPlusHeader {
    var Signature: String /* == kHFSPlusSigWord */
    var Version: UInt16 /* == kHFSPlusVersion */
    var Attributes: UInt32 /* volume attributes */
    var LastMountedVersion: String /* implementation version which last mounted volume */
//    var JournalInfoBlock: UInt32 /* block addr of journal info (if volume is journaled, zero otherwise) */
//    var CreateDate: UInt32 /* date and time of volume creation */
//    var ModifyDate: UInt32 /* date and time of last modification */
//    var BackupDate: UInt32 /* date and time of last backup */
//    var CheckedDate: UInt32 /* date and time of last disk check */
    var FileCount: UInt32 /* number of files in volume */
    var FolderCount: UInt32 /* number of directories in volume */
    var BlockSize: UInt32 /* size (in bytes) of allocation blocks */
//    var TotalBlocks: UInt32 /* number of allocation blocks in volume (includes this header and VBM*/
//    var FreeBlocks: UInt32 /* number of unused allocation blocks */
//    var NextAllocation: UInt32 /* start of next allocation search */
//    var RsrcClumpSize: UInt32 /* default resource fork clump size */
//    var DataClumpSize: UInt32 /* default data fork clump size */
//    var NextCatalogID: UInt32 /* next unused catalog node ID */
//    var WriteCount: UInt32 /* volume write count */
//    var EncodingsBitmap: UInt64 /* which encodings have been use  on this volume */
    
    
//    public var finderInfo: (UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8) /* information used by the Finder */
    
    
//    public var allocationFile: HFSPlusForkData /* allocation bitmap file */
//    public var extentsFile: HFSPlusForkData /* extents B-tree file */
//    public var catalogFile: HFSPlusForkData /* catalog B-tree file */
//    public var attributesFile: HFSPlusForkData /* extended attributes B-tree file */
//    public var startupFile: HFSPlusForkData /* boot file (secondary loader) */
    init(data: UnsafeMutableBufferPointer<UInt8>) {
        Signature  = String(UnicodeScalar(data[0])) + String(UnicodeScalar(data[1]))
        Version    = ByteArray(withBytes: data[2], data[3]).BigEndianasUInt16
        Attributes = ByteArray(withBytes: data[4], data[5], data[6], data[7]).BigEndianasUInt32
        LastMountedVersion = String(UnicodeScalar(data[8])) + String(UnicodeScalar(data[9])) + String(UnicodeScalar(data[10])) + String(UnicodeScalar(data[11]))
        FileCount = ByteArray(withBytes: data[0x20], data[0x21], data[0x22], data[0x23]).BigEndianasUInt32
        FolderCount = ByteArray(withBytes: data[0x24], data[0x25], data[0x26], data[0x27]).BigEndianasUInt32
        BlockSize = ByteArray(withBytes: data[0x28], data[0x29], data[0x2A], data[0x2B]).BigEndianasUInt32
    }
}

final class HFSPlus: Partition {
    let Header: HFSPlusHeader
    let VolumeName: String
    let AlternateName: String
    let CaseSensitive: Bool
    
    init?(data: UnsafeMutableBufferPointer<UInt8>) {
        Header = HFSPlusHeader(data: data)
        VolumeName = "Unknown"
        AlternateName = "Unknown"
        if Header.Signature == "HX" {
            CaseSensitive = true
        } else {
            CaseSensitive = false
        }
    }
    
    init?(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        VolumeName = partitionEntry.Name
        AlternateName = partitionEntry.Name
        Header = HFSPlusHeader(data: disk.read(lba: UInt64(partitionEntry.FirstLBA + 2)))
        if Header.Signature == "HX" {
            CaseSensitive = true
        } else {
            CaseSensitive = false
        }
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
