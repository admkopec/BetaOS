//
//  InterruptManager.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/13/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

typealias IRQHandler = (Int) -> ()
protocol InterruptController {
    func enableIRQ(_ irq: Int)
    func disableIRQ(_ irq: Int)
    func disableAllIRQs()
    func ackIRQ(_ irq: Int)
    func getIRQ(from: Int) -> Int
}

class InterruptManager: Loggable {
    let Name = "InterruptManager"
    fileprivate var irqController: InterruptController?
    // TODO: Add better support for APIC
    fileprivate var irqHandlers: [IRQHandler] = Array(repeating: UnexpectedInterrupt, count: 16)
    
    init() { }
    
    init(acpiTables: ACPI? = System.sharedInstance.ACPI) {
        func initAPIC() -> InterruptController? {
            if let madtEntries = (acpiTables?.tables.filter({$0 is MADT}).first as? MADT)?.madtEntries  {
                return APIC(madtEntries: madtEntries)
            } else {
            return nil
            }
        }
        
        func initPIC() -> InterruptController? {
            if (acpiTables?.tables.filter({$0 is MADT}).first as? MADT)?.HasCompatDual8259 == false {
                return nil
            } else {
                return PIC8259()
            }
        }
        
        guard let controller = initAPIC() ?? initPIC() else {
            let Nilcontroller: InterruptController? = nil
            irqController = Nilcontroller!
            Log("Cannot initialise IRQ controller", level: .Error)
            fatalError()
        }
        irqController = controller
        
        Log("Using \(irqController!.self) as Interrupt Controller", level: .Info)
        irqController!.disableAllIRQs()
    }
    
    func enableIRQs() {
        Log("Enabling IRQs", level: .Debug)
        cliCalled = 0
        sti()
    }
    
    func disableIRQs() {
        Log("Disabling IRQs", level: .Debug)
        cliCalled = -1
        cli()
    }
    
    func setIrqHandler(_ irq: Int, handler: @escaping IRQHandler) {
        irqHandlers[irq] = handler
        irqController!.enableIRQ(irq)
    }
    
    func removeIrqHandler(_ irq: Int) {
        irqController!.disableIRQ(irq)
        irqHandlers[irq] = InterruptManager.UnexpectedInterrupt
    }
    
    static private func UnexpectedInterrupt(irq: Int) {
        kprint("Unexpected interrupt: \(irq)")
    }
}

// FIXME: Add support for Local APIC
@_silgen_name("InterruptHandler")
public func InterruptHandler(InterruptNumber: Int) {
    let irq = System.sharedInstance.interruptManager.irqController!.getIRQ(from: InterruptNumber)
    System.sharedInstance.interruptManager.irqHandlers[irq](irq)
//    IncommingInterrupt(Int32(InterruptNumber))
    System.sharedInstance.interruptManager.irqController!.ackIRQ(irq)
}
