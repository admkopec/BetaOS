//
//  AHCI.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/25/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

class AHCI: PCIModule {
    let Name: String = "AHCI"
    static var CompatibleDevices: [PCIDevice] = []//[(0x15AD, 0x7E0)]
    static var CompatibleClasses: [PCIClass]? = [(0x01, 0x06)]
    
    var mem = UnsafeMutablePointer<HBA_MEM>(bitPattern: 0)
    
    var devices = [Disk]()
    
    var description: String {
        var str = "\(devices.count) Devices"
        for device in devices {
            str += "\n\t"
            str += device.description
        }
        return str
    }
    
    required init?(pci: PCI) {
        if pci.ProgIF != 0x1 {
            Log("Mass Storage device is not AHCI!", level: .Error)
            return nil
        }
        
        let addr = pci.GetBAR(BARNumber: 5)
        
        Log("Base Address: \(addr.address)", level: .Debug)

        mem = UnsafeMutablePointer<HBA_MEM>(bitPattern: addr.address.virtual)
        
        if mem!.pointee.ghc & 0x80000000 != 1 {
            Log("AHCI is not enabled, enabling...", level: .Debug)
            mem!.pointee.ghc |= 0x80000000
        }
        
        Log("Capabilities: \(String(mem!.pointee.cap, radix: 16)), Global Host Control: \(String(mem!.pointee.ghc, radix: 16))", level: .Debug)
        Log("Port implemented: \(String(mem!.pointee.pi, radix: 16))", level: .Debug)
        
        var ip = mem!.pointee.pi
        var i = 0
        
        while ip > 0 {
            if ip & 1 == 1 {
                let PortAddr = UInt(addr.address.virtual + 0x100 + 0x80 * UInt(i))
                let Port = UnsafeMutablePointer<HBA_PORT>(bitPattern: PortAddr)!
                
                if let device = SATADevice(port: Port, PortNumber: i, controller: self) {
                    devices.append(device)
                    if let gpt = GPT(disk: device) {
                        Log("\(gpt.description)", level: .Debug)
                    } else if let mbr = MBR(disk: device) {
                        Log("\(mbr.description)", level: .Debug)
                    }
                    device.PutDriveToSleep()
                }
            }
            ip >>= 1
            i   += 1
        }
    }
    
    final class SATADevice: Module, Disk {
        let Name = "SATADevice"
        var description: String {
            return "\(Model) (\(Size / 1024 / 1024 / 2)GB)"
        }
        
        let Controller: AHCI
        let Port: UnsafeMutablePointer<HBA_PORT>
        fileprivate(set) var CommandHeader: UnsafeMutablePointer<HBA_CMD_HEADER>
        fileprivate(set) var CommandTable: UnsafeMutablePointer<HBA_CMD_TBL>
        fileprivate(set) var Slot: Int // CommitCommand must be on a right slot which must correlate to CommandTable
        fileprivate(set) var Size  = 0
        fileprivate(set) var Model = ""
        
