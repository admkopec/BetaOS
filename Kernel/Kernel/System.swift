//
//  System.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import Loggable

final class System: Loggable {
    static let sharedInstance = System()
    let Name: String = "System"
    let modulesController = ModulesController()
    fileprivate(set) var interruptManager:  InterruptManager!
    fileprivate(set) var Timer: PIT8254?
    
    fileprivate(set) var DeviceVendor  = "Generic"
    fileprivate(set) var DeviceName    = "Generic Device"
    fileprivate(set) var DeviceID      = "Generic1,1"
    fileprivate(set) var SerialNumber  = "000000000000"
    
    internal var Video: VideoModule = VESA()
    internal var Drives = [PartitionTable]()
    
    internal var ACPI: ACPI? {
        return modulesController.modules.filter { $0 is ACPI }.first as? ACPI
    }
    
    internal var SMBIOS: SMBIOS? {
        return modulesController.modules.filter { $0 is SMBIOS }.first as? SMBIOS
    }
    
    func initialize() {
        modulesController.initialize()
        interruptManager  = InterruptManager(acpiTables: ACPI)
        DeviceVendor = SMBIOS?.SystemVendor       ?? DeviceVendor
        DeviceName   = SMBIOS?.ProductDisplayName ?? DeviceName
        DeviceID     = SMBIOS?.ProductName        ?? DeviceID
        SerialNumber = SMBIOS?.ProductSerial      ?? SerialNumber
        interruptManager.enableIRQs()
        Timer = PIT8254()
        if DeviceID == "VMware7,1" {
            modulesController.modules.append(VMwareTools())
        }
        modulesController.companionController = PCIModulesController()
    }
    
    internal func shutdown() {
        modulesController.stop()
        shutdown_system()
    }
}

public func NullHandler(irq: Int) {
    
}
