//
//  MADT.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct MADT: Loggable, ACPITable {
    let Name = "MADT"
    var Header: SDTHeader
    fileprivate let tablePointer: UnsafeMutablePointer<ACPIMADT>
    fileprivate let dataLength: Int
    fileprivate(set) var madtEntries: [MADTEntry] = []
    
    let LocalInterruptControllerAddress: Address
    
    var HasCompatDual8259: Bool {
        return tablePointer.pointee.MultipleAPICFlags.bit(0)
    }
    
    var description: String {
        return "MADT: \(Header)"
    }
    
    init(ptr: Address) {
        Header = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: ptr.virtual))!)
        tablePointer = UnsafeMutablePointer<ACPIMADT>(bitPattern: ptr.virtual)!
        dataLength = Int(Header.Length) - MemoryLayout<ACPIMADT>.size
        LocalInterruptControllerAddress = Address(tablePointer.pointee.LocalInterruptControllerAddress)
        Log("Local Interrupt Controller Address = \(LocalInterruptControllerAddress)", level: .Info)
        Log("HasCompatDual8259 = \(HasCompatDual8259)", level: .Info)
        guard dataLength >= 2 else {
            Log("dataLength is less than 2!", level: .Error)
            return
        }
        madtEntries = decodeEntries()
        for entry in madtEntries {
            Log("Entry: \(entry.debugDescription)", level: .Debug)
        }
    }
    
    fileprivate func decodeEntries() -> [MADTEntry] {
        return (tablePointer + 1).withMemoryRebound(to: UInt8.self,
                                     capacity: dataLength) {
                                        var entries: [MADTEntry] = []
                                        let controllers = UnsafeBufferPointer(start: $0,
                                                                              count: dataLength)
                                        var position = 0
                                        
                                        while position < controllers.count {
                                            let bytesRemaining = controllers.count - position
                                            guard bytesRemaining > 2 else {
                                                fatalError("error: bytesRemaining: \(bytesRemaining) "
                                                    + "count: \(controllers.count) position: \(position)")
                                            }
                                            let tableLen = Int(controllers[position + 1])
                                            guard tableLen > 0 && tableLen <= controllers.count - position
                                                else {
                                                    fatalError("error: tableLen: \(tableLen) "
                                                        + "position: \(position) "
                                                        + "controllers.count: \(controllers.count)")
                                            }
                                            
                                            let start: UnsafeMutablePointer<UInt8> = $0.advanced(by: position)
                                            let tableData = UnsafeBufferPointer(start: start,
                                                                                count: tableLen)
                                            let table = decodeTable(table: tableData)
                                            entries.append(table)
                                            position += tableLen
                                        }
                                        return entries
        }
    }
    
    fileprivate func decodeTable(table: UnsafeBufferPointer<UInt8>) -> MADTEntry {
        guard let type = IntControllerTableType(rawValue: table[0]) else {
            Log("Unknown MADT entry: \(String(table[0], radix: 16))", level: .Error)
            fatalError()
        }
        switch type {
        case .processorLocalApic:
            return ProcessorLocalApicTable(table: table)
            
        case .ioApic:
            return IOApicTable(table: table)
            
        case .interruptSourceOverride:
            return InterruptSourceOverrideTable(table: table)
            
        case .localApicNmi:
            return LocalApicNmiTable(table: table)
            
        default:
            Log("\(type): unsupported", level: .Error)
            fatalError()
        }
    }
    
    //
    // Tables:
    //
    
    struct ProcessorLocalApicTable: MADTEntry {
        let tableType = IntControllerTableType.processorLocalApic
        let tableLength = 8
        let processorUID: UInt8
        let apicID: UInt8
        let localApicFlags: UInt32
        var enabled: Bool { return localApicFlags.bit(0) }
        var debugDescription: String {
            return "\(tableType): uid: \(String(processorUID, radix: 16)) apicID: \(String(apicID, radix: 16)) flags: \(String(localApicFlags, radix: 16)) enabled: \(enabled ? "Yes" : "No")"
        }
        
        
        fileprivate init(table: UnsafeBufferPointer<UInt8>) {
            guard table.count == tableLength else {
                fatalError("Invalid ProcessorLocalApic size")
            }
            processorUID = table[2]
            apicID = table[3]
            // ACPI tables are all little endian
            localApicFlags = UInt32(withBytes: table[4], table[5],
                                    table[6], table[7]);
        }
    }
    
    
    struct IOApicTable: MADTEntry {
        let tableType = IntControllerTableType.ioApic
        let tableLength = 12
        let ioApicID: UInt8
        let ioApicAddress: UInt32
        let globalSystemInterruptBase: UInt32
        var debugDescription: String {
            let desc: String = String(describing: tableType)
                + ": APIC ID: \(String(ioApicID, radix: 16)) "
                + "Addr: \(String(ioApicAddress, radix: 16)) "
                + "Interrupt Base: \(String(globalSystemInterruptBase, radix: 16))"
            return desc
        }
        
        
        fileprivate init(table: UnsafeBufferPointer<UInt8>) {
            guard table.count == tableLength else {
                fatalError("Invalid IOApicTable size")
            }
            ioApicID = table[2]
            ioApicAddress = UInt32(withBytes: table[4], table[5],
                                   table[6], table[7]);
            globalSystemInterruptBase = UInt32(withBytes: table[8], table[9],
                                               table[10], table[11])
        }
    }
    
    
    struct InterruptSourceOverrideTable: MADTEntry {
        
        let tableType = IntControllerTableType.interruptSourceOverride
        let tableLength = 10
        let bus: UInt8
        let sourceIRQ: UInt8
        let globalInterrupt: UInt32
        let flags: UInt16
        var debugDescription: String {
            return "\(tableType): bus: \(String(bus, radix: 16)) irq: \(String(sourceIRQ, radix: 16)) globalInterrupt: \(String(globalInterrupt, radix: 16)) flags: \(String(flags, radix: 16))"
        }
        
        
        fileprivate init(table: UnsafeBufferPointer<UInt8>) {
            guard table.count == tableLength else {
                fatalError("Invalid InterruptSourceOverrideTable size")
            }
            bus = table[2]
            sourceIRQ = table[3]
            globalInterrupt = UInt32(withBytes: table[4], table[5],
                                     table[6], table[7]);
            flags = UInt16(withBytes: table[8], table[9])
        }
    }
    
    
    struct LocalApicNmiTable: MADTEntry {
        let tableType = IntControllerTableType.localApicNmi
        let tableLength = 6
        let acpiProcessorUID: UInt8
        let flags: UInt16
        let localApicLint: UInt8
        var debugDescription: String {
            return "\(tableType): processor UID: \(String(acpiProcessorUID, radix: 16)) flags: \(String(flags, radix: 16)) LINT# \(String(localApicLint, radix: 16))"
        }
        
        
        fileprivate init(table: UnsafeBufferPointer<UInt8>) {
            guard table.count == tableLength else {
                fatalError("Invalid LocalApicNmiTable size")
            }
            acpiProcessorUID = table[2]
            flags = UInt16(withBytes: table[3], table[4])
            localApicLint = table[5]
        }
    }
}

enum IntControllerTableType: UInt8 {
    case processorLocalApic             = 0x00
    case ioApic                         = 0x01
    case interruptSourceOverride        = 0x02
    case nmiSource                      = 0x03
    case localApicNmi                   = 0x04
    case localApicAddressOverride       = 0x05
    case ioSapic                        = 0x06
    case localSapic                     = 0x07
    case platformInterruptSources       = 0x08
    case processorLocalx2Apic           = 0x09
    case localx2ApicNmi                 = 0x0A
    case gicCPUInterface                = 0x0B
    case gicDistributor                 = 0x0C
    case gicMsiFrame                    = 0x0D
    case gicRedistributor               = 0x0E
    case gicInterruptTranslationService = 0x0F
    // 0x10 - 0x7F are reserved, 0x80-0xFF are for OEM use so treat as
    // invalid for now
}


protocol MADTEntry: CustomDebugStringConvertible {
    var tableType: IntControllerTableType { get }
}
