//
//  ACPI.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/14/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import Addressing

protocol ACPITable: CustomStringConvertible {
    var Header: SDTHeader { get }
}

struct ACPI: Module {
    let Name = "ACPI"
    fileprivate(set) var globalObjects: GlobalObjects!
    fileprivate(set) var tables = [ACPITable]()
    fileprivate      var ssdts  = [SSDT]()
    var description: String {
        return "Module"
    }
    
    init?(rsdp: RSDP?) {
        if rsdp == nil {
            Log("RSDP == nil", level: .Error)
            return nil
        }
        Log("\(rsdp!.description)", level: .Verbose)
        tables.append(RSDT(address: rsdp!.RSDT))
        var Dsdt: SSDT?
        for _addr in (tables[0] as! RSDT).SDTAddresses {
            let addr = Address(_addr.physical, baseAddress: _addr.baseAddr)
//            let addr = _addr
            guard let ptr = UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: addr.virtual) else { continue }
            let SDT = SDTHeader(ptr: ptr)
            switch SDT.Signature {
            case "MCFG":
                tables.append(MCFG(ptr: addr))
            case "FACP":
                let fadt = FADT(ptr: addr)
                tables.append(fadt)
                if fadt.DSDTAddress != 0 {
                    Dsdt = SSDT(ptr: fadt.DSDTAddress)
                    tables.append(Dsdt!)
                }
                if fadt.FACSAddress != 0 {
//                    tables.append(FACS(ptr: fadt.FACSAddress))
                }
            case "SSDT":
                ssdts.append(SSDT(ptr: addr))
            case "APIC":
                tables.append(MADT(ptr: addr))
            case "HPET":
                tables.append(HPET(ptr: addr))
            default:
                Log("Unsupported Table: \(SDT.Signature), skipping...", level: .Warning)
            }
        }
        if let dsdt = Dsdt {
            ssdts.insert(dsdt, at: 0)
        }
        for table in tables {
            Log("\(table.description)", level: .Verbose)
        }
        let acpiGlobalObjects = GlobalObjects()
        let parser = AMLParser(globalObjects: acpiGlobalObjects)
        do {
            for ssdt in ssdts {
//                try parser.parse(amlCode: ssdt.AMLCode)   // FIXME: Fix AML Parser (Fix Double Faults)
            }
        } catch {
            Log("Parser Error: \(error)", level: .Error)
        }
        ssdts.removeAll()
        globalObjects = parser.acpiGlobalObjects
    }
}
