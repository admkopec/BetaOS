//
//  PCI.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

struct BAR {
    let address : Address
    let port    : Int16
    let size    : Int
    let flags   : Int16
    let is64bit : Bool
}

final class PCI {
    private(set) public var Bus      : UInt8
    private(set) public var Slot     : UInt8
    private(set) public var Function : UInt8
    private(set) public var VendorID : UInt16 = 0x00
    private(set) public var DeviceID : UInt16 = 0x00
    public var isValid: Bool {
        if VendorID == 0x0000 {
            return false
        } else if VendorID == 0xFFFF {
            return false
        } else {
            return true
        }
    }
    
    private var BaseAddr : UInt32
    
    private func GetConfig(offset : UInt8) -> UInt32 {
        let addr = BaseAddr | UInt32(offset & 0xFC)
        outl(0xCF8, addr)
        let data = inl(0xCFC >> (8 * (offset % 0x04)))
        return data
    }
    
    func GetBAR(BARNumber: Int) -> BAR {
        var offset = UInt8(0x10 + BARNumber)
        
        let AddrLow = GetConfig(offset: offset)
        WriteConfig(offset: offset, data: 0xffffffff)
        let MaskLow = GetConfig(offset: offset)
        WriteConfig(offset: offset, data: AddrLow)
        
        if AddrLow & 0x1 == 1 {
            return BAR(address: Address(0), port: Int16(AddrLow & 0xFFFFFFFC), size: Int(~(MaskLow & ~0x3) + 1), flags: Int16(AddrLow & 0x3), is64bit: false)
        } else if AddrLow & 0x04 == 1 {
            // 64-bit Bar
            offset = UInt8(0x10 + BARNumber + 1 + 4)
            let AddrHigh = GetConfig(offset: offset)
            WriteConfig(offset: offset, data: 0xffffffff)
            let MaskHigh = GetConfig(offset: offset)
            WriteConfig(offset: offset, data: AddrHigh)
            let address = Address(UInt(UInt(AddrLow & 0xFFFFFFF0) + (UInt(AddrHigh & 0xFFFFFFFF) << 32)))
            let masks = (UInt(MaskHigh) << 32) | (UInt(MaskLow) & ~0xF)
            return BAR(address: address, port: 0, size: Int(~(masks) + 1), flags: Int16(AddrLow & 0xF), is64bit: true)
        } else {
            return BAR(address: Address(UInt(AddrLow & 0xFFFFFFF0)), port: 0, size: Int(~(MaskLow & ~0x3) + 1), flags: Int16(AddrLow & 0x3), is64bit: false)
        }
    }
    
    private func WriteConfig(offset : UInt8, data: UInt32) {
        let addr = BaseAddr | UInt32(offset & 0xFC)
        outl(0xCF8, addr)
        outl(0xCFC, data)
    }
    
    func EnableBusMastering() {
        var Command = GetConfig(offset: 0x4)
        Command |= 0x4
        WriteConfig(offset: 0x4, data: Command)
    }
    
    init(bus : UInt8 = 0, slot : UInt8 = 0, function : UInt8 = 0) {
        Bus      = bus
        Slot     = slot
        Function = function
        BaseAddr = GetBaseAddr(Bus, Slot, Function)
        let data = GetConfig(offset: 0)
        VendorID = UInt16(truncatingIfNeeded: data)
        DeviceID = UInt16(truncatingIfNeeded: (data >> 16))
    }
}