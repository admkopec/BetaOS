//
//  Partition.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/6/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

protocol PartitionTable: CustomStringConvertible {
    var Partitions: [Partition] { get }
    init?(disk: Disk)
}

protocol Partition {
    var VolumeName: String { get }
    var AlternateName: String { get }
    var CaseSensitive: Bool { get }
    func ReadDirectory(fromCluster: UInt32) -> [DirectoryEntry]
    func WriteDirectory(fromCluster: UInt32, entries: [DirectoryEntry])
    func ReadFile(fromCluster: UInt32) -> MutableData?
    func WriteFile(fromCluster: UInt32, file: MutableData)
}
extension Partition {
    func ReadFile(fromPath: String) -> (Info: DirectoryEntry, Data: MutableData)? {
        let path = fromPath.split(separator: "/")
        
        var i = 0
        if path[i].count <= 1 {
            i += 1
        }
        if path[i] == "." {
            // Current Working Directory
            i += 1
        }
        var entries = ReadDirectory(fromCluster: 0)
        var folderEntry: DirectoryEntry?
        
        while true {
            if entries.count < 1 || i >= path.count {
                return nil
            }
            for entry in entries {
                guard entry.Flags != 0x00 && !entry.Flags.bit(3) else {
                    continue
                }
                if (entry.Name + "." + entry.Extension == path[i] || (CaseSensitive ? false : entry.Name + "." + entry.Extension == path[i].uppercased())) && !entry.Flags.bit(4) {
                    if let file = ReadFile(fromCluster: entry.FirstCluster) {
                        return (entry, file)
                    }
                    break
                }
                if (entry.Name == path[i] || (CaseSensitive ? false : entry.Name == path[i].uppercased())) && entry.Flags.bit(4) {
                    folderEntry = entry
                    break
                }
            }
            entries = [DirectoryEntry]()
            if let folder = folderEntry {
                entries = ReadDirectory(fromCluster: folder.FirstCluster)
                i += 1
                folderEntry = nil
            }
        }
    }
    
    func ReadFolder(fromPath: String) -> [DirectoryEntry]? {
        if fromPath == "/" {
            return ReadDirectory(fromCluster: 0)
        }
        if fromPath == "." {
            // Current Working Directory
            return ReadDirectory(fromCluster: 0)
        }
        let path = fromPath.split(separator: "/")
        var i = 0
        if path[i].count <= 1 {
            i += 1
        }
        if path[i] == "." {
            // Current Working Directory
            i += 1
        }
        var entries = ReadDirectory(fromCluster: 0)
        var folderEntry: DirectoryEntry?
        while true {
            if entries.count < 1 || i >= path.count {
                return nil
            }
            for entry in entries {
                guard entry.Flags != 0x00 && !entry.Flags.bit(3) else {
                    continue
                }
                if (entry.Name == path[i] || (CaseSensitive ? false : entry.Name == path[i].uppercased())) && entry.Flags.bit(4) {
                    if i == path.count - 1 {
                        return ReadDirectory(fromCluster: entry.FirstCluster)
                    }
                    folderEntry = entry
                    break
                }
            }
            entries = [DirectoryEntry]()
            if let folder = folderEntry {
                entries = ReadDirectory(fromCluster: folder.FirstCluster)
                i += 1
                folderEntry = nil
            }
        }
    }
    
    func WriteFile(fromPath: String, file: (Info: DirectoryEntry, Data: UnsafeMutableBufferPointer<UInt8>)) {
        let path = fromPath.split(separator: "/")
        var i = 0
        if path[i].count <= 1 {
            i += 1
        }
        if path[i] == "." {
            // Current Working Directory
            i += 1
        }
        var dirCluster = 0 as UInt32
        var entries = ReadDirectory(fromCluster: dirCluster)
        var folderEntry: DirectoryEntry?
        
        while true {
            if entries.count < 1 || i >= path.count {
                return
            }
            for entry in entries {
                guard entry.Flags != 0x00 && !entry.Flags.bit(3) && !entry.Flags.bit(0) else {
                    continue
                }
                if entry.Name + "." + entry.Extension == path[i] || CaseSensitive ? false : (entry.Name + "." + entry.Extension == path[i].uppercased()) {
                    WriteDirectory(fromCluster: dirCluster, entries: [file.Info])
                    return WriteFile(fromCluster: entry.FirstCluster, file: file.Data)
                }
                if (entry.Name == path[i] || (CaseSensitive ? false : entry.Name == path[i].uppercased())) && entry.Flags.bit(4) {
                    folderEntry = entry
                    break
                }
            }
            entries = [DirectoryEntry]()
            if let folder = folderEntry {
                dirCluster = folder.FirstCluster
                entries = ReadDirectory(fromCluster: dirCluster)
                i += 1
                folderEntry = nil
            }
        }
    }
}