        init?(port: UnsafeMutablePointer<HBA_PORT>, PortNumber: Int, controller: AHCI) {
            Controller = controller
            Port = port
            Slot = 0
            CommandHeader = UnsafeMutablePointer<HBA_CMD_HEADER>(bitPattern: 0x1000)!
            CommandTable  = UnsafeMutablePointer<HBA_CMD_TBL>(bitPattern: 0x1001)!
            var command = 0xEC
            if Port.pointee.sig == 0x00000101 { // ATA Drive
                Log("Found SATA Drive at \(PortNumber)", level: .Debug)
                command = 0xEC
            } else if Port.pointee.sig == 0xEB140101 { //ATAPI Drive
                Log("Found SATAPI Drive at \(PortNumber)", level: .Debug)
                command = 0xA1
                return nil
            } else {
                return nil
            }
            // Identify drive
            Rebase(PortNumber: PortNumber)
            Slot = FindFreeCommandSlot()
            CommandHeader.advanced(by: Slot).pointee.cfl   = UInt8(MemoryLayout<FIS_REG_H2D>.size / MemoryLayout<UInt32>.size)
            CommandHeader.advanced(by: Slot).pointee.w     = 0
            CommandHeader.advanced(by: Slot).pointee.c     = 1
            CommandHeader.advanced(by: Slot).pointee.p     = 1
            CommandHeader.advanced(by: Slot).pointee.prdtl = 1
            let addressTable = Address(UInt(CommandHeader.advanced(by: Slot).pointee.ctba) + (UInt(CommandHeader.advanced(by: Slot).pointee.ctbau) << 32))
            Log("Address of Table = \(addressTable)", level: .Debug)
            CommandTable = UnsafeMutablePointer<HBA_CMD_TBL>(bitPattern: addressTable.virtual)!
            CommandTable.pointee.prdt_entry.dbc = 0x1ff
            CommandTable.pointee.prdt_entry.i   = 1
            let temp = UnsafeMutablePointer<UInt8>.allocate(capacity: 512)
            let tempPhys = kvtophys(UInt64(UInt(bitPattern: temp)))
            CommandTable.pointee.prdt_entry.dba  = UInt32(truncatingIfNeeded:  tempPhys)
            CommandTable.pointee.prdt_entry.dbau = UInt32(truncatingIfNeeded: (tempPhys >> 32))
            let fis = UnsafeMutablePointer<FIS_REG_H2D>.allocate(capacity: MemoryLayout<FIS_REG_H2D>.size)
            fis.pointee.fis_type = UInt8(FIS_TYPE_REG_H2D.rawValue)
            fis.pointee.pmport   = 0
            fis.pointee.c        = 1
            fis.pointee.command  = UInt8(command)
            fis.pointee.featurel = 1
            memcpy(&CommandTable.pointee.cfis.0, fis, MemoryLayout<FIS_REG_H2D>.size)
            if !CommitCommand(slot: Slot) {
                return //nil
            }
            let CommadnSets = ByteArray(withBytes: temp.advanced(by: 164).pointee, temp.advanced(by: 165).pointee, temp.advanced(by: 166).pointee, temp.advanced(by: 167).pointee).asInt
            if CommadnSets & (1 << 26) == 1 {
                Size = ByteArray(withBytes: temp.advanced(by: 200).pointee, temp.advanced(by: 201).pointee, temp.advanced(by: 202).pointee, temp.advanced(by: 203).pointee).asInt
            } else {
                Size = ByteArray(withBytes: temp.advanced(by: 120).pointee, temp.advanced(by: 121).pointee, temp.advanced(by: 122).pointee, temp.advanced(by: 123).pointee).asInt
            }
            
            var k = 0
            while k < 40 {
                Model.append(String(UnicodeScalar(temp.advanced(by: 54 + k + 1).pointee)))
                Model.append(String(UnicodeScalar(temp.advanced(by: 54 + k).pointee)))
                k += 2
            }
            Model = Model.trim()
        }
        
        func PutDriveToSleep() {
            let fis = UnsafeMutablePointer<FIS_REG_H2D>.allocate(capacity: MemoryLayout<FIS_REG_H2D>.size)
            fis.pointee.fis_type = UInt8(FIS_TYPE_REG_H2D.rawValue)
            fis.pointee.pmport   = 0
            fis.pointee.c        = 1
            fis.pointee.command = 0xE2
            memcpy(&CommandTable.pointee.cfis.0, fis, MemoryLayout<FIS_REG_H2D>.size)
            guard CommitCommand(slot: Slot) else {
                Log("Failed while putting drive to sleep!", level: .Error)
                return
            }
        }
        
