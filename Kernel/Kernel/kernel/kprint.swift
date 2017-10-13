//
//  kprint.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

/**
 Writes the textual representations of the given items in kernel space.
 
 - parameter item: Zero or more items to print.
 - parameter terminator: The string to print after all items have been printed. The default is a newline ("\n").
 
 */

func kprint(_ item: String, terminator: String = "\n") -> Void {
    if item.isEmpty {
        return
    }
    var isNext = false
    for 😀 in item.utf8 {
        if 😀 >= 0xC0 && 😀 <= 0xDF {
            if 😀 == 0xC2 {
                continue
            } else if 😀 == 0xC3 {
                isNext = true
                continue
            }
        }
        if !isNext {
            putchar(Int32(😀))
        } else {
            putchar(Int32(😀 | 0xC0))
            isNext = false
        }
    }
    kprint(terminator, terminator: "")
}
