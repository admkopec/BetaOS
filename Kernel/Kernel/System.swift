//
//  System.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

final class System: Loggable {
    static let sharedInstance = System()
    let Name: String = "System"
    let modulesController: ModulesController
    
    fileprivate(set) var DeviceVendor  = "Generic"
    fileprivate(set) var DeviceName    = "Generic Device"
    fileprivate(set) var DeviceID      = "Generic1,1"
    fileprivate(set) var SerialNumber  = "000000000000"
    
    static fileprivate let vesa = VESA()
    
    internal var Video: Video {
        if let i = modulesController.modules.index(where: {$0 is Video}) {
            return (modulesController.modules[i] as! Video)
        } else {
            return System.vesa
        }
    }
    
    internal var ACPI: ACPI? {
        if let i = modulesController.modules.index(where: {$0 is ACPI}) {
            return (modulesController.modules[i] as! ACPI)
        } else {
            return nil
        }
    }
    
    internal var SMBIOS: SMBIOS? {
        if let i = modulesController.modules.index(where: {$0 is SMBIOS}) {
            return (modulesController.modules[i] as! SMBIOS)
        } else {
            return nil
        }
    }
    
    init() {
        modulesController = ModulesController()
        DeviceVendor = SMBIOS?.SystemVendor         ?? DeviceVendor
        DeviceName   = SMBIOS?.ProductDisplayName   ?? DeviceName
        DeviceID     = SMBIOS?.ProductName          ?? DeviceID
        SerialNumber = SMBIOS?.ProductSerial        ?? SerialNumber
    }
}
