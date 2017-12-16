//
//  Graphics.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/10/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

final class Color {
    let value: UInt32
    let red: UInt8
    let green: UInt8
    let blue: UInt8
    let alpha: Float
    
    init(red: Float, green: Float, blue: Float, alpha: Float) {
        self.value = UInt32((Int(alpha * 255) * 0x1000000) + (Int(red * 255) * 0x10000) + (Int(green * 255) * 0x100) + (Int(blue * 255) * 0x1))
        self.alpha = alpha
        self.red   = UInt8((red   * 255))
        self.green = UInt8((green * 255))
        self.blue  = UInt8((blue  * 255))
    }
    
    init(red: Int, green: Int, blue: Int, alpha: Int) {
        self.value = UInt32((alpha * 0x1000000) + (red * 0x10000) + (green * 0x100) + (blue * 0x1))
        self.alpha = Float(alpha / 255)
        self.red   = UInt8(red)
        self.green = UInt8(green)
        self.blue  = UInt8(blue)
    }
    
    @nonobjc public convenience init(colorLiteralRed red: Float, green: Float, blue: Float, alpha: Float) {
        self.init(red: red, green: green, blue: blue, alpha: alpha)
    }
    
    public convenience init(displayP3Red: Float, green: Float, blue: Float, alpha: Float) {
        self.init(red: displayP3Red, green: green, blue: blue, alpha: alpha)
    }
}

fileprivate protocol ShapeBase {
    var position: Position { get }
    var color:    Color    { get }
    var filled:   Bool     { get }
}

fileprivate protocol RectangleBase: ShapeBase {
    var size:     Size     { get }
}

/**
 Structure used to represent coordinates
 */
struct Position {
    /**
     The X Coordinate
     */
    fileprivate(set) var X: Int
    /**
     The Y Coordinate
     */
    fileprivate(set) var Y: Int
    
    init(x: Int, y: Int) {
        X = x
        Y = y
    }
}

/**
 Structure used to represent a size of an object
 */
struct Size {
    /**
     The Width of an object
     */
    fileprivate(set) var Width:  Int
    /**
     The Height of an object
     */
    fileprivate(set) var Height: Int
    
    init(width: Int, height: Int) {
        Width  = width
        Height = height
    }
}

/**
 The simplest graphical structure used to represent a pixel
 */
struct Point {
    /**
     Position of Point to draw
     */
    fileprivate(set) var position: Position
    /**
     Color of Point
     */
    fileprivate(set) var color:    Color
    
    init(position: Position, color: Color) {
        self.position = position
        self.color    = color
    }
}

/**
 Graphical structure used to represent a line
 */
struct Line {
    /**
     Position of the first Point to draw
     */
    fileprivate(set) var from:  Position
    /**
     Position of the last Point to draw
     */
    fileprivate(set) var to:    Position
    /**
     Color of the Line
     */
    fileprivate(set) var color: Color
    
    init(from: Position, to: Position, color: Color) {
        self.from   = from
        self.to     = to
        self.color  = color
    }
}

/**
 Graphical structure for representing circles
 */
final class Circle: ShapeBase {
    /**
     Position of Midpoint
     */
    fileprivate(set) var position: Position
    /**
     Length of radius
     */
    fileprivate(set) var radius:   Int
    /**
     Color
     */
    fileprivate(set) var color:    Color
    /**
     Boolean indicating if circle should be filled
     */
    fileprivate(set) var filled:   Bool
    
    /**
     - parameter position: Position of Midpoint
     - parameter color: Color
     - parameter filled: Boolean indicating if circle should be filled
     - parameter radius: Length of radius
     */
    init(position: Position, color: Color, filled: Bool, radius: Int) {
        self.position = position
        self.radius   = radius
        self.color    = color
        self.filled   = filled
    }
}

/**
 Graphical structure for representing rectangles
 */
final class Rectangle {
    /**
     Position of Upper Left Corner
     */
    fileprivate(set) var positionTop: Position
    /**
     Position of Lower Right Corner
     */
    fileprivate(set) var positionBottom: Position
    /**
     Size of Rectangle
     */
    fileprivate(set) var size:     Size
    /**
     Color of Rectangle
     */
    fileprivate(set) var color:    Color
    /**
     Boolean indicating if Rectangle should be filled
     */
    fileprivate(set) var filled:   Bool
    
    init(position: Position, size: Size, color: Color, filled: Bool) {
        self.positionTop    = position
        self.positionBottom = Position(x: position.X + size.Width, y: position.Y + size.Height)
        self.size           = size
        self.color          = color
        self.filled         = filled
    }
    
    init(positionOfUpperLeftCorner: Position, positionOfLowerRightCorner: Position, size: Size, color: Color, filled: Bool) {
        self.positionTop    = positionOfUpperLeftCorner
        self.positionBottom = positionOfLowerRightCorner
        self.size           = size
        self.color          = color
        self.filled         = filled
    }
}

/**
 Graphical structure for representing rectangles with rounded corners
 */
final class RoundedRectangle: RectangleBase {
    /**
     Position of Upper Left Corner
     */
    fileprivate(set) var position: Position
    /**
     Size of Rectangle
     */
    fileprivate(set) var size:     Size
    /**
     Color of Rectangle
     */
    fileprivate(set) var color:    Color
    /**
     Radius of Rectangle's Corners
     */
    fileprivate(set) var radius:   Int
    /**
     Boolean indicating if Rectangle should be filled
     */
    fileprivate(set) var filled:   Bool
    
    init(position: Position, size: Size, color: Color, radius: Int, filled: Bool) {
        self.position = position
        self.size     = size
        self.color    = color
        self.radius   = radius
        self.filled   = filled
    }
}
