//
//  ModulesController.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/16/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import Loggable

typealias PCIDevice = (VendorID: UInt16, DeviceID: UInt16)
typealias PCIClass  = (Class: UInt8, Subclass: UInt8)

protocol Module: Loggable, CustomStringConvertible { }

protocol PCIModule: Module, AnyObject {
    static var CompatibleDevices: [PCIDevice] { get }
    static var CompatibleClasses: [PCIClass]? { get }
    init?(pci: PCI)
}

protocol VideoModule: Module, AnyObject {
    var mainView: MainView { get set }
    
    func refresh() -> Void
}

final class ModulesController: CustomStringConvertible {
    let Name: String = "ModulesController"
    var companionController: PCIModulesController? = nil
    internal var modules = [Module]()
    
    var description: String {
        var retValue = "Swift Modules Controller - loaded modules:"
        for module in modules {
            retValue += "\n" + module.Name + ": " + module.description
        }
        return retValue
    }
    
    func initialize() {
        if let acpi = ACPI(rsdp: RSDP(structure: RSDP_)) {
            modules.append(acpi)
        }
        if let smbios = SMBIOS(structure: SMBIOS_) {
            modules.append(smbios)
        }
    }
    
    func stop() {
        companionController = nil
        modules = []
    }
}



final class PCIModulesController: CustomStringConvertible {
    let Name = "PCIModulesController"
    fileprivate var controllers: [PCIModule.Type] = [SVGA.self, AHCI.self, NVMe.self, E1000.self, HDA.self]
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
                            if controller.CompatibleDevices.count > 0 {
                                for comptaibleDevice in controller.CompatibleDevices {
                                    if (pci.VendorID, pci.DeviceID) == comptaibleDevice {
                                        if let module = controller.init(pci: pci) {
                                            System.sharedInstance.modulesController.modules.append(module)
                                        }
                                    }
                                }
                            } else {
                                if controller.CompatibleClasses != nil {
                                    for compatibleClass in controller.CompatibleClasses! {
                                        if (pci.Class, pci.Subclass) == compatibleClass {
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
    }
}

