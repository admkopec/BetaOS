//
//  PIT8254.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/18/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import CustomArrays
import Loggable

final class PIT8254: Module {
    fileprivate let BaseFrequency = 1193182
    fileprivate let CommandPort   = 0x43 as UInt32
    var Name: String = "PIT8254"
    var description: String {
        return "Legacy PIT8254 Timer Device Controller"
    }
    
    // Raw Value is the I/O port
    enum TimerChannel: UInt32 {
        case CHANNEL_0 = 0x40
        // CHANNEL_1 is not valid
        case CHANNEL_2 = 0x42
        
        var channelSelect: ChannelSelect {
            switch self {
            case .CHANNEL_0: return ChannelSelect.CHANNEL0
            case .CHANNEL_2: return ChannelSelect.CHANNEL2
            }
        }
    }
    
    
    // Mode / Command Register commands
    enum ChannelSelect: UInt8 {
        static let mask: UInt8 = 0b11000000
        
        case CHANNEL0 = 0b00000000
        case CHANNEL1 = 0b01000000
        case CHANNEL2 = 0b10000000
        case READBACK = 0b11000000
        
        init(channel: UInt8) {
            self.init(rawValue: channel & ChannelSelect.mask)!
        }
    }
    
    
    enum AccessMode: UInt8 {
        static let mask: UInt8 = 0b00110000
        
        case LATCH_COUNT  = 0b00000000
        case LO_BYTE_ONLY = 0b00010000
        case HI_BYTE_ONLY = 0b00100000
        case LO_HI_BYTE   = 0b00110000
        
        init(mode: UInt8) {
            self.init(rawValue: mode & AccessMode.mask)!
        }
    }
    
    
    enum OperatingMode: UInt8 {
        static let mask: UInt8 = 0b00001110
        
        case MODE_0 = 0b00000000
        case MODE_1 = 0b00000010
        case MODE_2 = 0b00000100
        case MODE_3 = 0b00000110
        case MODE_4 = 0b00001000
        case MODE_5 = 0b00001010
        case MODE_6 = 0b00001100    // Actually mode 2
        case MODE_7 = 0b00001110    // Actually mode 3
        
        init(mode: UInt8) {
            var value = UInt8(mode & OperatingMode.mask);
            if (value == OperatingMode.MODE_6.rawValue) {
                value = OperatingMode.MODE_2.rawValue
            } else if (value == OperatingMode.MODE_7.rawValue) {
                value = OperatingMode.MODE_3.rawValue
            }
            self.init(rawValue: value)!
        }
    }
    
    
    enum NumberMode: UInt8 {
        static let mask: UInt8 = 0b00000001
        
        case BINARY = 0b00000000
        case BCD    = 0b00000001    // This mode is not supported
        
        init(mode: UInt8) {
            self.init(rawValue: mode & NumberMode.mask)!
        }
    }
    
    init() {
        setChannel(.CHANNEL_0, mode: .MODE_3, hz: 60)
    }
    
    fileprivate func toCommandByte(_ channel: ChannelSelect, _ access: AccessMode,
                               _ mode: OperatingMode, _ number: NumberMode) -> UInt8 {
        return channel.rawValue | access.rawValue | mode.rawValue | number.rawValue
    }
    
    
    fileprivate func fromCommandByte(_ command: UInt8) ->
        (ChannelSelect, AccessMode, OperatingMode, NumberMode) {
            return (ChannelSelect(channel: command),
                    AccessMode(mode: command),
                    OperatingMode(mode: command),
                    NumberMode(mode: command)
            )
    }
    
    fileprivate func mapChannelToSelect(_ channel: TimerChannel) -> ChannelSelect {
        switch(channel) {
        case .CHANNEL_0: return ChannelSelect.CHANNEL0
        case .CHANNEL_2: return ChannelSelect.CHANNEL2
        }
    }
    
    fileprivate func setDivisor(_ channel: TimerChannel, _ value: UInt16) {
        let v = ByteArray(value)
        outb(channel.rawValue, v[0])
        outb(channel.rawValue, v[1])
    }
    
    fileprivate func setHz(_ channel: TimerChannel, _ hz: Int) {
        let divisor = UInt16(BaseFrequency / hz)
        setDivisor(channel, divisor)
    }
    
    func setChannel(_ channel: TimerChannel, mode: OperatingMode, hz: Int) {
        let command = toCommandByte(mapChannelToSelect(channel), .LO_HI_BYTE, mode, .BINARY)
        outb(CommandPort, command)
        setHz(channel, hz)
        // Enable IRQ
        System.sharedInstance.interruptManager.setIrqHandler(0, handler: Handler)
    }
    
    // Interrupt Handler
    public func Handler(irq: Int) {
        // Do some timer specific interrupt stuff, like screen refresh or ticks
        System.sharedInstance.Video.refresh()
    }
}
