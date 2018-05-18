//
//  E1000.swift
//  Kernel
//
//  Created by Adam Kopeć on 3/1/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable
import Networking

class E1000: PCIModule, NetworkModule {
    static var CompatibleDevices: [PCIDevice] = [ (0x8086, 0x100E), (0x8086, 0x153A), (0x8086, 0x10D3),
                                                  (0x8086, 0x10EA), (0x8086, 0x1502), (0x8086, 0x1503),
                                                  (0x8086, 0x1000), (0x8086, 0x1001), (0x8086, 0x1004),
                                                  (0x8086, 0x1008), (0x8086, 0x1009), (0x8086, 0x100C),
                                                  (0x8086, 0x100D), (0x8086, 0x1015), (0x8086, 0x1016),
                                                  (0x8086, 0x1017), (0x8086, 0x101E), (0x8086, 0x100F),
                                                  (0x8086, 0x1011), (0x8086, 0x1026), (0x8086, 0x1027),
                                                  (0x8086, 0x1028), (0x8086, 0x1010), (0x8086, 0x1012),
                                                  (0x8086, 0x101D), (0x8086, 0x1013), (0x8086, 0x1018),
                                                  (0x8086, 0x1014), (0x8086, 0x1078), (0x8086, 0x1075),
                                                  (0x8086, 0x1076), (0x8086, 0x1077), (0x8086, 0x107C),
                                                  (0x8086, 0x1079), (0x8086, 0x107A), (0x8086, 0x107B),
                                                  (0x8086, 0x108A), (0x8086, 0x1099), (0x8086, 0x1019),
                                                  (0x8086, 0x101A), (0x8086, 0x10B5), (0x8086, 0x2E6E),
                                                  (0x8086, 0x10F6) ]
    static var CompatibleClasses: [PCIClass]? = nil
    
    let Name: String = "E1000"
    var description: String {
        return "Intel E1000 Series Network Device Controller"
    }
    
    let TXBufferNum = 8
    let RXBufferNum = 8
    
    let TXBufferSize = 2048
    let RXBufferSize = 2048
    
    enum Registers: UInt {
        case CTRL       = 0x0000
        case STATUS     = 0x0008
        case EEPROMCTRL = 0x0010
        case EEPROM     = 0x0014
        case CTRLEXT    = 0x0018
        case VET        = 0x0038
        case ICAUSE     = 0x00C0
        case IMASK      = 0x00D0
        case IMASKCLR   = 0x00D8
        
        case RXCTRL     = 0x0100
        case RXDESCLO   = 0x2800
        case RXDESCHI   = 0x2804
        case RXDESCLEN  = 0x2808
        case RXDESCHEAD = 0x2810
        case RXDESCTAIL = 0x2818
        case RXDELAY    = 0x2820
        case RADV       = 0x282C
        
        case TXCTRL     = 0x0400
        case TXDESCLO   = 0x3800
        case TXDESCHI   = 0x3804
        case TXDESCLEN  = 0x3808
        case TXDESCHEAD = 0x3810
        case TXDESCTAIL = 0x3818
        case TXDELAY    = 0x3820
        case TADV       = 0x382C
        
        case RAL        = 0x5400
    }
    
    enum Control: UInt {
        case AUTOSPEED  = 0x00000020
        case LINKUP     = 0x00000040
        case RESET      = 0x04000000
        case PHYRESET   = 0x80000000
    }
    
    enum Status: UInt {
        case LINKUP = 1
    }
    
    enum RXControl: UInt {
        case ENABLE     = 0x0001
        case BROADCAST  = 0x8000
        case BUFSIZE    = 0x0000
    }
    
    enum TXControl: UInt {
        case ENABLE     = 0x00000001
        case PADDING    = 0x00000008
        case COLLTRESH  = 0x000000F0
        case COLLDIST   = 0x00040000
        case COLLDIST2  = 0x0003F000
        case RRTRESH    = 0x30000000
    }
    
    enum TXCommand: UInt8 {
        case EOP  = 0x01
        case IFCS = 0x02
    }
    
    enum ICRTypes: UInt {
        case TRANSMIT   = 0x01
        case LINKCHANGE = 0x04
        case RECEIVE    = 0x80
    }
    
