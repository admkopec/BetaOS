//
//  MCFG.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//import Addressing
import Loggable

struct MCFG: Loggable, ACPITable {
    let Name = "MCFG"
    var Header: SDTHeader
    
    var description: String {
        return "MCFG: \(Header)"
    }
    
    init(ptr: Address) {
        Header = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: ptr.virtual))!)
    }
}
