//
//  PrivateExtensions.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/23/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Loggable
import Graphics
import CustomArrays

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
        let InfoColor    = #colorLiteral(red:0.00, green:0.65, blue:1.00, alpha:1.0) as Color //Color(red: 0x00, green: 0xA5, blue: 0xFF, alpha: 0xFF)
        let DebugColor   = #colorLiteral(red:0.04, green:0.83, blue:0.00, alpha:1.0) as Color //Color(red: 0x09, green: 0xD4, blue: 0x00, alpha: 0xFF)
        let WarningColor = #colorLiteral(red:1.00, green:0.68, blue:0.26, alpha:1.0) as Color //Color(red: 0xFF, green: 0xAE, blue: 0x42, alpha: 0xFF)
        let ErrorColor   = #colorLiteral(red:0.93, green:0.26, blue:0.22, alpha:1.0) as Color //Color(red: 0xED, green: 0x43, blue: 0x37, alpha: 0xFF)
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

extension Position {
    static var Center: Position {   // You could try making it center of active view instead of mainView
        return self.init(x: System.sharedInstance.Video.mainView.Display.Resolution.Width/2, y: System.sharedInstance.Video.mainView.Display.Resolution.Height/2)
    }
}