        func CommitCommand(slot: Int) -> Bool { // Needs a little bit more refining
//            Port.pointee.serr = Port.pointee.serr
//            Port.pointee.is   = Port.pointee.is
//            if ((Port.pointee.is & ((1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26)|(1 << 24)|(1 << 23))) == 1) ||            ((Port.pointee.tfd & ((1 << 0)|(1 << 7))) == 1){
//                Port.pointee.cmd &= ~(1 << 0)
//                //Start|Command List Override|FIS Receive Enable
//                Port.pointee.cmd |= (1 << 0)|(1 << 3)|(1 << 4)
//                Port.pointee.is = Port.pointee.is
//            }
            var spin = 0
            while (Port.pointee.tfd & 0x88) == 1 && spin < 1000000 {
                spin += 1
            }
            if spin == 1000000 {
                Log("Port is Hung", level: .Error)
                return false
            }
            Port.pointee.ci = UInt32(1 << slot)
            while spin < 20 {
                hlt() // sleep(500) should be here but 1/60 sec works just fine now
                if ((Port.pointee.ci & (1 << slot)) == 0) && ((Port.pointee.tfd & (1 << 7)) != 1) {
                    break
                }
                if (Port.pointee.is & (1 << 30)) == 1 {
                    Log("Disk error", level: .Error)
                    return false
                }
                spin += 1
            }
            if spin == 20 {
                Log("Device is Hung!", level: .Error)
                return false
            }
            if Port.pointee.tfd & (1 << 0) == 1 {
                Log("Device Error", level: .Error)
                return false
            }
            if (Port.pointee.is & (1 << 30)) == 1 {
                Log("Disk error", level: .Error)
                return false
            }
//            for _ in 0 ... 1000 {
//                hlt()
//                if (Port.pointee.tfd & (1 << 7)) != 1 {
//                    break
//                }
//            }
//            if Port.pointee.tfd & (1 << 0) == 1 {
//                Log("Device Error", level: .Error)
//                return false
//            }
            return true
        }
        
        func read(lba: UInt64, count: Int) -> UnsafeMutableBufferPointer<UInt8> {
            let fis = UnsafeMutablePointer<FIS_REG_H2D>.allocate(capacity: MemoryLayout<FIS_REG_H2D>.size)
            let LBA = UnsafeMutableBufferPointer<UInt8>(start: UnsafeMutablePointer<UInt8>.allocate(capacity: 512 * count), count: 512 * count)
            let LBAPhys = kvtophys(UInt64(UInt(bitPattern: LBA.baseAddress)))
            CommandTable.pointee.prdt_entry.dba  = UInt32(truncatingIfNeeded:  LBAPhys)
            CommandTable.pointee.prdt_entry.dbau = UInt32(truncatingIfNeeded: (LBAPhys >> 32))
            fis.pointee.fis_type = UInt8(FIS_TYPE_REG_H2D.rawValue)
            fis.pointee.pmport   = 0
            fis.pointee.c        = 1
            fis.pointee.command  = 0x25
            fis.pointee.lba0     = UInt8(truncatingIfNeeded:   lba)
            fis.pointee.lba1     = UInt8(truncatingIfNeeded:  (lba >> 8))
            fis.pointee.lba2     = UInt8(truncatingIfNeeded:  (lba >> 16))
            fis.pointee.lba3     = UInt8(truncatingIfNeeded:  (lba >> 24))
            fis.pointee.lba4     = UInt8(truncatingIfNeeded:  (lba >> 32))
            fis.pointee.lba5     = UInt8(truncatingIfNeeded:  (lba >> 40))
            fis.pointee.device   = (1 << 6)
            fis.pointee.countl   =  UInt8(count) & 0xFF
            fis.pointee.counth   = (UInt8(count) >> 8) & 0xFF
            memcpy(&CommandTable.pointee.cfis.0, fis, MemoryLayout<FIS_REG_H2D>.size)
            guard CommitCommand(slot: Slot) else {
                Log("Couldn't read LBA \(lba)!", level: .Error)
                return LBA
            }
            return LBA
        }
        
        func write(lba: UInt64, count: Int, buffer: UnsafeMutablePointer<UInt8>) {
            let fis = UnsafeMutablePointer<FIS_REG_H2D>.allocate(capacity: MemoryLayout<FIS_REG_H2D>.size)
            let bufferPhys = kvtophys(UInt64(UInt(bitPattern: buffer)))
            CommandTable.pointee.prdt_entry.dba  = UInt32(truncatingIfNeeded:  bufferPhys)
            CommandTable.pointee.prdt_entry.dbau = UInt32(truncatingIfNeeded: (bufferPhys >> 32))
            fis.pointee.fis_type = UInt8(FIS_TYPE_REG_H2D.rawValue)
            fis.pointee.pmport   = 0
            fis.pointee.c        = 1
            fis.pointee.command  = 0x35
            fis.pointee.lba0     = UInt8(truncatingIfNeeded:  lba)
            fis.pointee.lba1     = UInt8(truncatingIfNeeded: (lba >> 8))
            fis.pointee.lba2     = UInt8(truncatingIfNeeded: (lba >> 16))
            fis.pointee.lba3     = UInt8(truncatingIfNeeded: (lba >> 24))
            fis.pointee.lba4     = UInt8(truncatingIfNeeded: (lba >> 32))
            fis.pointee.lba5     = UInt8(truncatingIfNeeded: (lba >> 40))
            fis.pointee.device   = (1 << 6)
            fis.pointee.countl   = UInt8(count) & 0xFF
            fis.pointee.counth   = UInt8(count >> 8) & 0xFF
            memcpy(&CommandTable.pointee.cfis.0, fis, MemoryLayout<FIS_REG_H2D>.size)
            guard CommitCommand(slot: Slot) else {
                Log("Couldn't write LBA \(lba)!", level: .Error)
                return
            }
        }
        
