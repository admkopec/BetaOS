//
//  VMwareTools.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

class VMwareTools: Module {
    let Name = "VMware Tools"
    var description: String {
        return ""
    }
    
    var MessageChannel: (Protocol: UInt32, ID: UInt16, Initialized: Bool)
    
    init() {
        MessageChannel = (Protocol: 0, ID: 0, Initialized: false)
        GetRPCIChannel()
    }
    
    func RegisterResolutionCapabilities() {
        RPC(string: "tools.capability.resolution_set 1")
        RPC(string: "tools.capability.resolution_server toolbox 1")
        RPC(string: "tools.capability.display_topology_set 1")
        RPC(string: "tools.capability.color_depth_set 1")
        RPC(string: "tools.capability.resolution_min 0 0")
        RPC(string: "tools.capability.unity 1")
    }
    
    func RPC(string: String) {
        var arr = string.utf8CString
        CheckedRPCI(request: &arr[0], size: UInt32(arr.count))
    }
    
    func CheckedRPCI(request: UnsafeRawPointer, size: UInt32) {
        let replyBuffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 16)
        let replyLength = RPCI(request: request, size: size, replyBuffer: replyBuffer, replyBufferLength: 16)
        if replyLength < 1 || replyBuffer.pointee != 0 {
            Log("RPCI response invalid!", level: .Error)
            return
        }
    }
    
    fileprivate func MessageOpen(withProtocol: UInt32) {
        var rax = 0 as UInt32, rbx = 0 as UInt32, rcx = 0 as UInt32, rdx = 0 as UInt32, rsi = 0 as UInt32, rdi = 0 as UInt32
        rcx = UInt32(BDOOR_CMD_MESSAGE | 0x00000000)
        rbx = withProtocol
        BackdoorIn(&rax, &rbx, &rcx, &rdx, &rsi, &rdi)
        if rcx & 0x00010000 == 0 {
            Log("Failed to open message channel with protocol \(withProtocol)", level: .Error)
        }
        MessageChannel.Protocol = withProtocol
        MessageChannel.ID       = UInt16(truncatingIfNeeded: rdx >> 16)
        MessageChannel.Initialized = true
    }
    
    fileprivate func GetRPCIChannel(){
        if !MessageChannel.Initialized {
            MessageOpen(withProtocol: 0x49435052)
        }
    }
    
    fileprivate func MessageSend(request: UnsafeRawPointer, size: UInt32) {
        var rax = 0 as UInt32, rbx = 0 as UInt32, rcx = 0 as UInt32, rdx = 0 as UInt32, rsi = 0 as UInt32, rdi = 0 as UInt32
        rcx = UInt32(BDOOR_CMD_MESSAGE | 0x00010000)
        rbx = size
        rdx = UInt32(MessageChannel.ID) << 16
        BackdoorIn(&rax, &rbx, &rcx, &rdx, &rsi, &rdi)
        if size == 0 {
            return
        }
        
        guard (rcx >> 16) & 0x81 == 0x81 else {
            Log("Only High Bandwidth backdoor port is supported.", level: .Error)
            return
        }
        
        rbx = UInt32(BDOORHB_CMD_MESSAGE | 0x00010000)
        rcx = size
        rdx = UInt32(MessageChannel.ID) << 16
        rsi = UInt32(kvtophys(UInt64(UInt(bitPattern: request))))
        BackdoorHighBandwidthOut(&rax, &rbx, &rcx, &rdx, &rsi, &rdi)
        guard ebx & 0x00010000 == 1 else {
            Log("Failed to send message", level: .Error)
            return
        }
    }
    
    fileprivate func MessageRecive(replyBuffer: UnsafeMutableRawPointer, replyBufferLength: UInt32) -> UInt32 {
        var size = 0 as UInt32
        var rax = 0 as UInt32, rbx = 0 as UInt32, rcx = 0 as UInt32, rdx = 0 as UInt32, rsi = 0 as UInt32, rdi = 0 as UInt32
        rcx = UInt32(BDOOR_CMD_MESSAGE | 0x00030000)
        rdx = UInt32(MessageChannel.ID) << 16
        BackdoorIn(&rax, &rbx, &rcx, &rdx, &rsi, &rdi)
        guard rdx >> 16 == 0x0001 else {
            Log("Error reciving message size", level: .Error)
            return 0
        }
        size = rbx
        guard size <= replyBufferLength else {
            Log("Buffer overflow", level: .Error)
            return 0
        }
        guard (rcx >> 16) & 0x83 == 0x83 else {
             Log("Only High Bandwidth backdoor port is supported.", level: .Error)
            return 0
        }
        rbx = UInt32(BDOORHB_CMD_MESSAGE | 0x00010000)
        rcx = size
        rdx = UInt32(MessageChannel.ID) << 16
        rdi = UInt32(kvtophys(UInt64(UInt(bitPattern: replyBuffer))))
        BackdoorHighBandwidthIn(&rax, &rbx, &rcx, &rdx, &rsi, &rdi)
        guard rbx & 0x00010000 == 1 else {
            Log("Failed to recieve message", level: .Error)
            return 0
        }
        rcx = UInt32(BDOOR_CMD_MESSAGE | 0x00050000)
        rbx = 0x1
        rdx = UInt32(MessageChannel.ID) << 16
        BackdoorIn(&rax, &rbx, &rcx, &rdx, &rsi, &rdi)
        return size
    }
    
    func RPCI(request: UnsafeRawPointer, size: UInt32, replyBuffer: UnsafeMutableRawPointer, replyBufferLength: UInt32) -> UInt32 {
        GetRPCIChannel()
        MessageSend(request: request, size: size)
        return MessageRecive(replyBuffer: replyBuffer, replyBufferLength: replyBufferLength)
    }
}
