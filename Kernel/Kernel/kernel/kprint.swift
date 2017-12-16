//
//  kprint.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

/**
 Writes the textual representations of the given items in kernel space.
 
 - parameter items: Zero or more items to print.
 - parameter separator: A string to print between each item. The default is a single space (`" "`).
 - parameter terminator: The string to print after all items have been printed. The default is a newline (`"\n"`).
 
 */
public func kprint(_ items: String..., separator: String = " ", terminator: String = "\n") -> Void {
    let item = items.map {"\($0)"} .joined(separator: separator)
    if item.isEmpty {
        return
    }
    for 😀 in item.unicodeScalars {
        putchar(Int32(😀.value))
    }
    kprint(terminator, terminator: "")
}

/**
 Writes the textual representations of the given item in a task. Tasks do not support .map and .joined
 
 - parameter items: Zero or one item to print.
 - parameter terminator: The string to print after the item has been printed. The default is a newline (`"\n"`).
 
 */
func tprint(_ item: String, terminator: String = "\n") -> Void {
    if item.isEmpty {
        return
    }
    for 😀 in item.unicodeScalars {
        putchar(Int32(😀.value))
    }
    tprint(terminator, terminator: "")
}

func writeString(atLocation: Position, string: String) -> Void {
    if string.isEmpty {
        return
    }
    var x = atLocation.X
    var y = atLocation.Y
    for 😀 in string.unicodeScalars {
        if 😀.value <= 255 {
            if 😀 == "\n" {
                y += 16
                continue
            }
            paint_char(UInt32(x), UInt32(y), UInt8(😀.value))
            x += 8
        }
    }
}

internal func panic(_ items: String..., separator: String = " ") -> Void {
    let item = items.map {"\($0)"} .joined(separator: separator)
    panic_common(item)
}

@_silgen_name("panic_C_wrapper")
internal func panic_C_wrapper(_ item: UnsafePointer<CChar>) -> Void {
    panic_common(String(cString: item))
}

internal func panic_common(_ item: String) -> Void {
    experimental = false
    System.sharedInstance.Video.draw(rectangle: Rectangle(position: Position.init(x: 0, y: 0), size: System.sharedInstance.Video.Display.Resolution, color: Color.init(red: 0, green: 0, blue: 0, alpha: 0.4), filled: true))
    let Center = Position(x: System.sharedInstance.Video.Display.Resolution.Width / 2, y: System.sharedInstance.Video.Display.Resolution.Height / 2)
    System.sharedInstance.Video.draw(roundedRectangle: RoundedRectangle(position: Position.init(x: Center.X - 253, y: Center.Y - 140), size: Size.init(width: 506, height: 281), color: Color(red: 0.6000000238, green: 0.6000000238, blue: 0.6000000238, alpha: 0.95), radius: 15, filled: true))
    System.sharedInstance.Video.draw(roundedRectangle: RoundedRectangle(position: Position.init(x: Center.X - 250, y: Center.Y - 137), size: Size.init(width: 500, height: 275), color: Color(red: 1, green: 0.1857388616, blue: 0.5733950138, alpha: 1), radius: 15, filled: true))
}
