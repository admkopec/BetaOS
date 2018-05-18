//
//  APIC.swift
//  Kernel
//
//  Created by Adam Kopeć on 3/6/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Addressing
import CustomArrays
import Loggable

struct LVTEntry: CustomStringConvertible {
    fileprivate(set) var value: BitArray
    
    enum DeliveryStatus: Int {
        case Idle = 0
        case SendPending = 1
    }
    
    var vector: UInt8 {
        get {
            return UInt8(value[0...7])
        }
        set(newValue) {
            value[0...7] = UInt64(newValue)
        }
    }
    
    var deliveryStatus: DeliveryStatus {
        return DeliveryStatus(rawValue: value[12])!
    }
    
    var masked: Bool {
        get { return value[16] == 1 }
        set(newValue) { value[16] = newValue ? 1 : 0 }
    }
    
    
    var description: String {
        return "INT\(vector) Masked: " + (masked ? "Yes" : "No")
    }
    
    var rawValue: UInt32 { return value.asUInt32 }
    
    init(rawValue: UInt32) {
        value = BitArray(rawValue)
    }
}

typealias TimerEntry = LVTEntry
extension TimerEntry {
    
    enum TimerMode: UInt8 {
        case oneShot     = 0b00
        case periodic    = 0b01
        case tscDeadline = 0b10
    }
    
    var timerMode: TimerMode {
        get {
            let rawValue = UInt8(value[17...18])
            return TimerMode(rawValue: rawValue) ?? .oneShot
        }
        set(newValue) {
            value[17...18] = UInt64(newValue.rawValue)
        }
    }
    
//    var description: String {
//        return "\(entry) TimerMode: " + String(describing: timerMode)
//    }
}

typealias InterruptEntry = LVTEntry
extension InterruptEntry {
    
    enum DeliveryMode: Int {
        case Fixed  = 0b000
        case SMI    = 0b010
        case NMI    = 0b100
        case ExtInt = 0b111
        case INIT   = 0b101
    }
    
    var deliveryMode: DeliveryMode {
        get {
            let mode = DeliveryMode(rawValue: Int(value[8...10]))
            return mode ?? .Fixed
        }
        set(newValue) {
            value[8...10] = UInt64(newValue.rawValue)
        }
    }
    
//    var description: String {
//        return "\(entry) DeliveryMode: " + String(describing: deliveryMode)
//    }
}

typealias LocalInterruptEntry = InterruptEntry
extension LocalInterruptEntry {
    enum InputPinPolarity {
        case ActiveHigh
        case ActiveLow
    }
    
    enum TriggerMode: Int {
        case Edge
        case Level
    }
    
    var inputPinPolarity: InputPinPolarity {
        return value[13] == 0 ? .ActiveHigh : .ActiveLow
    }
    
    var triggerMode: TriggerMode {
        return value[15] == 0 ? .Edge : .Level
    }
    
    var remoteIRR: Bool { return value[14] == 1 }
    
//    var description: String {
//        return "\(intEntry) Trigger: \(triggerMode)"
//            + " InputPolarity: \(inputPinPolarity)"
//    }
}

class APIC: InterruptController, Loggable {
    let Name: String = "APIC"
    static var IOAPICs = [InterruptController]()
    // 256 bit register composed of 8x32bit values
    // Each 32bit value is on a 128bit (16byte) boundary
    // These registers can only be read using 32bit accesses
    struct Register256 {
        private let registers: UnsafeMutableRawBufferPointer
        private let offset: Int
        
        init(_ registers: UnsafeMutableRawBufferPointer, _ offset: Int) {
            self.registers = registers
            self.offset = offset
        }
        
        func bit(_ bit: Int) -> Bool {
            let idx = (bit / 32) * 16
            let value = registers.load(fromByteOffset: offset + idx,
                                       as: UInt32.self)
            return value.bit(bit % 32)
        }
    }
    
    private let IA32_APIC_BASE_MSR: UInt32 = 0x1B
    private let APIC_REGISTER_SPACE_SIZE = 0x400
    private let bootProcessorBit = 8
    private let globalEnableBit = 11
    
    private var apicRegistersAddress: Address!
    private var apicRegisters: UnsafeMutableRawBufferPointer!
    
