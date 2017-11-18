//
//  command.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

@_silgen_name("version")
public func version() -> Void {
    kprint("\(OS_NAME) \(VERSION_MAJOR).\(VERSION_MINOR)", terminator: "");
    if (VERSION_XMINOR > 0) {
        kprint(".\(VERSION_XMINOR)", terminator: "");
    }
    kprint(" \(BUILD_TYPE)(\(String(BUILD_NUMBER, radix: 16, uppercase: true)))")
    
    kprint(" ")
    
    kprint("\(System.sharedInstance.DeviceName):")
    kprint("    CPU Vendor \(String(&cpuid_info().pointee.cpuid_vendor.0, maxLength: MemoryLayout.size(ofValue: cpuid_info().pointee.cpuid_vendor)))")
    kprint("    CPU \(String(&cpuid_info().pointee.cpuid_brand_string.0, maxLength: MemoryLayout.size(ofValue: cpuid_info().pointee.cpuid_brand_string)))")
    kprint("    Memory \(Int(Platform_state.bootArgs.pointee.PhysicalMemorySize) / GB) GB")
    kprint("    Serial Number: \(System.sharedInstance.SerialNumber)")
    
    kprint(" ")
    
    kprint(COPYRIGHT)
}

@_silgen_name("printLoadedModules")
public func printLoadedModules() -> Void {
    kprint("Loaded C++ Modules:")
    PrintLoadedModules()
    kprint("\(System.sharedInstance.modulesController)")
}

@_silgen_name("time_")
public func time_(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    
    let UNIXtime   = time(nil)
    let TimeStruct = gmtime(nil)
    var hour   = TimeStruct!.pointee.tm_hour
    let minute = TimeStruct!.pointee.tm_min
    let second = TimeStruct!.pointee.tm_sec
    let wday   = TimeStruct!.pointee.tm_wday
    let mday   = TimeStruct!.pointee.tm_mday
    let mon    = TimeStruct!.pointee.tm_mon
    var year   = TimeStruct!.pointee.tm_year
    
    year += 1900
    var pmam = ""
    
    if (hour<12 && hour>0) {
        pmam="AM";
    } else if (hour>12 && hour<24) {
        hour-=12;
        pmam="PM";
    } else if (hour==12) {
        pmam="PM";
    } else if (hour==0 || hour==24) {
        hour=12;
        pmam="AM";
    }
    
    var dayofweeklong = ""
    var dayofweekshort = ""
    
    switch (wday) {
    case 0:
        dayofweeklong = "Sunday";
        dayofweekshort = "Sun";
        break;
    case 1:
        dayofweeklong = "Monday";
        dayofweekshort = "Mon";
        break;
    case 2:
        dayofweeklong = "Tuesday";
        dayofweekshort = "Tue";
        break;
    case 3:
        dayofweeklong = "Wednesday";
        dayofweekshort = "Wed";
        break;
    case 4:
        dayofweeklong = "Thursday";
        dayofweekshort = "Thu";
        break;
    case 5:
        dayofweeklong = "Friday";
        dayofweekshort = "Fri";
        break;
    case 6:
        dayofweeklong = "Saturday";
        dayofweekshort = "Sat";
        break;
    default:
        break;
    }
    var monthl = ""
    switch (mon) {
    case 1:
        monthl="January";
        break;
    case 2:
        monthl="February";
        break;
    case 3:
        monthl="March";
        break;
    case 4:
        monthl="April";
        break;
    case 5:
        monthl="May";
        break;
    case 6:
        monthl="June";
        break;
    case 7:
        monthl="July";
        break;
    case 8:
        monthl="August";
        break;
    case 9:
        monthl="September";
        break;
    case 10:
        monthl="October";
        break;
    case 11:
        monthl="November";
        break;
    case 12:
        monthl="December";
        break;
        
    default:
        break;
    }
    
    if arguments.count == 2 {
        if arguments[1] == "long" {
            kprint("\(hour):\(String(minute).leftPadding(toLength: 2, withPad: "0")):\(String(second).leftPadding(toLength: 2, withPad: "0")) \(pmam)")
            kprint("\(dayofweeklong), \(monthl) \(mday), \(year)")
        } else if arguments[1] == "absolute" {
            kprint("Absolute time is: \(UNIXtime)")
            kprint("Mach Absolute time is: \(mach_absolute_time())")
        } else {
            kprint("Usage: time [long | absolute]")
            return
        }
    } else if arguments.count == 1 {
        kprint("\(dayofweekshort) \(hour):\(String(minute).leftPadding(toLength: 2, withPad: "0")) \(pmam)")
    } else {
        kprint("Usage: time [long | absolute]")
        return
    }
}

@_silgen_name("set_color")
public func set_color(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    if arguments.count == 7 {
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
