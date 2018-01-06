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
    lapic_init()
    APICInit()
    ModulesStartController()
    CommandInit()
    PIT8254.sharedInstance.setChannel(.CHANNEL_0, mode: .MODE_3, hz: 60)
//    let fontcolor = get_font_color()
//    let CopyrightNoticeColor = Color(red: 0x16, green: 0xF3, blue: 0xFF, alpha: 0xFF)
//    change_font_color(CopyrightNoticeColor.value)
    kprint("Hello from Swift!" + "\n" + "Welcome to BetaOS in x86_64 MACH-O EFI mode :) !")
    kprint("Copyright © 2015-2018 Adam Kopec. All rights reserved.")
//    change_font_color(fontcolor)
    _ = System.sharedInstance
    System.sharedInstance.initializePCIDevices()
//    _ = addTask(name: "DummyTaskA", task: dummyA)
//    _ = addTask(name: "DummyTaskB", task: dummyB)
//    _ = addTask(name: "Console", task: console)
//    saveKernelState()
//    experimental = false
//    runFirstTask()
    if System.sharedInstance.DeviceVendor == "Apple Inc." {
        while true {
            hlt()
        }
    } else {
        console()
    }
}

func dummyA() {
    while true {
        tprint("A")
        yield()
    }
}

func dummyB() {
    while true {
        tprint("B")
        yield()
    }
}

func console() -> Void {
    while true {
        kprint("root$ ", terminator: "")
        findcommand()
    }
}
