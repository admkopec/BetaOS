//
//  kernel.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//TODO:
// * Add APIC and enable IRQs   In progress.... (LAPIC Timer - working, PIC & PIT - working, Rest - sort of…)
// * Change pollchar()          Done (for now) Actually changed getchar() (Added an asm("hlt") instruction, so it waits for an interrupt)
// * Add PCI driver             Partialy done
// * Add SATA driver            Several tests have been conducted 🙂
// * Add Filesystem driver
// * Add ACPI                   Done (for now) (Adding more right now 🙂)

@_silgen_name("kernelMain")
public func kernelMain() -> Void {
    // New kernelMain written in Swift
    PIT8254.sharedInstance.setChannel(.CHANNEL_0, mode: .MODE_3, hz: 60)
    ModulesStartController()
    CommandInit()
    kprint("Hello from Swift!" + "\n" + "Welcome to BetaOS in x86_64 MACH-O EFI mode :) !")
    let username = "root"
    _ = System.sharedInstance
    if System.sharedInstance.DeviceVendor == "Apple Inc." {
        while true {
            hlt()
        }
    } else {
        while true {
            kprint(username + "$ ", terminator: "")
            findcommand()
        }
    }
}