    var localAPICId:      UInt32 { return atOffset(0x20) }
    var localAPICVersion: UInt32 { return atOffset(0x30) }
    var taskPriority:     UInt32 { return atOffset(0x80) }
    var arbitrationPriority: UInt32 { return atOffset(0x90) }
    var processorPriority: UInt32 { return atOffset(0xA0) }
    var EOI: UInt32 {
        get { return 0 }
        set(value) { atOffset(0xB0, value: value) }
    }
    var remoteRead:       UInt32 { return atOffset(0xC0) }
    var logicalDestination: UInt32 {
        get { return atOffset(0xD0) }
        set(value) { atOffset(0xD0, value: value) }
    }
    var destinationFormat: UInt32 {
        get { return atOffset(0xE0) }
        set(value) { atOffset(0xE0, value: value) }
    }
    var spuriousIntVector: UInt32 {
        get { return atOffset(0xF0) }
        set(value) { atOffset(0xF0, value: value) }
    }
    var inService:    Register256 { return Register256(apicRegisters, 0x100) }
    var triggerMode:  Register256 { return Register256(apicRegisters, 0x180) }
    var interruptReq: Register256 { return Register256(apicRegisters, 0x200) }
    var errorStatus:  UInt32      { return atOffset(0x280) }
    
    var lvtCMCI: InterruptEntry {
        get { return InterruptEntry(rawValue: atOffset(0x2F0)) }
        set(newValue) { atOffset(0x2F0, value: newValue.rawValue) }
    }
    
    var interruptCmd: UInt64 {
        get {
            let lo = atOffset(0x300)
            let hi = atOffset(0x310)
            return UInt64(withDWords: lo, hi)
        }
        set(value) {
            let v = ByteArray(value)
            atOffset(0x300, value: UInt32(withBytes: v[0], v[1], v[2], v[3]))
            atOffset(0x310, value: UInt32(withBytes: v[4], v[5], v[6], v[7]))
        }
    }
    var lvtTimer: TimerEntry {
        get { return TimerEntry(rawValue: atOffset(0x320)) }
        set(newValue) { atOffset(0x320, value: newValue.rawValue) }
    }
    
    var lvtThermalSensor: InterruptEntry {
        get { return InterruptEntry(rawValue: atOffset(0x330)) }
        set(newValue) { atOffset(0x330, value: newValue.rawValue) }
    }
    var lvtPerfMonitorCounters: InterruptEntry {
        get { return InterruptEntry(rawValue: atOffset(0x340)) }
        set(newValue) { atOffset(0x340, value: newValue.rawValue) }
    }
    var lvtLint0: LocalInterruptEntry {
        get { return LocalInterruptEntry(rawValue: atOffset(0x350)) }
        set(newValue) { atOffset(0x350, value: newValue.rawValue) }
    }
    var lvtLint1: LocalInterruptEntry {
        get { return LocalInterruptEntry(rawValue: atOffset(0x360)) }
        set(newValue) { atOffset(0x360, value: newValue.rawValue) }
    }
    var lvtError: LVTEntry {
        get { return LVTEntry(rawValue: atOffset(0x370)) }
        set(newValue) { atOffset(0x370, value: newValue.rawValue) }
    }
    var initialCount: UInt32 {
        get { return atOffset(0x380) }
        set(value) { atOffset(0x380, value: value) }
    }
    var currentCount: UInt32 { return atOffset(0x390) }
    var divideConfig: UInt32 {
        get { return atOffset(0x3E0) }
        set(value) { atOffset(0x3E0, value: value) }
    }
    
    private func atOffset(_ offset: Int) -> UInt32 {
        return apicRegisters.load(fromByteOffset: offset, as: UInt32.self)
    }
    
    private func atOffset(_ offset: Int, value: UInt32) {
        apicRegisters.storeBytes(of: value, toByteOffset: offset,
                                 as: UInt32.self)
    }
    
