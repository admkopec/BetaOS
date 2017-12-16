//
//  String.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

extension String {
    init(_ rawPtr: UnsafeRawPointer, maxLength: Int) {
        if maxLength < 0 {
            self = ""
            return
        }
        let ptr = rawPtr.bindMemory(to: UInt8.self, capacity: maxLength)
        var buffer: [UInt8]
        buffer = [UInt8](UnsafeBufferPointer(start: ptr, count: maxLength))
        buffer.append(UInt8(0))
        self = String.init(cString: buffer)
    }
    
    func leftPadding(toLength: Int, withPad: String = " ") -> String {
        
        guard toLength > self.count else { return self }
        
        let padding = String(repeating: withPad, count: toLength - self.count)
        
        return padding + self
    }
}

extension Loggable {
    /**
     Writes the textual representations of the given items to default logging destination.

     - parameter items: Zero or more items to print.
     - parameter level: Level of message being logged.
     - parameter separator: A string to print between each item. The default is a single space (`" "`).
     - parameter terminator: The string to print after all items have been printed. The default is a newline (`"\n"`).

     */
    public func Log(_ items: String..., level: LogLevel, separator: String = " ", terminator: String = "\n") -> Void {
        let item = items.map {"\($0)"} .joined(separator: separator)
        let InfoColor    = Color(red: 0x00, green: 0xA5, blue: 0xFF, alpha: 0xFF)
        let DebugColor   = Color(red: 0x09, green: 0xD4, blue: 0x00, alpha: 0xFF)
        let WarningColor = Color(red: 0xFF, green: 0xAE, blue: 0x42, alpha: 0xFF)
        let ErrorColor   = Color(red: 0xED, green: 0x43, blue: 0x37, alpha: 0xFF)
        let color = get_font_color()
        switch level {
        case .Info:
            change_font_color(InfoColor.value)
            kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
        case .Verbose:
            #if DEBUG
                kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
            #endif
        case .Debug:
            #if DEBUG
                change_font_color(DebugColor.value)
                kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
            #endif
        case .Warning:
            change_font_color(WarningColor.value)
            kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
        case .Error:
            change_font_color(ErrorColor.value)
            kprint(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
        }
        change_font_color(color)
    }
}