    var dhcp: DHCP!
    var udp: UDP!
    var ip: IPProtocol!
    var MAC: [UInt8] = [UInt8]()
    var IP: IPv4 = IPv4(IP4: (0, 0, 0, 0), iIP4: 0)
    var Subnet: IPv4 = IPv4(IP4: (0, 0, 0, 0), iIP4: 0)
    var GatewayIP: IPv4 = IPv4(IP4: (0, 0, 0, 0), iIP4: 0)
    let BaseAddr: BAR
    var IntLine: Int
    var IntPin: Int
    var RXDesc: UnsafeMutableBufferPointer<e1000_rx_descriptor>
    var TXDesc: UnsafeMutableBufferPointer<e1000_tx_descriptor>
    var RXBuffer: UnsafeMutableBufferPointer<UInt8>
    var TXBuffer: UnsafeMutableBufferPointer<UInt8>
    var RXCurBuffer = 0
    var TXCurBuffer = 0
    var foundEEPROM = false
    var e1000e = false
    
    required init?(pci: PCI) {
        pci.EnableBusMastering()
        BaseAddr = pci.GetBAR(BARNumber: 0)
        IntLine  = Int(pci.InterruptLine)
        IntPin   = Int(pci.InterruptPin)
        RXDesc = UnsafeMutableBufferPointer<e1000_rx_descriptor>(start: UnsafeMutablePointer<e1000_rx_descriptor>.allocate(capacity: RXBufferNum), count: RXBufferNum)
        TXDesc = UnsafeMutableBufferPointer<e1000_tx_descriptor>(start: UnsafeMutablePointer<e1000_tx_descriptor>.allocate(capacity: TXBufferNum), count: TXBufferNum)
        RXBuffer = UnsafeMutableBufferPointer<UInt8>(start: UnsafeMutablePointer<UInt8>.allocate(capacity: RXBufferSize), count: RXBufferSize)
        TXBuffer =  UnsafeMutableBufferPointer<UInt8>(start: UnsafeMutablePointer<UInt8>.allocate(capacity: TXBufferSize), count: TXBufferSize)
        dhcp = DHCP(device: self)
        udp  = UDP(device: self)
        ip   = IPProtocol(device: self)
        Log("IRQ: \(IntLine)", level: .Debug)
        Log("IRQ Pin: \(IntPin)", level: .Debug)
        
        if !findEEPROM() {
            Log("Couldn't find EEPROM", level: .Warning)
        } else {
            Log("Found EEPROM", level: .Verbose)
        }
        writeCommand(register: .RXCTRL, value: 0)
        writeCommand(register: .TXCTRL, value: 0)
        writeCommand(register: .CTRL, value: Control.PHYRESET.rawValue)
        // Wait 10ms
        hlt()
        writeCommand(register: .CTRL, value: Control.RESET.rawValue)
        // Wait 10ms
        hlt()
        for _ in 0 ... 1000 {
            if readCommand(register: .CTRL) & Control.RESET.rawValue != 0 {
                break
            }
        }
        
        // Setup Flash
        // Here
        
        writeCommand(register: .CTRL, value: Control.AUTOSPEED.rawValue | Control.LINKUP.rawValue)
        
        // Rx/Tx-Ring Init
        let rxAddr = Address(UInt(bitPattern: RXDesc.baseAddress!))
        writeCommand(register: .RXDESCHI, value: rxAddr.physical >> 32)
        writeCommand(register: .RXDESCLO, value: rxAddr.physical & 0xFFFFFFFF)
        writeCommand(register: .RXDESCLEN, value: UInt(RXBufferNum * MemoryLayout<e1000_rx_descriptor>.size))
        writeCommand(register: .RXDESCHEAD, value: 0)
        writeCommand(register: .RXDESCTAIL, value: UInt(RXBufferNum - 1))
        writeCommand(register: .RXDELAY, value: 0)
        writeCommand(register: .RADV, value: 0)
        
        let txAddr = Address(UInt(bitPattern: TXDesc.baseAddress!))
        writeCommand(register: .TXDESCHI, value: txAddr.physical >> 32)
        writeCommand(register: .TXDESCLO, value: txAddr.physical & 0xFFFFFFFF)
        writeCommand(register: .TXDESCLEN, value: UInt(TXBufferNum * MemoryLayout<e1000_tx_descriptor>.size))
        writeCommand(register: .TXDESCHEAD, value: 0)
        writeCommand(register: .TXDESCTAIL, value: UInt(TXBufferNum - 1))
        writeCommand(register: .TXDELAY, value: 0)
        writeCommand(register: .TADV, value: 0)
        
        // Deaktivate VLANs
        writeCommand(register: .VET, value: 0)
        
        if !getMAC() {
            Log("Couldn't read MAC", level: .Error)
            RXBuffer.deallocate()
            TXBuffer.deallocate()
            RXDesc.deallocate()
            TXDesc.deallocate()
            return nil
        } else {
            var MACdesc = ""
            for a in MAC {
                MACdesc += "\(String(a, radix: 16).leftPadding(toLength: 2, withPad: "0").uppercased()):"
            }
            MACdesc.removeLast()
            Log("Found MAC: \(MACdesc)", level: .Verbose)
        }
        
        // Setup descs
        for i in 0 ... RXBufferNum - 1 {
            RXDesc[i].length = UInt16(RXBufferSize)
            RXDesc[i].buffer = UInt64(Address(UInt(bitPattern: RXBuffer.baseAddress!.advanced(by: i * RXBufferSize))).physical)
        }
        
        // Activate RX/TX-Rings
        writeCommand(register: .RXCTRL, value: RXControl.ENABLE.rawValue | RXControl.BROADCAST.rawValue | RXControl.BUFSIZE.rawValue)
        writeCommand(register: .TXCTRL, value: TXControl.ENABLE.rawValue | TXControl.PADDING.rawValue | TXControl.COLLTRESH.rawValue | (e1000e ? TXControl.COLLDIST2.rawValue | TXControl.RRTRESH.rawValue : TXControl.COLLDIST.rawValue))
        
        // Get your self together and make an MSI
        if pci.InterruptLine == 0xFF {
            if pci.InterruptPin == 0x00 {
                Log("Neither Interrupt Line nor Pin is set", level: .Error)
            }
        }
        
//        for _ in 0 ... 3000 {
//            if readCommand(register: .STATUS) & Status.LINKUP.rawValue != 0 || readCommand(register: .STATUS) & 2 != 0 {
//                break
//            }
//        }
//        
//        if !readCommand(register: .STATUS).bit(1) && !readCommand(register: .STATUS).bit(2) {
//            Log("Can't find link!", level: .Error)
//            RXBuffer.baseAddress?.deallocate(capacity: RXBuffer.count)
//            TXBuffer.baseAddress?.deallocate(capacity: TXBuffer.count)
//            RXDesc.baseAddress?.deallocate(capacity: RXDesc.count)
//            TXDesc.baseAddress?.deallocate(capacity: TXDesc.count)
//            return nil
//        } else {
//            Log("Found Link!", level: .Debug)
//        }
        
        if pci.InterruptLine != 0xFF {
            System.sharedInstance.interruptManager.setIrqHandler(Int(pci.InterruptLine), handler: InterruptHandler)
        }
        writeCommand(register: .IMASKCLR, value: 0xFFFF)
        writeCommand(register: .IMASK, value: 0xFFFF)
        dhcp.Discover()
    }
    
