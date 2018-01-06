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
    func ReadDirectory(fromCluster: UInt32) -> [DirectoryEntry]
    func WriteDirectory(fromCluster: UInt32, entries: [DirectoryEntry])
    func ReadFile(fromCluster: UInt32) -> UnsafeMutableBufferPointer<UInt8>?
    func WriteFile(fromCluster: UInt32, file: UnsafeMutableBufferPointer<UInt8>)
}
extension Partition {
    func ReadFile(fromPath: String) -> (Info: DirectoryEntry, Data: UnsafeMutableBufferPointer<UInt8>)? {
        let path = fromPath.split(separator: "/")
        
        var i = 0
        if path[i].count <= 1 {
            i += 1
        }
        var entries = ReadDirectory(fromCluster: 0)
        var folderEntry: DirectoryEntry?
        
        while true {
            if entries.count < 1 {
                return nil
            }
            for entry in entries {
                if entry.Name + "." + entry.Extension == path[i] {
                    if let file = ReadFile(fromCluster: entry.FirstCluster) {
                        return (entry, file)
                    }
                    break
                }
                if entry.Name == path[i] && entry.Extension == " " {
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
        let path = fromPath.split(separator: "/")
        var i = 0
        if path[i].count <= 1 {
            i += 1
        }
        var entries = ReadDirectory(fromCluster: 0)
        var folderEntry: DirectoryEntry?
        while true {
            if entries.count < 1 {
                return nil
            }
            for entry in entries {
                if entry.Name == path[i] && entry.Extension == " " {
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
        var dirCluster = 0 as UInt32
        var entries = ReadDirectory(fromCluster: dirCluster)
        var folderEntry: DirectoryEntry?
        
        while true {
            if entries.count < 1 {
                return
            }
            for entry in entries {
                if entry.Name + "." + entry.Extension == path[i] {
                    WriteDirectory(fromCluster: dirCluster, entries: [file.Info])
                    return WriteFile(fromCluster: entry.FirstCluster, file: file.Data)
                }
                if entry.Name == path[i] && entry.Extension == " " {
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
