//
//  SVGA.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import Addressing
import CommonExtensions
import Loggable
import Graphics

class SVGA: VideoModule, PCIModule {
    let Name: String = "SVGA"
    var description: String {
        return "VMware SVGA II Virtual Graphics Card \(VRAMSize / MB)MB"
    }
    
    static let CompatibleDevices: [PCIDevice] = [(0x15AD, 0x0405)]
    static var CompatibleClasses: [PCIClass]? = nil
    
    var IOBase          = 0 as Int16
    var FrameBufferBase = Address(0)
    var FIFOBase        = Address(0)
    var VRAMSize        = 0
    var FrameBufferSize = 0
    var FIFOSize        = 0
    var FIFOMemPtr      = UnsafeMutablePointer<UInt32>(bitPattern: 0)
    
    
    var Capabilities    = 0 as UInt32
    
    var mainView: MainView = MainView(size: Size.init(width: 1280, height: 720), depth: 0)
    
    enum Registers: UInt32 {
        case ID = 0
        case Enable = 1
        case Width = 2
        case Height = 3
        case MaxWidth = 4
        case MaxHeight = 5
        case Depth = 6
        case BitsPerPixel = 7
        case Pseudocolor = 8
        case RedMask = 9
        case GreenMask = 10
        case BlueMask = 11
        case BytesPerLine = 12
        case FBStart = 13
        case FBOffset = 14
        case VRAMSize = 15
        case FBSize = 16
        case Capabilities = 17
        case MemStart = 18
        case MemSize = 19
        case ConfigDone = 20
    }
    
    var ReservedSize: UInt32 = 0
    var UsingBounceBuffer: Bool = false
    var BounceBuffer: UnsafeMutableRawPointer = UnsafeMutableRawPointer.allocate(byteCount: 1024 * 1024, alignment: 16)
    var NextFence: UInt32 = 0
    
    required init?(pci: PCI) {
        if ((pci.VendorID != 0x15AD) || (pci.DeviceID != 0x0405)) {
            Log("Provided PCI device is not SVGA II!", level: .Error)
            return nil
        }
        
        IOBase          = pci.GetBAR(BARNumber: 0).port
        FrameBufferBase = pci.GetBAR(BARNumber: 1).address
        FIFOBase        = pci.GetBAR(BARNumber: 2).address
        
        let id2: UInt32 = UInt32(UInt32(0x900000) << 8 | 2)
        Write(register: .ID, value: id2)

        if Read(register: .ID) != id2 {
            Log("SVGA II does not support version ID 2!", level: .Error)
            return nil
        }
        
        VRAMSize        = Int(Read(register: .VRAMSize))
        FrameBufferSize = Int(Read(register: .FBSize))
        FIFOSize        = Int(Read(register: .MemSize))
        
        Log("Sizes: VRAM: \(VRAMSize/MB)MB, FB: \(FrameBufferSize/MB)MB, FIFO: \(FIFOSize/KB)KB", level: .Verbose)
        
        if FrameBufferSize < MB {
            Log("Framebuffer size too small!", level: .Error)
            return nil
        }
        if FIFOSize < 128 * KB {
            Log("FIFO size to small!", level: .Error)
            return nil
        }
        
        Capabilities = Read(register: .Capabilities)
        
        // Check for IRQs
        
        FIFOMemPtr = UnsafeMutablePointer<UInt32>(bitPattern: FIFOBase.virtual)
        FIFOMemPtr!.advanced(by: 0).pointee = (291 * 4) as UInt32
        FIFOMemPtr!.advanced(by: 1).pointee = UInt32(FIFOSize)
        FIFOMemPtr!.advanced(by: 2).pointee = FIFOMemPtr!.advanced(by: 0).pointee
        FIFOMemPtr!.advanced(by: 3).pointee = FIFOMemPtr!.advanced(by: 0).pointee
        
        let FIFOCapabilities = FIFOMemPtr!.advanced(by: 4).pointee
        
        if FIFOCapabilities & 0x00008000 != 0 && (FIFOMemPtr!.pointee > (287 << 2)) {
            FIFOMemPtr!.advanced(by: 287).pointee = (((2) << 16) | ((1) & 0xFF))
        }

        Write(register: .Enable, value: 1)
        Write(register: .ConfigDone, value: 1)
        
        Write(register: .Width, value: 1440)
        Write(register: .Height, value: 900)
        Write(register: .BitsPerPixel, value: 32)
        Write(register: .Enable, value: 1)

        mainView = MainView(size: Size.init(width: Int(Read(register: .Width)), height: Int(Read(register: .Height))), depth: Int(Read(register: .BitsPerPixel)))
        System.sharedInstance.Video = self
        if let tools = System.sharedInstance.modulesController.modules.first(where: {$0 is VMwareTools}) as? VMwareTools {
            Log("Got Tools: \(tools.self)", level: .Debug)
        }
//        if let tools = System.sharedInstance.modulesController.modules.first(where: {type(of: $0) == VMwareTools.self}) {
//            Log("Got Tools: \(type(of: tools))", level: .Debug)
//        }
//        if let tools = System.sharedInstance.modulesController.modules.first(where: {$0 is VMwareTools}) as? VMwareTools {
//            tools.RegisterResolutionCapabilities()
//        }
//        if let tools = System.sharedInstance.modulesController.modules.index(where: {$0 is VMwareTools}) {
//            (System.sharedInstance.modulesController.modules[tools] as! VMwareTools).RegisterResolutionCapabilities()
//        }
    }
    
    
    func FIFOReserve(bytes: UInt32) -> UInt {
        let Max = FIFOMemPtr!.advanced(by: 1).pointee
        let Min = FIFOMemPtr!.advanced(by: 0).pointee
        let NextCommand = FIFOMemPtr!.advanced(by: 2).pointee
        let FIFOCapabilities = FIFOMemPtr!.advanced(by: 4).pointee
        let reservable: Bool
        if FIFOCapabilities & 0x40 != 0 {
            reservable = true
        } else {
            reservable = false
        }
        if bytes > 1024*1024 || bytes > (Max - Min) {
            Log("FIFO command too large!", level: .Error)
            return 0
        }
        
        if bytes % UInt32(MemoryLayout<UInt32>.size) == 1 {
            Log("FIFO command not 32-bit aligned!", level: .Error)
            return 0
        }
        
        if ReservedSize != 0 {
            Log("FIFOReserve before FIFOCommit!", level: .Error)
            return 0
        }
        
        ReservedSize = bytes
        
        while true {
            let Stop = FIFOMemPtr!.advanced(by: 3).pointee
            var reserveInPlace = false
            var needBounce = false
            if NextCommand >= Stop {
                if NextCommand + bytes < Max || (NextCommand + bytes == Max && Stop > Min) {
                    reserveInPlace = true
                } else if (Max - NextCommand) + (Stop - Min) <= bytes {
                    //FIFOFull
                    hlt()
                } else {
                    needBounce = true
                }
            } else {
                if NextCommand + bytes < Stop {
                    reserveInPlace = true
                } else {
                    //FIFOFull
                    hlt()
                }
            }
            if reserveInPlace {
                if reservable || bytes <= MemoryLayout<UInt32>.size {
                    UsingBounceBuffer = false
                    if reservable {
                        FIFOMemPtr!.advanced(by: 14).pointee = bytes
                    }
                    return UInt(NextCommand) + FIFOBase.virtual
                } else {
                    needBounce = true
                }
            }
            if needBounce {
                UsingBounceBuffer = true
                return UInt(bitPattern: BounceBuffer)
            }
        }
    }
    
