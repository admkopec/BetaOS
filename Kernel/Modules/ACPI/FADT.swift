//
//  FADT.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/31/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct FADT: Loggable, ACPITable {
    let Name = "FADT"
    var Header: SDTHeader
    
    fileprivate let tablePointer: UnsafeMutablePointer<ACPIFADT>
    
    // IA-PC Boot Architecture Flags (bit)
    fileprivate let IAPC_LEGACY_DEVICES     = 0
    fileprivate let IAPC_8042               = 1
    fileprivate let IAPC_VGA_NOT_PRESENT    = 2
    fileprivate let IAPC_MSI_NOT_SUPPORTED  = 3
    fileprivate let IAPC_PCIE_ASPM          = 4
    fileprivate let IAPC_RTC_NOT_PRESENT    = 5
    
    var description: String {
        return "FADT: \(Header) hasLegacyDevices: \(hasLegacyDevices) has8042Controller: \(has8042Controller) isVGAPresent: \(isVGAPresent)" + "\n" + "isMSISupported: \(isMSISupported) canEnablePCIEASPMControls: \(canEnablePCIEASPMControls) hasCmosRtc: \(hasCmosRtc)"
    }
    
    var hasLegacyDevices: Bool {
        return tablePointer.pointee.iapc_boot_arch.bit(IAPC_LEGACY_DEVICES)
    }
    
    var has8042Controller: Bool {
        return tablePointer.pointee.iapc_boot_arch.bit(IAPC_8042)
    }
    
    var isVGAPresent: Bool {
        return tablePointer.pointee.iapc_boot_arch.bit(IAPC_VGA_NOT_PRESENT) == false
    }
    
    var isMSISupported: Bool {
        return tablePointer.pointee.iapc_boot_arch.bit(IAPC_MSI_NOT_SUPPORTED) == false
    }
    
    var canEnablePCIEASPMControls: Bool {
        return tablePointer.pointee.iapc_boot_arch.bit(IAPC_PCIE_ASPM) == false
    }
    
    var hasCmosRtc: Bool {
        return tablePointer.pointee.iapc_boot_arch.bit(IAPC_RTC_NOT_PRESENT) == false
    }
    
    let DSDTAddress: Address
    let FACSAddress: Address
    
    
    init(ptr: Address) {
        Header = SDTHeader(ptr: (UnsafeMutablePointer<ACPISDTHeader_t>(bitPattern: ptr.virtual))!)
        tablePointer = UnsafeMutablePointer<ACPIFADT>(bitPattern: ptr.virtual)!
        if tablePointer.pointee.header.Length >= 44  {
            DSDTAddress = Address(tablePointer.pointee.dsdt, baseAddress: ptr.baseAddr)
            FACSAddress = Address(tablePointer.pointee.firmware_ctrl, baseAddress: ptr.baseAddr)
        } else {
            DSDTAddress = 0
            FACSAddress = 0
            Log("Couldn't find DSDT Address!", level: .Error)
        }
        Log("FADT: \(ptr), DSDT: \(DSDTAddress)", level: .Debug)
        Log("XDSDT: 0x\(String(tablePointer.pointee.x_dsdt, radix: 16)), DSDT: 0x\(String(tablePointer.pointee.dsdt, radix: 16))", level: .Debug)
        Log("XFACS: 0\(String(tablePointer.pointee.x_firmware_ctrl, radix: 16)), FACS: 0x\(String(tablePointer.pointee.firmware_ctrl, radix: 16))", level: .Debug)
    }
}
