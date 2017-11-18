//
//  ACPI.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/14/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//import Addressing

protocol ACPITable: CustomStringConvertible {
    var Header: SDTHeader { get }
}

struct ACPI: Module {
    let Name = "ACPI"
    fileprivate(set) var tables = [ACPITable]()
    var description: String {
        return Name + " " + "Module"
    }
    
    init?(rsdp: RSDP?) {
        if rsdp == nil {
            Log("RSDP == nil", level: .Error)
            return nil
        }
        Log("\(rsdp!.description)", level: .Verbose)
        tables.append(RSDT(address: rsdp!.RSDT))
        for _addr in (tables[0] as! RSDT).SDTAddresses {
            let addr = Address(_addr.physical, baseAddress: _addr.baseAddr)
//            let addr = _addr
            
            let SDT = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: addr.virtual))!)
            switch SDT.Signature {
            case "MCFG":
                tables.append(MCFG(ptr: addr))
            case "FACP":
                tables.append(FADT(ptr: addr))
            case "APIC":
                tables.append(MADT(ptr: addr))
            case "HPET":
                tables.append(HPET(ptr: addr))
            default:
                Log("Unsupported Table: \(SDT.Signature), skipping...", level: .Warning)
            }
        }
        for table in tables {
            Log("\(table.description)", level: .Verbose)
        }
    }
}
