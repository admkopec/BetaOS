//
//  String.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

public extension String {
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
    
    init?<T: Collection>(utf16Characters: T) where T.Element == UInt16 {
        var str = ""
        var iterator = utf16Characters.makeIterator()
        var utf16 = UTF16()
        var done = false
        while !done {
            let r = utf16.decode(&iterator)
            switch r {
            case .emptyInput:
                done = true
            case let .scalarValue(val):
                if val != "\0" {
                    str.append(Character(val))
                } else {
                    done = true
                }
            case .error:
                return nil
            }
        }
        self = str
    }
    
    init?<T: Collection>(utf8Characters: T) where T.Element == UInt8 {
        var str = ""
        var iterator = utf8Characters.makeIterator()
        var utf8 = UTF8()
        var done = false
        while !done {
            let r = utf8.decode(&iterator)
            switch r {
            case .emptyInput:
                done = true
            case let .scalarValue(val):
                if val != "\0" {
                    str.append(Character(val))
                } else {
                    done = true
                }
            case .error:
                return nil
            }
        }
        self = str
    }
    
    func leftPadding(toLength: Int, withPad: String = " ") -> String {
        
        guard toLength > self.count else { return self }
        
        let padding = String(repeating: withPad, count: toLength - self.count)
        
        return padding + self
    }
    
    func trim() -> String {
        let array = Array(self)
        var i = array.count - 1
        while i > 0 {
            if array[i] != " " && array[i] != "\0" {
                break
            }
            i -= 1
        }
        var retStr = ""
        var j = 0
        for elem in array {
            if j == i {
                retStr.append(elem)
                return retStr
            }
            retStr.append(elem)
            j += 1
        }
        return retStr
    }
}
