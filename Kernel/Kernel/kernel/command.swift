//
//  command.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import CommonExtensions
import Loggable
import Graphics

@_silgen_name("version")
public func version() -> Void {
    kprint("\(OSName) \(VersionMajor).\(VersionMinor)", terminator: "");
    if (VersionXMinor > 0) {
        kprint(".\(VersionXMinor)", terminator: "");
    }
    kprint(" \(BUILD_TYPE)(\(String(BuildNumber, radix: 16, uppercase: true)))")
    
    kprint(" ")
    
    kprint("\(System.sharedInstance.DeviceName):")
    kprint("    CPU Vendor \(String(&cpuid_info().pointee.cpuid_vendor.0, maxLength: MemoryLayout.size(ofValue: cpuid_info().pointee.cpuid_vendor)))")
    kprint("    CPU \(String(&cpuid_info().pointee.cpuid_brand_string.0, maxLength: MemoryLayout.size(ofValue: cpuid_info().pointee.cpuid_brand_string)))")
    kprint("    Memory \(Int(Platform_state.bootArgs.pointee.PhysicalMemorySize) / GB) GB")
    kprint("    GPU \(System.sharedInstance.Video.description)")
    kprint("    Serial Number: \(System.sharedInstance.SerialNumber)")
    
    kprint(" ")
    
    kprint(Copyright)
}

@_silgen_name("printLoadedModules")
public func printLoadedModules() -> Void {
    kprint("Loaded C++ Modules:")
    PrintLoadedModules()
    kprint("\(System.sharedInstance.modulesController)")
}

@_silgen_name("shutdown_")
public func SystemShutdown() -> Void {
    System.sharedInstance.shutdown()
}

