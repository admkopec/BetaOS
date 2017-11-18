//
//  Video.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Loggable

typealias Size = (Width: Int, Height: Int)
typealias Position = (X: Int, Y: Int)
typealias Color = (Red: Int, Green: Int, Blue: Int)

protocol Video {
    var Display: (Resolution: Size, Depth: Int) { get }
    func drawRectangle(Size: Size, Position: Position, Color: Color) -> Void
    func drawCharacter(character: UnicodeScalar, Position: (X: Int, Y: Int)) -> Void
}

struct VESA: Video, Loggable {
    let Name: String = "VESA"
    fileprivate(set) var Display: (Resolution: Size, Depth: Int)
    fileprivate var BAR: Address
    
    init() {
        let BootVideo = Platform_state.video
        
        Display.Resolution = (Int(BootVideo.v_width), Int(BootVideo.v_height))
        Display.Depth      =  Int(BootVideo.v_depth)

        BAR = Address(BootVideo.v_baseAddr)
    }
    
    func drawRectangle(Size: Size, Position: Position, Color: Color) -> Void {
        paint_rectangle(UInt32(Position.X), UInt32(Position.Y), UInt8(Color.Red), UInt8(Color.Green), UInt8(Color.Blue), UInt8(Size.Width), UInt8(Size.Height))
    }
    
    func drawCharacter(character: UnicodeScalar, Position: (X: Int, Y: Int)) -> Void {
        Log("Unicode Code is \(character.value)", level: .Info)
    }
}

internal struct _Stdout: TextOutputStream {
    mutating func write(_ string: String) {
        for scalar in string.unicodeScalars {
            System.sharedInstance.Video.drawCharacter(character: scalar, Position: (0, 0))
        }
    }
}
