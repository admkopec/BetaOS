//
//  SDTHeader.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

struct SDTHeader: CustomStringConvertible {
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
        return "\(Signature): \(OemID): \(CreatorID): \(OemTableID): rev: \(Revision)"
    }
    
    init(ptr: UnsafeMutablePointer<ACPISDTHeader_t>) {
        Signature  = String(&ptr.pointee.Signature.0, maxLength: 4)
        Length     = ptr.pointee.Length
        Revision   = ptr.pointee.Revision
        Checksum   = ptr.pointee.Checksum
        OemID      = String(&ptr.pointee.OEMID.0, maxLength: 6)
        OemTableID = String(&ptr.pointee.OEMTableID.0, maxLength: 8)
        OemRev     = ptr.pointee.OEMRevision
        CreatorID  = String(&ptr.pointee.CreatorID, maxLength: 4)
        CreatorRev = ptr.pointee.CreatorRevision
    }
}
