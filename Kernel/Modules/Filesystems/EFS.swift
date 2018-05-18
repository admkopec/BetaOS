//
//  EFS.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/30/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

final class EFS: FAT32 {
    override init(partitionEntry: GPTPartitionEntry, onDisk disk: Disk) {
        super.init(partitionEntry: partitionEntry, onDisk: disk)!
        guard Header.BytesPerSector == 512 else {
            kprint("Sector sizes do not match! Can't support that yet")
            return
        }
    }
}
