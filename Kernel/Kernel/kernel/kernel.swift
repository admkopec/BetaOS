//
//  kernel.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//TODO:
// * Add APIC and enable IRQs   In progress.... (Timer - working, Rest - sort of…)
// * Change pollchar() (start from waiting for IRQ 1 instead of polling)
// * Add PCI driver             Partialy done
// * Add SATA driver            Several tests have been conducted :)
// * Add Filesystem driver
// * Add ACPI                   Done (for now)

@_silgen_name("kernelMain")
func kernelMain() -> Void {
    // New kernelMain written in Swift
    ModulesStartController()
    CommandInit()
    kprint("Hello from Swift!" + "\n" + "Welcome to BetaOS in x86_64 MACH-O EFI mode :) !")
    let username = "root"
    if !VendorisApple {
        while true {
            kprint(username + "$ ", terminator: "")
            findcommand()
        }
    } else {
        hlt()
    }
}