    deinit {
        RXBuffer.deallocate()
        TXBuffer.deallocate()
        RXDesc.deallocate()
        TXDesc.deallocate()
    }
    
    internal func InterruptHandler(irq: Int) {
        Log("Got Interrupt!", level: .Debug)
        let icr = readCommand(register: .ICAUSE)
        if icr & ICRTypes.RECEIVE.rawValue != 0 {
            let head = Int(readCommand(register: .RXDESCHEAD))
            while RXCurBuffer != head {
                var size = RXDesc[RXCurBuffer].length
                let status = RXDesc[RXCurBuffer].status
                
                if status & 1 == 0 {
                    break
                }
                
                size -= 4
                
                Log("Got Packet!\n\(RXDesc[RXCurBuffer].buffer)", level: .Debug)
                // Call API Receive
                
                RXCurBuffer += 1
                RXCurBuffer %= RXBufferNum
            }
            
            if RXCurBuffer == head {
                writeCommand(register: .RXDESCTAIL, value: UInt((head + RXBufferNum - 1) % RXBufferNum))
            } else {
                writeCommand(register: .RXDESCTAIL, value: UInt(RXCurBuffer))
            }
        } else if icr & ICRTypes.TRANSMIT.rawValue != 0 {
            //
        } else if icr & ICRTypes.LINKCHANGE.rawValue != 0 {
            let link: String
            if readCommand(register: .STATUS) & Status.LINKUP.rawValue != 0 {
                link = "up"
            } else {
                link = "down"
            }
            Log("Link \(link)", level: .Info)
        } else {
            Log("Unhandled Interrupt", level: .Warning)
        }
    }
    
