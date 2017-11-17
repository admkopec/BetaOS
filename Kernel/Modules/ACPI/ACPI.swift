//
//  ACPI.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/14/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

protocol ACPITable {
    //
}

struct ACPI: Module {
    let Name = "ACPI"
    private(set) var tables: [ACPITable] = []
    var description: String {
        return Name + " " + "Module"
    }
    
    init(rsdp: RSDP) {
        let RSDT = ACPIHeader(ptr: UnsafePointer(bitPattern: rsdp.RSDT)!)
        kprint("\(rsdp)")
        kprint("\(RSDT)")
        if RSDT.Length < (0x1000 - 0x68) {
            kprint("It'll fit: \(RSDT.Length)")
        } else {
            kprint("It won't fit: \(RSDT.Length)")
        }
    }
}

struct ACPIHeader: CustomStringConvertible {
    let Signature:  String
    let Length:     UInt32
    let Revision:   UInt8
    let Checksum:   UInt8
    let OemID:      String
    let OemTableID: String
    let OemRev:     UInt32
    let CreatorID:  String
    let CreatorRev: UInt32
    
    var description: String {
        return "ACPI: \(Signature): \(OemID): \(CreatorID): \(OemTableID): rev: \(Revision)"
    }
    
    init(ptr: UnsafePointer<ACPISDTHeader_t>) {
        let CCreatorID = [CChar(ptr.pointee.CreatorID.0), CChar(ptr.pointee.CreatorID.1), CChar(ptr.pointee.CreatorID.2), CChar(ptr.pointee.CreatorID.3), CChar(0)]
        let COEMTableID = [CChar(ptr.pointee.OEMTableID.0), CChar(ptr.pointee.OEMTableID.1), CChar(ptr.pointee.OEMTableID.2), CChar(ptr.pointee.OEMTableID.3), CChar(ptr.pointee.OEMTableID.4), CChar(ptr.pointee.OEMTableID.5), CChar(ptr.pointee.OEMTableID.6), CChar(ptr.pointee.OEMTableID.7), CChar(0)]
        let COEMID = [CChar(ptr.pointee.OEMID.0), CChar(ptr.pointee.OEMID.1), CChar(ptr.pointee.OEMID.2), CChar(ptr.pointee.OEMID.3), CChar(ptr.pointee.OEMID.4), CChar(ptr.pointee.OEMID.5), CChar(0)]
        Signature  = String(ptr, maxLength: 4)
        Length     = ptr.pointee.Length
        Revision   = ptr.pointee.Revision
        Checksum   = ptr.pointee.Checksum
        OemID      = String(cString: COEMID)
        OemTableID = String(cString: COEMTableID)
        OemRev     = ptr.pointee.OEMRevision
        CreatorID  = String(cString: CCreatorID)
        CreatorRev = ptr.pointee.CreatorRevision
    }
}
