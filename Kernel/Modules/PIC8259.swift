//
//  PIC8259.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/13/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

final class PIC8259: InterruptController, Loggable {
    let Name = "PIC8259"
    // IO Port addresses
    fileprivate let PIC1_CMD_REG:  UInt32 = 0x20
    fileprivate let PIC1_DATA_REG: UInt32 = 0x21
    fileprivate let PIC2_CMD_REG:  UInt32 = 0xA0
    fileprivate let PIC2_DATA_REG: UInt32 = 0xA1
    
    // 8259 Commands
    fileprivate let ICW1_ICW4:       UInt8 = 0x01    // ICW1/ICW4
    fileprivate let ICW1_SINGLE:     UInt8 = 0x02    // Single (cascade) mode
    fileprivate let ICW1_INTERVAL4:  UInt8 = 0x04    // Call address interval 4 (8)
    fileprivate let ICW1_LEVEL:      UInt8 = 0x08    // Level triggered (edge) mode
    fileprivate let ICW1_INIT:       UInt8 = 0x10    // Initialization
    
    fileprivate let ICW4_8086:       UInt8 = 0x01    // 8086/88 (MCS-80/85) mode
    fileprivate let ICW4_AUTO:       UInt8 = 0x02    // Auto (normal) EOI
    fileprivate let ICW4_BUF_SLAVE:  UInt8 = 0x08    // Buffered mode/slave
    fileprivate let ICW4_BUF_MASTER: UInt8 = 0x0C    // Buffered mode/master
    fileprivate let ICW4_SFNM:       UInt8 = 0x10    // Special fully nested (not)
    
    fileprivate let OCW3_READ_IRR:   UInt8 = 0x0A    // OCW3 IRR read
    fileprivate let OCW3_READ_ISR:   UInt8 = 0x0B    // OCW3 ISR read
    fileprivate let EOI:             UInt8 = 0x20    // End of interrupt
    fileprivate let SPECIFIC_EOI:    UInt8 = 0x60    // Specific IRQ (+ irq)
    fileprivate let CASCADE_IRQ:     UInt8 = 0x02    // PIC2 is at IRQ2 on PIC1
    
    
    init() {
        // Disable all IRQs
        disableAllIRQs()
        rebaseIRQs()
        Log("Initialized", level: .Debug)
    }
    
    
    func enableIRQ(_ irq: Int) {
        Log("Enabling IRQ: \(irq)", level: .Debug)
        guard irq < 16 else {
            Log("Invalid IRQ: \(irq)", level: .Error)
            return
        }
        if (irq <= 7) {
            var mask = inb(PIC1_DATA_REG)
            mask &= ~(UInt8(1 << irq))
            outb(PIC1_DATA_REG, mask)
        } else {
            var mask = inb(PIC2_DATA_REG)
            mask &= ~(1 << UInt8(irq - 7))
            outb(PIC2_DATA_REG, mask)
        }
    }
    
    
    func disableIRQ(_ irq: Int) {
        Log("Disabling IRQ: \(irq)", level: .Debug)
        guard irq < 16 else {
            Log("Invalid IRQ: \(irq)", level: .Error)
            return
        }
        if (irq <= 7) {
            var mask = inb(PIC1_DATA_REG)
            mask |= (1 << UInt8(irq))
            outb(PIC1_DATA_REG, mask)
        } else {
            var mask = inb(PIC2_DATA_REG)
            mask |= (1 << UInt8(irq - 7))
            outb(PIC2_DATA_REG, mask)
        }
    }
    
    
    func disableAllIRQs() {
        outb(PIC1_DATA_REG, 0xff)
        outb(PIC2_DATA_REG, 0xff)
    }
    
    func getIRQ(from vector: Int) -> Int {
        return vector - 0xD0
    }
    
    func ackIRQ(_ irq: Int) {
        guard irq < 16 else {
            Log("Invalid IRQ: \(irq)", level: .Error)
            return
        }
        
        // Check real IRQ occurred
        let active = readISR().bit(irq)
        if !active {
            Log("Spurious IRQ: \(irq)", level: .Debug)
        }
        
        if (irq > 7) {
            outb(PIC2_CMD_REG, specificEOIFor(irq: irq))
            outb(PIC2_CMD_REG, specificEOIFor(irq: Int(CASCADE_IRQ)))
        } else {
            outb(PIC1_CMD_REG, specificEOIFor(irq: irq))
        }
    }
    
    var status: String {
        return "IRR: \(String(readIRR(), radix: 16)) ISR: \(String(readISR(), radix: 16))"
    }
    
    
    private func specificEOIFor(irq: Int) -> UInt8 {
        return SPECIFIC_EOI + UInt8(irq & 0x7)
    }
    
    
    // Reroute the interrupts to vectors 0x20 - 0x2F
    private func rebaseIRQs() {
        let loVector: UInt8 = 0xD0
        let hiVector: UInt8 = 0xD8
        cli()
        outb(PIC1_CMD_REG, ICW1_ICW4 | ICW1_INIT)
        outb(PIC1_DATA_REG, loVector)
        outb(PIC1_DATA_REG, 1 << CASCADE_IRQ)
        outb(PIC1_DATA_REG, ICW4_8086)
        outb(PIC2_CMD_REG, ICW1_ICW4 | ICW1_INIT)
        outb(PIC2_DATA_REG, hiVector)
        outb(PIC2_DATA_REG, CASCADE_IRQ)
        outb(PIC2_DATA_REG, ICW4_8086)
    }
    
    
    // Helper routine for readIRR()/readISR(), UInt16 is mask of IRQ0-15
    private func readIRQReg(_ cmd: UInt8) -> UInt16 {
        outb(PIC1_CMD_REG, cmd)
        outb(PIC2_CMD_REG, cmd)
        let msb = inb(PIC2_CMD_REG)
        let lsb = inb(PIC1_CMD_REG)
        return UInt16(withBytes: lsb, msb)
    }
    
    
    // Read Interrupt Request Register, interrupts that have been raised
    private func readIRR() -> UInt16 {
        return readIRQReg(OCW3_READ_IRR)
    }
    
    
    // Read Interrupt Service Register, interrupts that are being serviced
    // (sent to CPU)
    private func readISR() -> UInt16 {
        return readIRQReg(OCW3_READ_ISR)
    }
}
