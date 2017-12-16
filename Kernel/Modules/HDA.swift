//
//  HDA.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/8/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct HDA: PCIModule {
    var Name: String = "HDA"
    var description: String {
        return Name
    }
    static var CompatibleDevices: [PCIDevice] = [(0x8086, 0x2668), (0x8086, 0x27D8), (0x1002, 0x4383)]
    
    init?(pci: PCI) {
        Log("Found Intel HDA", level: .Verbose)
    }
}