        fileprivate func FindFreeCommandSlot() -> Int {
            var slots = (Port.pointee.sact | Port.pointee.ci)
            for i in 0 ... 31 {
                if ((slots & 1) == 0) {
                    return i
                }
                slots >>= 1
            }
            Log("Cannot find free command list entry", level: .Error)
            return -1
        }
        
        fileprivate func Rebase(PortNumber: Int) {
            Controller.mem!.pointee.ghc = (1 << 31)
            Controller.mem!.pointee.ghc = (1 <<  0)
            Controller.mem!.pointee.ghc = (1 << 31)
            Controller.mem!.pointee.ghc = (1 <<  1)
            Port.pointee.cmd &= ~(1 << 0)
            while true {
                if (Port.pointee.cmd & (1 << 14)) == 1 {
                    continue
                }
                if (Port.pointee.cmd & (1 << 15)) == 1 {
                    continue
                }
                break
            }
            Port.pointee.cmd &= ~(1 << 4)
            Port.pointee.cmd = Port.pointee.cmd & 0xffff7fff; //Bit 15
            Port.pointee.cmd = Port.pointee.cmd & 0xffffbfff; //Bit 14
            Port.pointee.cmd = Port.pointee.cmd & 0xfffffffe; //Bit 0
            Port.pointee.cmd = Port.pointee.cmd & 0xfffffff7; //Bit 4
            let cmdBases = UInt64((40960) + (PortNumber << 13))
            var addr64 = 0 as UInt64
            let NewBase = UnsafeMutableRawPointer.allocate(bytes: Int(cmdBases + 8192), alignedTo: 8)
            let NewBasePhys = kvtophys(UInt64(UInt(bitPattern: NewBase)))
            addr64 = NewBasePhys + (UInt64(PortNumber) << 10)
            Port.pointee.clb  = UInt32(truncatingIfNeeded:  addr64)
            Port.pointee.clbu = UInt32(truncatingIfNeeded: (addr64 >> 32))
            addr64 = NewBasePhys + UInt64(32768) + (UInt64(PortNumber) << 8)
            Port.pointee.fb   = UInt32(truncatingIfNeeded:  addr64)
            Port.pointee.fbu  = UInt32(truncatingIfNeeded: (addr64 >> 32))
            Port.pointee.serr = 1
            Port.pointee.is   = 0
            Port.pointee.ie   = 1
            let address = Address(UInt(Port.pointee.clb) + (UInt(Port.pointee.clbu) << 32))
            Log("Address = \(address)", level: .Debug)
            CommandHeader = UnsafeMutablePointer<HBA_CMD_HEADER>(bitPattern: address.virtual)!
            for i in 0 ... 31 {
                CommandHeader.advanced(by: i).pointee.prdtl = 8
                addr64 = NewBasePhys + cmdBases + (UInt64(i) << 8)
                CommandHeader.advanced(by: i).pointee.ctba  = UInt32(truncatingIfNeeded:  addr64)
                CommandHeader.advanced(by: i).pointee.ctbau = UInt32(truncatingIfNeeded: (addr64 >> 32))
            }
            while (Port.pointee.cmd & (1 << 15)) == 1 { }
            Port.pointee.cmd |= (1 << 4)
            Port.pointee.cmd |= (1 << 0)
            Port.pointee.is   = 0
            Port.pointee.ie   = 0xffffffff
        }
    }
}
