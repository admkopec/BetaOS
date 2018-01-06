//
//  DSDT.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/29/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct DSDT: Loggable, ACPITable {
    let Name = "DSDT"
    var Header: SDTHeader
    var description: String {
        return "DSDT: \(Header)"
    }
    init(ptr: Address) {
        Header = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: ptr.virtual))!)
    }
}