    internal func sendPacket(data: UnsafeRawPointer, size: Int) {
        let cur = TXCurBuffer
        let size_: Int
        TXCurBuffer += 1
        TXCurBuffer %= TXBufferNum
        
        let head = readCommand(register: .TXDESCHEAD)
        guard TXCurBuffer != head else {
            Log("Sending queue overflow", level: .Error)
            TXCurBuffer = cur
            return
        }
        
        if size > TXBufferSize {
            size_ = TXBufferSize
        } else {
            size_ = size
        }

        memcpy(/*&TXBuffer[cur * TXBufferSize]*/TXBuffer.baseAddress!.advanced(by: cur * TXBufferSize), data, size_)

        TXDesc[cur].cmd     = TXCommand.EOP.rawValue | TXCommand.IFCS.rawValue
        TXDesc[cur].length  = UInt16(size_)
        TXDesc[cur].buffer  = UInt64(Address(UInt(bitPattern: TXBuffer.baseAddress!.advanced(by: cur * TXBufferSize))).physical)

        writeCommand(register: .TXDESCTAIL, value: UInt(TXCurBuffer))
    }
    
    func findEEPROM() -> Bool {
        var val = 0 as UInt
        writeCommand(register: .EEPROM, value: 1)
        for _ in 0 ... 1000 {
            val = readCommand(register: .EEPROM)
            if val.bit(4) {
                e1000e = true
                foundEEPROM = true
                break
            } else if val.bit(1) {
                foundEEPROM = true
                break
            }
        }
        return foundEEPROM
    }
    
    func getMAC() -> Bool {
        var val: UInt16
        val = readEEPROM(offset: 0)
        MAC.append(UInt8(val & 0xFF))
        MAC.append(UInt8(val >> 8))
        val = readEEPROM(offset: 1)
        MAC.append(UInt8(val & 0xFF))
        MAC.append(UInt8(val >> 8))
        val = readEEPROM(offset: 2)
        MAC.append(UInt8(val & 0xFF))
        MAC.append(UInt8(val >> 8))
        if val == 0xBAD {
            guard BaseAddr.address != 0 else {
                return false
            }
            let mem_base_mac_32 = UnsafeMutablePointer<UInt32>(bitPattern: BaseAddr.address.virtual + 0x5400)!
            let mem_base_mac_8  = UnsafeMutablePointer<UInt8>(bitPattern: BaseAddr.address.virtual + 0x5400)!
            if mem_base_mac_32.pointee != 0 {
                for i in 0 ... 5 {
                    MAC[i] = mem_base_mac_8.advanced(by: i).pointee
                }
            } else {
                return false
            }
        }
        return true
    }
    
    fileprivate func readEEPROM(offset: UInt, startFlag: UInt, doneBit: Int, moveByBits: Int) -> UInt16 {
        var eerd = 0 as UInt
        writeCommand(register: .EEPROM, value: startFlag | (offset << moveByBits))
        for _ in 0 ... 1000 {
            eerd = readCommand(register: .EEPROM)
            if eerd.bit(doneBit) {
                return UInt16((eerd >> 16) & 0xFFFF)
            }
        }
        return 0xBAD
    }
    
    func readEEPROM(offset: UInt, startFlag: UInt = 1) -> UInt16 {
        let doneBit: Int
        let moveByBits: Int
        if e1000e {
            doneBit = 4
            moveByBits = 8
        } else {
            doneBit = 1
            moveByBits = 2
        }
        return readEEPROM(offset: offset, startFlag: startFlag, doneBit: doneBit, moveByBits: moveByBits)
    }
    
    func writeCommand(register: Registers, value: UInt) {
        if BaseAddr.address != 0 {
            let ptr = UnsafeMutablePointer<UInt32>(bitPattern: BaseAddr.address.virtual + register.rawValue)!
            ptr.pointee = UInt32(truncatingIfNeeded: value)
        } else {
            outl(UInt32(BaseAddr.port), UInt32(register.rawValue))
            outl(UInt32(BaseAddr.port) + 4, UInt32(value))
        }
    }
    
    func readCommand(register: Registers) -> UInt {
        if BaseAddr.address != 0 {
            let ptr = UnsafePointer<UInt32>(bitPattern: BaseAddr.address.virtual + register.rawValue)!
            return UInt(ptr.pointee)
        } else {
            outl(UInt32(BaseAddr.port), UInt32(register.rawValue))
            return UInt(inl(UInt32(BaseAddr.port) + 4))
        }
    }
}
