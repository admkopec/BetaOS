//
//  NVMe.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/6/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

class NVMe: PCIModule {
    let Name: String = "NVMe"
    static var CompatibleDevices: [PCIDevice] = [PCIDevice]()
    static var CompatibleClasses: [PCIClass]? = [(Class: 0x1, Subclass: 0x8)]
    var description: String {
        return "NVM Express"
    }
    
    fileprivate var BAR = 0 as Address
    
    required init?(pci: PCI) {
        if pci.ProgIF != 0x2 {
            Log("Non-Volatile Memory Controller device is not NVMe!", level: .Error)
            return nil
        }
        BAR = pci.GetBAR(BARNumber: 0).address
        let buffer = MemoryBuffer(BAR.virtual, size: 0x40)
        guard let capabilities: UInt64 = /*try?*/ buffer.readAtIndex(0) else { return nil }
        guard let version: UInt32 = /*try?*/ buffer.readAtIndex(8) else { return nil }
        Log("Version is \(String(version, radix: 16))", level: .Debug)
        Log("Capabilities: \(String(capabilities, radix: 16))", level: .Debug)
    }
}
