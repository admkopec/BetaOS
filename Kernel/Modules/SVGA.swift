//
//  SVGA.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

struct SVGA: PCIModule {
    var Name: String = "SVGA"
    var description: String {
        return "SVGA: VRAM: \(VRAMSize / GB)GB"
    }
    
    static let CompatibleDevices: [PCIDevice] = [(0x15AD, 0x0405)]
    
    var IOBase          = BAR(address: 0, port: 0, size: 0, flags: 0, is64bit: false)
//    var FrameBufferBase = BAR(address: 0, port: 0, size: 0, flags: 0, is64bit: false)
    var FIFOBase        = BAR(address: 0, port: 0, size: 0, flags: 0, is64bit: false)
    var VRAMSize        = 0
    var FrameBufferSize = 0
    var FIFOSize        = 0
    
    init?(pci: PCI) {
        if ((pci.VendorID != 0x15AD) || (pci.DeviceID != 0x0405)) {
            Log("Provided PCI device is not SVGA II!", level: .Error)
            return nil
        }
        
        IOBase          = pci.GetBAR(BARNumber: 0)
//        FrameBufferBase = pci.GetBAR(BARNumber: 1)
        FIFOBase        = pci.GetBAR(BARNumber: 2)
        
        let id2: UInt32 = UInt32(UInt32(0x900000) << 8 | 2)
//        let id1: UInt32 = UInt32(UInt32(0x900000) << 8 | 1)
//        let id0: UInt32 = UInt32(UInt32(0x900000) << 8 | 0)
        
//        var DeviceVersionID = id2
//        repeat {
//            WriteReg(index: 0, value: DeviceVersionID)
//            if (ReadReg(index: 0) == DeviceVersionID) {
//                Log("Negotiated Version: \(DeviceVersionID)", level: .Debug)
//                break
//            } else {
//                DeviceVersionID -= 1
//            }
//        } while DeviceVersionID >= id0
//
//        if DeviceVersionID < id0 {
//            Log("Couldn't negotiate SVGA device version.", level: .Error)
//            return nil
//        }
        
        WriteReg(index: 0, value: id2)

        if ReadReg(index: 0) != id2 {
            Log("SVGA II does not support version ID 2!", level: .Error)
            return nil
        }
        
        VRAMSize        = Int(ReadReg(index: 15))
        FrameBufferSize = Int(ReadReg(index: 16))
        FIFOSize        = Int(ReadReg(index: 19))
        
        Log("Sizes: VRAM: \(VRAMSize/GB)GB, FB: \(FrameBufferSize/MB)MB, FIFO: \(FIFOSize/KB)", level: .Debug)
        
        if FrameBufferSize < MB {
            Log("Framebuffer size too small!", level: .Error)
            return nil
        }
        if FIFOSize < 128 * KB {
            Log("FIFO size to small!", level: .Error)
            return nil
        }
    }
    
    func ReadReg(index: UInt32) -> UInt32 {
        outl(UInt32(IOBase.port), index)
        return inl(UInt32(IOBase.port + 1))
    }
    
    func WriteReg(index: UInt32, value: UInt32) -> Void {
        outl(UInt32(IOBase.port), index)
        outl(UInt32(IOBase.port + 1), value)
    }
}
