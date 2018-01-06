//
//  kprint.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

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
                x = atLocation.X
                continue
            }
            System.sharedInstance.Video.mainView.draw(character: 😀, position: Position.init(x: x, y: y))
            x += 8
        }
    }
}

internal func panic(_ items: String..., separator: String = " ") -> Void {
    let item = items.map {"\($0)"} .joined(separator: separator)
    panic_common(item)
}

@_silgen_name("panic_C_wrapper")
public func panic_C_wrapper(_ item: UnsafePointer<CChar>) -> Void {
    panic_common(String(cString: item))
}

internal func panic_common(_ item: String) -> Void {
    experimental = false
    System.sharedInstance.Video.mainView.draw(rectangle: Rectangle(position: Position.init(x: 0, y: 0), size: System.sharedInstance.Video.mainView.Display.Resolution, color: Color.init(red: 0, green: 0, blue: 0, alpha: 0.4), filled: true))
    let outsideSize = Size.init(width: 506, height: 281)
    System.sharedInstance.Video.mainView.draw(roundedRectangle: RoundedRectangle(position: Position.Center - outsideSize / 2, size: outsideSize, color: Color(red: 0.6000000238, green: 0.6000000238, blue: 0.6000000238, alpha: 0.95), radius: 15, filled: true))
    let insideColor = Color(red: 1, green: 0.1857388616, blue: 0.5733950138, alpha: 1)
    let insideSize = Size.init(width: 500, height: 275)
    System.sharedInstance.Video.mainView.draw(roundedRectangle: RoundedRectangle(position: Position.Center - insideSize / 2, size: insideSize, color: insideColor, radius: 15, filled: true))
    let KernelPanicMessage = "Kernel Panic"
    change_font_color(0xFFFFFF)
    writeString(atLocation: Position.init(x: Position.Center.X - ((KernelPanicMessage.count * 8) / 2), y: Position.Center.Y - 133), string: KernelPanicMessage)
    writeString(atLocation: Position.init(x: Position.Center.X - 245, y: Position.Center.Y - 112), string: item)
    if (!kRebootOnPanic) {
        writeString(atLocation: Position.init(x: Position.Center.X - 237, y: Position.Center.Y + 117), string: "CPU Halted")
        System.sharedInstance.Video.refresh()
        cli()
        hlt()
    } else {
        writeString(atLocation: Position.init(x: Position.Center.X - 237, y: Position.Center.Y + 117), string: "Rebooting in 3 seconds.")
        System.sharedInstance.Video.refresh()
        let startTime = time(nil)
        var Dot1Printed = false
        var Dot2Printed = false
        while startTime + 3 > time(nil) {
            if startTime + 0 < time(nil) && !Dot1Printed {
                writeString(atLocation: Position.init(x: Position.Center.X - 237 + ("Rebooting in 3 seconds.".count * 8), y: Position.Center.Y + 117), string: ".")
                System.sharedInstance.Video.refresh()
                Dot1Printed = true
            }
            if startTime + 1 < time(nil) && !Dot2Printed {
                writeString(atLocation: Position.init(x: Position.Center.X - 237 + ("Rebooting in 3 seconds..".count * 8), y: Position.Center.Y + 117), string: ".")
                System.sharedInstance.Video.refresh()
                Dot2Printed = true
            }
        }
        reboot_system(true)
    }
}
