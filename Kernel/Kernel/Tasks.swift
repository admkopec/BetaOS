//
//  Tasks.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/19/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

fileprivate let stackPages = 2
fileprivate let stackSize = stackPages * Int(vm_page_size)
fileprivate var tasks: [Task] = []
fileprivate var KernelTask: [Task] = []
fileprivate var currentTask  = 0
fileprivate var previousTask = 0
fileprivate var nextPID: UInt = 1
fileprivate var switchNow = false

@discardableResult
func addTask(name: String, task: @escaping @convention(c)() -> ()) -> UInt {
    let newTask = Task(name: name, entry: task)
    kprint("Adding task:", newTask.description)
    tasks.append(newTask)
    return newTask.pid
}

@_silgen_name("sched_yield")
@discardableResult  // return value is only to satisfy the C definition
public func yield() -> Int {
    switchTasks()
    return 0
}

func saveKernelState() -> Void {
    KernelTask.append(Task(name: "Kernel", entry: kernelMain))
//    x86_64_context_save_state(KernelTask[0].rsp)
}

func runFirstTask() -> Void {
    switchNow = true
    x86_64_context_switch_first(tasks[currentTask].rsp)
}

//func removeTask(number: Int) -> Void {
//    tasks.remove(at: number)
//}

@_silgen_name("switchTasks")
func switchTasks() -> Void {
    if switchNow {
        if tasks.count > 0 {
            previousTask = currentTask
            currentTask = (currentTask + 1) % tasks.count
            x86_64_context_switch(tasks[previousTask].rsp, tasks[currentTask].rsp)
        } else {
//            x86_64_context_switch_first(KernelTask[0].rsp)
        }
    }
}

struct Task: CustomStringConvertible {
    let Name: String
    let stack: UnsafeMutableRawPointer
    var state: UnsafeMutablePointer<context_switch_regs>
//    var state: UnsafeMutablePointer<x86_kernel_state_t>
    
    let pid: UInt
    var rsp: UnsafeMutablePointer<UInt>
    
    var description: String {
        return "\(Name)\nRAX: \(String(state.pointee.rax, radix: 16)) RBX: \(String(state.pointee.rbx, radix: 16)) RCX: \(String(state.pointee.rcx, radix: 16)) RDX: \(String(state.pointee.rdx, radix: 16))\nRSI: \(String(state.pointee.rsi, radix: 16)) RDI: \(String(state.pointee.rdi, radix: 16)) RBP: \(String(state.pointee.rbp, radix: 16)) RSP: \(String(state.pointee.rsp, radix: 16)) RIP: \(String(state.pointee.rip, radix: 16))\nRFLAGS: \(String(state.pointee.eflags, radix: 16))"
    }
    
    init(name: String, entry: @escaping @convention(c)() -> ()) {
        Name = name
        pid = nextPID
        nextPID += 1
        let addr = unsafeBitCast(entry, to: UInt64.self)
        stack = malloc(stackPages)
        kprint("")
        let stateOffset = stackSize - MemoryLayout<context_switch_regs>.size
//        let stateOffset = stackSize - MemoryLayout<x86_kernel_state_t>.size
        rsp = stack.advanced(by: stateOffset - MemoryLayout<UInt>.size).bindMemory(to: UInt.self, capacity: 1)
//        state = stack.advanced(by: stateOffset).bindMemory(to: x86_kernel_state_t.self, capacity: 1)
//        state.initialize(to: x86_kernel_state_t())
//        state.pointee.k_r12 = 0
//        state.pointee.k_r13 = 0
//        state.pointee.k_r14 = 0
//        state.pointee.k_r15 = 0
//        state.pointee.k_rbp = 0
//        state.pointee.k_rbx = 0xbbbbbbbbbbbbbbbb
//        state.pointee.k_rip = UInt64(addr)
        state = stack.advanced(by: stateOffset).bindMemory(to: context_switch_regs.self, capacity: 1)
        state.initialize(to: context_switch_regs())
        state.pointee.es = UInt64(USER_DATA_SELECTOR) // UInt64(0x68)
        state.pointee.ds = UInt64(USER_DATA_SELECTOR)
        state.pointee.ss = UInt64(USER_DATA_SELECTOR)
        state.pointee.rax = 0xaaaaaaaaaaaaaaaa
        state.pointee.rbx = 0xbbbbbbbbbbbbbbbb
        state.pointee.rcx = 0xcccccccccccccccc
        state.pointee.rdx = 0xdddddddddddddddd
        state.pointee.fs  = 0
//        state.pointee.gs  = 0
        state.pointee.rip = UInt64(addr)
        state.pointee.cs = UInt64(USER_CODE_SELECTOR)
        state.pointee.eflags = 514
//      Alignment hack. See ALIGN_STACK / UNALIGN_STACK in entry.asm
        let topOfStack = stack.advanced(by: stackSize)
        state.pointee.rsp = UInt64(UInt(bitPattern: topOfStack))
//        state.pointee.k_rsp = UInt64(UInt(bitPattern: topOfStack))
        rsp.pointee = UInt(bitPattern: state)
        rsp -= 1
    }
}
