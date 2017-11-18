//
//  Loggable.swift
//  Loggable
//
//  Created by Adam Kopeć on 11/5/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

public protocol Loggable {
    var Name: String { get }
}


public enum LogLevel: Int, CustomStringConvertible {
    case Verbose = 0
    case Debug   = 1
    case Info    = 2
    case Warning = 3
    case Error   = 4
    
    public var description: String {
        get {
            switch self {
            case .Verbose:
                return "Verbose"
            case .Debug:
                return "Debug"
            case .Info:
                return "Info"
            case .Warning:
                return "Warning"
            case .Error:
                return "Error"
            }
        }
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
        let OldColor = get_font_color()
        let color: UInt32
        let isConfigurationValid: Bool
        let item = items.map {"\($0)"} .joined(separator: separator)
        switch level {
        case .Info:
            color = 0x0000A5FF
            isConfigurationValid = true
        case .Warning:
            color = 0x00FFAE42
            isConfigurationValid = true
        case .Error:
            color = 0x00ED4337
            isConfigurationValid = true
        case .Verbose:
            color = OldColor
            #if DEBUG
                isConfigurationValid = true
            #else
                isConfigurationValid = false
            #endif
        case .Debug:
            color = 0x0009D400
            #if DEBUG
                isConfigurationValid = true
            #else
                isConfigurationValid = false
            #endif
        }
        change_font_color(color)
        print(Name + " " + "[" + level.description + "]" + ":", item, separator: separator, terminator: terminator)
        change_font_color(OldColor)
    }
}
