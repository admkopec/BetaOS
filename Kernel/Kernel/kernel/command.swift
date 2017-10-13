//
//  command.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

@_silgen_name("version")
func version() -> Void {
    kprint("\(OS_NAME) \(VERSION_MAJOR).\(VERSION_MINOR)", terminator: "");
    if (VERSION_XMINOR > 0) {
        kprint(".\(VERSION_XMINOR)", terminator: "");
    }
    kprint(" \(BUILD_TYPE)(\(String(BUILD_NUMBER, radix: 16, uppercase: true)))")
    kprint(COPYRIGHT)
    
    let vendor = withUnsafePointer(to: &cpuid_info().pointee.cpuid_vendor) {
        $0.withMemoryRebound(to: UInt8.self, capacity: MemoryLayout.size(ofValue: cpuid_info().pointee.cpuid_vendor)) {
            String(cString: $0)
        }
    }
    
    let brand = withUnsafePointer(to: &cpuid_info().pointee.cpuid_brand_string) {
        $0.withMemoryRebound(to: UInt8.self, capacity: MemoryLayout.size(ofValue: cpuid_info().pointee.cpuid_brand_string)) {
            String(cString: $0)
        }
    }
    
    kprint("CPU Vendor \(vendor)")
    kprint("CPU \(brand)")
    kprint("Memory \(Platform_state.bootArgs.pointee.PhysicalMemorySize / GB) GB")
}

@_silgen_name("printLoadedModules")
func printLoadedModules() -> Void {
    kprint("Loaded C++ Modules:")
    PrintLoadedModules()
//    kprint("Loaded Swift Modules:")
//    ModulesController.sharedInstance.printLoadedModules()
}

@_silgen_name("time_")
func time_(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    gettime()
    if argc == 2 {
        if arguments[1] == "long" {
            kprint("\(hour):\(String(minute).leftPadding(toLength: 2, withPad: "0")):\(String(second).leftPadding(toLength: 2, withPad: "0")) \(String(cString: pmam))")
            kprint("\(String(cString: dayofweeklong)), \(String(cString: monthl)) \(day), \(year)")
        } else if arguments[1] == "absolute" {
            kprint("Absolute time is: \(time())")
            kprint("Mach Absolute time is: \(mach_absolute_time())")
        } else {
            kprint("Usage: time [long | absolute]")
            return
        }
    } else if argc == 1 {
        kprint("\(String(cString: dayofweekshort)) \(hour):\(String(minute).leftPadding(toLength: 2, withPad: "0")) \(String(cString: pmam))")
    } else {
        kprint("Usage: time [long | absolute]")
        return
    }
}

@_silgen_name("set_color")
func set_color(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    if argc == 7 {
        let redF   = Int(arguments[1], radix: 16)
        let greenF = Int(arguments[2], radix: 16)
        let blueF  = Int(arguments[3], radix: 16)
        let redB   = Int(arguments[4], radix: 16)
        let greenB = Int(arguments[5], radix: 16)
        let blueB  = Int(arguments[6], radix: 16)
        
        var foreground = 0x00 as UInt32
        var background = 0x00 as UInt32
        
        foreground += UInt32(redF   ?? 0) << 16
        foreground += UInt32(greenF ?? 0) << 8
        foreground += UInt32(blueF  ?? 0) << 0
        
        background += UInt32(redB   ?? 0) << 16
        background += UInt32(greenB ?? 0) << 8
        background += UInt32(blueB  ?? 0) << 0
        
        if foreground == background {
            kprint("Foregroud can't be tha same as background")
        }
        change_color(foreground, background)
    } else {
        kprint("Usage: setcolor RR GG BB RR GG BB")
        kprint("                (Foregr) (Backgr)")
    }
}
