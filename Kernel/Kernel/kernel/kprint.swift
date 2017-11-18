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
//    var isNext = false
//    for 😀 in item.utf8 {
//        if 😀 >= 0xC0 && 😀 <= 0xDF {
//            if 😀 == 0xC2 {
//                continue
//            } else if 😀 == 0xC3 {
//                isNext = true
//                continue
//            }
//        }
//        if !isNext {
//            putchar(Int32(😀))
//        } else {
//            putchar(Int32(😀 | 0xC0))
//            isNext = false
//        }
//    }
    kprint(terminator, terminator: "")
}
