//
//  exFAT.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/3/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

class exFAT: Partition {
    let VolumeName: String
    let AlternateName: String
    
    
    init?(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        // To be implemented
        VolumeName = "exFAT"
        AlternateName = "exFAT"
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
