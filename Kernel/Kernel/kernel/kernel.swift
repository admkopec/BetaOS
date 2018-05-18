//
//  kernel.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

//TODO:
// * Add APIC and enable IRQs   In progress.... (LAPIC Timer - working, PIC & PIT - working, Rest - sort of…)
// * Change pollchar()          Done (changed getchar() (Added an asm("hlt") instruction, so it waits for an interrupt))
// * Add PCI driver             Partialy done
// * Add SATA driver            Done (might need some correction for specific chipsets)
// * Add Filesystem driver      Done (needs some finishing touches)
// * Add ACPI                   Done (for now) (Adding more right now 🙂)
// * Fix new Exclusivity Checks

@_silgen_name("kernelMain")
public func kernelMain() -> Void {
    // New kernelMain written in Swift
    if !VendorisApple {
        ModulesStartController() // MacBook fails on XHCI
    }
    CommandInit()
    _ = System.sharedInstance
//    beforeInited = false
    System.sharedInstance.initialize()
//    let fontcolor = get_font_color()
//    let CopyrightNoticeColor: Color = #colorLiteral(red: 0.09, green: 0.95, blue: 1.00, alpha: 1.0)
//    change_font_color(CopyrightNoticeColor.value)
    kprint("Hello from Swift!" + "\n" + "Welcome to BetaOS in x86_64 MACH-O EFI mode :) !")
    kprint("Copyright © 2015-2018 Adam Kopec. All rights reserved.")
//    change_font_color(fontcolor)
//    System.sharedInstance.Timer.setChannel(.CHANNEL_0, mode: .MODE_3, hz: 1000000)
    System.sharedInstance.interruptManager.setIrqHandler(1, handler: NullHandler)
//    _ = addTask(name: "DummyTaskA", task: dummyA)
//    _ = addTask(name: "DummyTaskB", task: dummyB)
//    _ = addTask(name: "Console", task: console)
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
        serial_putc(65)
        yield()
    }
}

func dummyB() {
    while true {
        tprint("B")
        serial_putc(66)
        yield()
    }
}

func console() -> Void {
    while true {
        kprint("root$ ", terminator: "")
        findcommand()
    }
}