    func FIFOCommit(bytes: UInt32) -> Void {
        let Max = FIFOMemPtr!.advanced(by: 1).pointee
        let Min = FIFOMemPtr!.advanced(by: 0).pointee
        var NextCommand = FIFOMemPtr!.advanced(by: 2).pointee
        let FIFOCapabilities = FIFOMemPtr!.advanced(by: 4).pointee
        let reservable: Bool
        if FIFOCapabilities & 0x40 != 0 {
            reservable = true
        } else {
            reservable = false
        }
        if ReservedSize == 0 {
            Log("FIFOCommit before FIFOReserve!", level: .Error)
            return
        }
        ReservedSize = 0
        if UsingBounceBuffer {
            if reservable {
                let chunkSize = min(bytes, Max - NextCommand)
                FIFOMemPtr!.advanced(by: 14).pointee = bytes
                memcpy(UnsafeMutableRawPointer(bitPattern:(UInt(NextCommand) + FIFOBase.virtual)), BounceBuffer, Int(chunkSize))
                memcpy(UnsafeMutableRawPointer(bitPattern:(UInt(Min) + FIFOBase.virtual)), BounceBuffer.advanced(by: Int(chunkSize)), Int(bytes - chunkSize))
            } else {
                Log("Not supported rigth now!", level: .Error)
                return
            }
        }
        if UsingBounceBuffer || reservable {
            NextCommand += bytes
            if NextCommand >= Max {
                NextCommand -= Max - Min
            }
            FIFOMemPtr!.advanced(by: 2).pointee = NextCommand
        }
        if reservable {
            FIFOMemPtr!.advanced(by: 14).pointee = 0
        }
    }
    
    func FIFOCommitAll() -> Void {
        FIFOCommit(bytes: ReservedSize)
    }
    
    func FIFOReserveCommand(type: UInt32, bytes: UInt32) -> UnsafeMutablePointer<UInt32> {
        let command = UnsafeMutablePointer<UInt32>(bitPattern: FIFOReserve(bytes: bytes + 4))
        command!.pointee = type
        return command!.advanced(by: 1)
    }
    
    func SendUpdate() {
        let command = FIFOReserveCommand(type: 1, bytes: 4*4)
        command.advanced(by: 0).pointee = 0
        command.advanced(by: 1).pointee = 0
        command.advanced(by: 2).pointee = UInt32(mainView.Display.Resolution.Width)
        command.advanced(by: 3).pointee = UInt32(mainView.Display.Resolution.Height)
        FIFOCommitAll()
    }
    
    func Read(register: Registers) -> UInt32 {
        outl(UInt32(IOBase), register.rawValue)
        return inl(UInt32(IOBase + 1))
    }
    
    func Write(register: Registers, value: UInt32) -> Void {
        outl(UInt32(IOBase), register.rawValue)
        outl(UInt32(IOBase + 1), value)
    }
    
    func refresh() {
        guard mainView.ShouldRedraw || modified else {
            return
        }
        let ptr = UnsafeMutablePointer<UInt32>(bitPattern: FrameBufferBase.virtual)
        canUseSSEmemcpy = true
        memcpy(ptr!, mainView.FrameBuffer.baseAddress!, mainView.Display.Resolution.Height * mainView.Display.Resolution.Width * (mainView.Display.Depth / 8))
        canUseSSEmemcpy = false
        mainView.ShouldRedraw = false
        modified = false
        SendUpdate()
    }
}
