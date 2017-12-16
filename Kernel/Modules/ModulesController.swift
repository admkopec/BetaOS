//
//  ModulesController.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

typealias PCIDevice = (VendorID: UInt16, DeviceID: UInt16)

protocol Module: Loggable, CustomStringConvertible {
    
}

protocol PCIModule: Module {
    static var CompatibleDevices: [PCIDevice] { get }
    init?(pci: PCI)
}

final class ModulesController: CustomStringConvertible {
    let Name: String = "ModulesController"
    let companionController: PCIModulesController
    fileprivate(set) var modules = [Module]()
    
    var description: String {
        var retValue = "Swift Modules Controller - loaded modules:"
        for module in modules {
            retValue += "\n" + module.description
        }
        return retValue
    }
    
    init() {
        if let acpi = ACPI(rsdp: RSDP(structure: RSDP_)) {
            modules.append(acpi)
        }
        
        if let smbios = SMBIOS(structure: SMBIOS_) {
            modules.append(smbios)
        }
        companionController = PCIModulesController()
    }
}



final class PCIModulesController: CustomStringConvertible {
    let Name = "PCIModulesController"
    fileprivate var controllers: [PCIModule.Type] = [SVGA.self, HDA.self]
    var description: String {
        return Name
    }
    
    init() {
        for bus in 0 ... 255 {
            var pci = PCI(bus: UInt8(bus))
            if !pci.isValid {
                continue
            }
            for slot in 0 ... 31 {
                for function in 0 ... 7 {
                    pci = PCI(bus: UInt8(bus), slot: UInt8(slot), function: UInt8(function))
                    if pci.isValid {
                        for controller in controllers {
                            for comptaibleDevice in controller.CompatibleDevices {
                                if (pci.VendorID, pci.DeviceID) == comptaibleDevice {
                                    if let module = controller.init(pci: pci) {
                                        System.sharedInstance.modulesController.modules.append(module)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