// FIXME: Correct strange time values
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
    System.sharedInstance.Video.mainView.draw(roundedRectangle: RoundedRectangle(position: Position(x: 200, y: 200), size: Size(width: 200, height: 200), color: #colorLiteral(red: 0.9204662442, green: 0.1463340819, blue: 0.1714396775, alpha: 1), radius: 20, filled: true))
    
    System.sharedInstance.Video.mainView.draw(circle: Circle(position: Position(x: 500, y: 500), color: #colorLiteral(red: 0.2560254931, green: 0.7153064609, blue: 0.2907235324, alpha: 1), filled: true, radius: 100))
    
    System.sharedInstance.Video.mainView.draw(circle: Circle(position: Position(x: 500, y: 500), color: #colorLiteral(red: 0.2560254931, green: 0.7153064609, blue: 0.2907235324, alpha: 1), filled: false, radius: 125))
    
//    let line = Line(from: Position(x: 0, y: 0), to: Position(x: 900, y: 900), color: #colorLiteral(red: 0.6530236602, green: 0.8558481932, blue: 0.909222424,  alpha: 0.25))
//    System.sharedInstance.Video.draw(line: line, thickness: 15)
    System.sharedInstance.Video.mainView.draw(rectangle: Rectangle(position: Position.init(x: 120, y: 120), size: Size.init(width: 70, height: 70), color: #colorLiteral(red: 0.9422255158, green: 0.4098693728, blue: 0.1537380219, alpha: 1), filled: true))
//    let rect = Rectangle(position: Position(x: 0, y: 0), size: System.sharedInstance.Video.Display.Resolution, color: #colorLiteral(red: 0.0, green: 0.0, blue: 0.0, alpha: 0.50), filled: true)
//    System.sharedInstance.Video.draw(rectangle: rect)
    System.sharedInstance.Video.mainView.draw(quadraticBézier: QuadraticBézier(p0: Position.init(x: 100, y: 100), p1: Position.init(x: 150, y: 50), p2: Position.init(x: 200, y: 100), color: #colorLiteral(red: 0.9204662442, green: 0.1463340819, blue: 0.1714396775, alpha: 1)))
    let newView = View(position: Position.init(x: 250, y: 250), size: Size.init(width: 300, height: 100))
    newView.draw(line: Line.init(from: Position.init(x: 10, y: 10), to: Position.init(x: 290, y: 10), color: #colorLiteral(red: 0.2588235438, green: 0.7568627596, blue: 0.9686274529, alpha: 1)), thickness: 70)
//    System.sharedInstance.Video.mainView += newView

//    let newWindow = Window.Popup
//    newWindow.view.draw(roundedRectangle: RoundedRectangle(position: Position(x: 0, y: 0), size: newWindow.view.Display.Resolution, color: #colorLiteral(red: 0, green: 0.9810667634, blue: 0.5736914277, alpha: 1), radius: 15, filled: true))
//    newWindow.windowView += newWindow.view
//    System.sharedInstance.Video.mainView += newWindow.windowView
    let size = Size(width: 500, height: 275)
    let newTitleBarWindow = Window(size: size)
    newTitleBarWindow.view.draw(roundedRectangle: RoundedRectangle(position: Position(x: 0, y: 0), size: newTitleBarWindow.view.Display.Resolution, color: #colorLiteral(red: 0, green: 0.9810667634, blue: 0.5736914277, alpha: 1), radius: 15, filled: true))
    newTitleBarWindow.windowView += newTitleBarWindow.view
//    System.sharedInstance.Video.mainView += newTitleBarWindow.windowView
}

@_silgen_name("test_new_panic")
public func test_new_graphical_panic(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Void {
    panic("That's a Test Panic")
}

@_silgen_name("openFile")
public func openFile(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    if arguments.count < 2 || arguments.count != 3 || arguments[1] == "help" {
        kprint("Usage: open PartitionName PathToFile ")
        kprint("       Opens file for reading")
        return
    }
    guard System.sharedInstance.Drives.count >= 1 else {
        kprint("No Drives available")
        return
    }
    var partition: Partition?
    for disk in System.sharedInstance.Drives {
        for partition_ in disk.Partitions {
            if partition_.VolumeName == arguments[1] || partition_.AlternateName == arguments[1] {
                partition = partition_
            }
        }
    }
    guard partition != nil else {
        kprint("Partition not found!")
        return
    }
    
    if let file = TextFile(partition: partition!, path: arguments[2]) {
        kprint("\(file.text)")
    } else if let file = PNG(partition: partition!, path: arguments[2]) {
        kprint("It's a PNG")
        kprint("\(file)")
    } else {
        kprint("Couldn't open the file")
    }
}

@_silgen_name("writeFile")
public func writeFile(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    if arguments.count < 2 || arguments.count != 4 || arguments[1] == "help" {
        kprint("Usage: write PartitionName PathToFile Contents Of File")
        kprint("       Writes file to disk")
        return
    }
    guard System.sharedInstance.Drives.count >= 1 else {
        kprint("No Drives available")
        return
    }
    var partition: Partition?
    for disk in System.sharedInstance.Drives {
        for partition_ in disk.Partitions {
            if partition_.VolumeName == arguments[1] || partition_.AlternateName == arguments[1] {
                partition = partition_
            }
        }
    }
    guard partition != nil else {
        kprint("Partition not found!")
        return
    }
    
    guard let file = TextFile(partition: partition!, path: arguments[2]) else { kprint("Can't write a new file yet!"); return }
    file.text = arguments[3]
}

@_silgen_name("listFiles")
public func listFiles(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    if arguments.count < 2 || arguments.count != 3 || arguments[1] == "help" {
        kprint("Usage: list PartitionName PathToFolder")
        kprint("       Lists all files and folders in selected directory")
        return
    }
    guard System.sharedInstance.Drives.count >= 1 else {
        kprint("No Drives available")
        return
    }
    var partition: Partition?
    for disk in System.sharedInstance.Drives {
        for partition_ in disk.Partitions {
            if partition_.VolumeName == arguments[1] || partition_.AlternateName == arguments[1] {
                partition = partition_
            }
        }
    }
    guard partition != nil else {
        kprint("Partition not found!")
        return
    }
    if let entries = partition!.ReadFolder(fromPath: arguments[2]) {
        for entry in entries {
            guard entry.Flags != 0x00 && entry.Flags & 0x02 == 0 && entry.Flags & 0x08 == 0 && entry.Flags & 0x04 == 0 else {
                continue
            }
            kprint("Name: \(entry.Name + (entry.Extension != "" ? "." + entry.Extension : ""))  \(entry.Size) Bytes (Flags: \(String(entry.Flags, radix: 16)))")
        }
    } else {
        kprint("Couldn't find selected folder")
        return
    }
}

@_silgen_name("runExec")
public func runExec(argc: Int, argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) {
    var arguments : [String] = [String(cString: argv[0]!)]
    if argc > 1 {
        for arg in 1 ..< argc {
            arguments.append(String(cString: argv[arg]!))
        }
    }
    if arguments.count < 2 || arguments.count != 3 || arguments[1] == "help" {
        kprint("Usage: run PartitionName PathToExecutable ")
        kprint("       Runs an executable")
        return
    }
    guard System.sharedInstance.Drives.count >= 1 else {
        kprint("No Drives available")
        return
    }
    var partition: Partition?
    for disk in System.sharedInstance.Drives {
        for partition_ in disk.Partitions {
            if partition_.VolumeName == arguments[1] || partition_.AlternateName == arguments[1] {
                partition = partition_
            }
        }
    }
    guard partition != nil else {
        kprint("Partition not found!")
        return
    }
    
    if let file = partition!.ReadFile(fromPath: arguments[2]) {
        let Mainfunction: @convention(c) () -> Int32 = UnsafeMutablePointer<@convention(c) () -> Int32>(OpaquePointer(file.Data.baseAddress!.advanced(by: 0xf90))).pointee
        kprint("Data: \(file.Data[0xf90]), \(file.Data[0xf91]), \(file.Data[0xf92])")
        kprint("Return value is: \(Mainfunction())")
        file.Data.deallocate()
    } else {
        kprint("Couldn't open the file")
    }
}