    init?(madtEntries: [MADTEntry]) {
//        guard CPU.capabilities.apic else {
//            Log("APIC is not avaible", level: .Error)
//            return nil
//        }
        Log("Initialising...", level: .Info)
        
//        var apicStatus = BitArray(CPU.readMSR(IA32_APIC_BASE_MSR))
        var apicStatus = BitArray(rdmsr64(IA32_APIC_BASE_MSR))
        
        
        // Enable the APIC if currently disabled
        if apicStatus[globalEnableBit] != 1 {
            apicStatus[globalEnableBit] = 1
//            CPU.writeMSR(IA32_APIC_BASE_MSR, apicStatus.asUInt64)
            wrmsr64(IA32_APIC_BASE_MSR, apicStatus.asUInt64)
//            apicStatus = BitArray64(CPU.readMSR(IA32_APIC_BASE_MSR))
            apicStatus = BitArray(rdmsr64(IA32_APIC_BASE_MSR))
            if apicStatus[globalEnableBit] != 1 {
                Log("Failed to enable", level: .Error)
                return nil
            }
        }
        
        let bootProcessor = apicStatus[bootProcessorBit] == 1
        let maxPhyAddrBits = UInt(cpuid_info().pointee.cpuid_address_bits_physical)//CPU.capabilities.maxPhyAddrBits
        let lomask = ~(UInt(1 << 12) - 1)
        let himask = (1 << maxPhyAddrBits) - 1
        let mask = lomask & himask
        apicRegistersAddress = Address(apicStatus.asUInt64 & UInt64(mask), size: vm_size_t(APIC_REGISTER_SPACE_SIZE))
        let ptr = UnsafeMutableRawPointer(bitPattern: apicRegistersAddress.virtual)!
        apicRegisters = UnsafeMutableRawBufferPointer(start: ptr, count: APIC_REGISTER_SPACE_SIZE)
        Log("Boot: \(bootProcessor), maxPhys: \(maxPhyAddrBits)", level: .Debug)
        
        var ioapicEntries: [MADT.IOApicTable] = []
        var overrideEntries: [MADT.InterruptSourceOverrideTable] = []
        
        madtEntries.forEach {
            if let entry = $0 as? MADT.IOApicTable {
                ioapicEntries.append(entry)
            } else if let entry = $0 as? MADT.InterruptSourceOverrideTable {
                overrideEntries.append(entry)
            }
        }
        
        guard ioapicEntries.count > 0 else {
            Log("Cant find any I/O APICs in the ACPI:MADT tables", level: .Error)
            return nil
        }

        for entry in ioapicEntries {
            if let ioapic = IOAPIC(ioapicTable: entry, intSourceOverrides: overrideEntries) {
                Log("Found: \(ioapic)", level: .Verbose)
                APIC.IOAPICs.append(ioapic)
            }
        }
        Log("Has \(APIC.IOAPICs.count) I/O APICs", level: .Verbose)
        setupTimer()
        // FIXME: Use correct Vector
        spuriousIntVector = 0x1ff
    }
    
    func setupTimer() {
        Log("InitialCount: \(initialCount) CurrentCount: \(currentCount) Divide Config: \(divideConfig)", level: .Debug)
        
        var newLvtTimer = lvtTimer
        newLvtTimer.deliveryMode = .Fixed
        // FIXME: Use correct Vector
        newLvtTimer.vector = 48
        newLvtTimer.masked = true
        newLvtTimer.timerMode = .periodic
        lvtTimer = newLvtTimer
        divideConfig = 0b1001
        initialCount = 100000000
        Log("New Timer: \(lvtTimer)", level: .Verbose)
        
        Log("InitialCount: \(initialCount) CurrentCount: \(currentCount) Divide Config: \(divideConfig)", level: .Debug)
    }
    
    func enableIRQ(_ irq: Int) {
        Log("Enabling IRQ: \(irq)", level: .Info)
        APIC.IOAPICs[0].enableIRQ(irq)
    }
    
    
    func disableIRQ(_ irq: Int) {
        Log("Disabling IRQ: \(irq)", level: .Info)
        APIC.IOAPICs[0].disableIRQ(irq)
    }
    
    
    func disableAllIRQs() {
    }
    
    // FIXME: Translate also Local APIC IRQs
    func getIRQ(from vector: Int) -> Int {
        return  APIC.IOAPICs[0].getIRQ(from: vector)
    }

    func ackIRQ(_ irq: Int) {
        EOI = 1
    }
}
