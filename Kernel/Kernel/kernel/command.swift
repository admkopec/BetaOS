//
//  command.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

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
        
        let foreground = Color(red: (redF ?? 0), green: (greenF ?? 0), blue: (blueF ?? 0), alpha: 0xFF)
        let background = Color(red: (redB ?? 0), green: (greenB ?? 0), blue: (blueB ?? 0), alpha: 0xFF)
        
        if foreground.value == background.value {
            kprint("Foregroud can't be tha same as background")
        }
        change_color(foreground.value, background.value)
    } else if arguments.count == 9 {
        let redF   = Int(arguments[1], radix: 16)
        let greenF = Int(arguments[2], radix: 16)
        let blueF  = Int(arguments[3], radix: 16)
        let alphaF = Int(arguments[4], radix: 16)
        let redB   = Int(arguments[5], radix: 16)
        let greenB = Int(arguments[6], radix: 16)
        let blueB  = Int(arguments[7], radix: 16)
        let alphaB = Int(arguments[8], radix: 16)
        
        let foreground = Color(red: (redF ?? 0), green: (greenF ?? 0), blue: (blueF ?? 0), alpha: (alphaF ?? 0))
        let background = Color(red: (redB ?? 0), green: (greenB ?? 0), blue: (blueB ?? 0), alpha: (alphaB ?? 0))
        
        if foreground.value == background.value {
            kprint("Foregroud can't be tha same as background")
        }
        change_color(foreground.value, background.value)
    } else {
        kprint("Usage: setcolor RR GG BB (AA) RR GG BB (AA)")
        kprint("                (Foreground)   (Background)")
    }
}

@_silgen_name("tasks")
public func tasks(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    kprint("Starting first task...")
    runFirstTask()
}

@_silgen_name("test_graphics")
public func test_graphics(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    System.sharedInstance.Video.draw(roundedRectangle: RoundedRectangle(position: Position(x: 200, y: 200), size: Size(width: 200, height: 200), color: Color(red: 0.9204662442, green: 0.1463340819, blue: 0.1714396775, alpha: 1), radius: 20, filled: true))
    
    System.sharedInstance.Video.draw(circle: Circle(position: Position(x: 500, y: 500), color: Color(red: 0.2560254931, green: 0.7153064609, blue: 0.2907235324, alpha: 1), filled: true, radius: 125))
    
//    let line = Line(from: Position(x: 0, y: 0), to: Position(x: 900, y: 900), color: Color(red: 0.6530236602, green: 0.8558481932, blue: 0.909222424,  alpha: 0.25))
//    System.sharedInstance.Video.draw(line: line, thickness: 15)
    System.sharedInstance.Video.draw(rectangle: Rectangle(position: Position.init(x: 120, y: 120), size: Size.init(width: 70, height: 70), color: Color(red: 0.9422255158, green: 0.4098693728, blue: 0.1537380219, alpha: 1), filled: true))
//    let rect = Rectangle(position: Position(x: 0, y: 0), size: System.sharedInstance.Video.Display.Resolution, color: Color.init(red: 0.0, green: 0.0, blue: 0.0, alpha: 0.50), filled: true)
//    System.sharedInstance.Video.draw(rectangle: rect)
}

@_silgen_name("test_new_panic")
public func test_new_graphical_panic(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    panic("That's a Test Panic")
}
