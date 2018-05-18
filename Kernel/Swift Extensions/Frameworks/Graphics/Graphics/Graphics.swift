//
//  Graphics.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/10/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

//FIXME: Fix Swift classes implemented in Frameworks
public final class Color: _ExpressibleByColorLiteral {
    fileprivate(set) public var Red: Float
    fileprivate(set) public var Green: Float
    fileprivate(set) public var Blue: Float
    fileprivate(set) public var Alpha: Float
    
    public var value: UInt32 {
        return UInt32((Int(Alpha * 255) * 0x1000000) + (Int(Red * 255) * 0x10000) + (Int(Green * 255) * 0x100) + (Int(Blue * 255) * 0x1))
    }
    
    public init(red: Float, green: Float, blue: Float, alpha: Float) {
        Red   = red
        Green = green
        Blue  = blue
        Alpha = alpha
    }
    
    public init(red: Int, green: Int, blue: Int, alpha: Int) {
        Red   = Float(red)   / 255
        Green = Float(green) / 255
        Blue  = Float(blue)  / 255
        Alpha = Float(alpha) / 255
    }
    
    public convenience init(_colorLiteralRed red: Float, green: Float, blue: Float, alpha: Float) {
        self.init(red: red, green: green, blue: blue, alpha: alpha)
    }
    
    public convenience init(displayP3Red: Float, green: Float, blue: Float, alpha: Float) {
        self.init(red: displayP3Red, green: green, blue: blue, alpha: alpha)
    }
    
    public convenience init(rawPixelData: UInt32) {
        self.init(red: Int(UInt8(truncatingIfNeeded: rawPixelData >> 16)), green: Int(UInt8(truncatingIfNeeded: rawPixelData >> 8)), blue: Int(UInt8(truncatingIfNeeded: rawPixelData)), alpha: 255)
    }
    
    public static func *(lhs: Color, rhs: Color) -> Color {
        let red   = (1 - lhs.Alpha) * rhs.Red   + lhs.Alpha + lhs.Red
        let green = (1 - lhs.Alpha) * rhs.Green + lhs.Alpha + lhs.Green
        let blue  = (1 - lhs.Alpha) * rhs.Blue  + lhs.Alpha + lhs.Blue
        return Color(red: red, green: green, blue: blue, alpha: lhs.Alpha)
    }
    
    public static func *=(lhs: inout Color, rhs: Color) {
        lhs.Red   = (1 - lhs.Alpha) * rhs.Red   + lhs.Alpha + lhs.Red
        lhs.Green = (1 - lhs.Alpha) * rhs.Green + lhs.Alpha + lhs.Green
        lhs.Blue  = (1 - lhs.Alpha) * rhs.Blue  + lhs.Alpha + lhs.Blue
    }
}

private protocol ShapeBase {
    var position: Position { get }
    var color:    Color    { get }
    var filled:   Bool     { get }
}

private protocol RectangleBase: ShapeBase {
    var size:     Size     { get }
}

/**
 Structure used to represent coordinates
 */
public struct Position: Comparable {
    /**
     The X Coordinate
     */
    fileprivate(set) public var X: Int
    /**
     The Y Coordinate
     */
    fileprivate(set) public var Y: Int
    
    public init(x: Int, y: Int) {
        X = x
        Y = y
    }
    
    public static func <(lhs: Position, rhs: Position) -> Bool {
        return lhs.X < rhs.X && lhs.Y < rhs.Y
    }
    
    public static func <=(lhs: Position, rhs: Position) -> Bool {
        return lhs.X <= rhs.X && lhs.Y <= rhs.Y
    }
    
    public static func >(lhs: Position, rhs: Position) -> Bool {
        return lhs.X > rhs.X && lhs.Y > rhs.Y
    }
    
    public static func >=(lhs: Position, rhs: Position) -> Bool {
        return lhs.X >= rhs.X && lhs.Y >= rhs.Y
    }
    
    public static func ==(lhs: Position, rhs: Position) -> Bool {
        return lhs.X == rhs.X && lhs.Y == rhs.Y
    }
    
    public static func <(lhs: Position, rhs: Size) -> Bool {
        return lhs.X < rhs.Width && lhs.Y < rhs.Height
    }
    
    public static func <=(lhs: Position, rhs: Size) -> Bool {
        return lhs.X <= rhs.Width && lhs.Y <= rhs.Height
    }
    
    public static func >(lhs: Position, rhs: Size) -> Bool {
        return lhs.X > rhs.Width && lhs.Y > rhs.Height
    }
    
    public static func >=(lhs: Position, rhs: Size) -> Bool {
        return lhs.X >= rhs.Width && lhs.Y >= rhs.Height
    }
    
    public static func ==(lhs: Position, rhs: Size) -> Bool {
        return lhs.X == rhs.Width && lhs.Y == rhs.Height
    }
    
    public static func -(lhs: Position, rhs: Size) -> Position {
        return Position(x: lhs.X - rhs.Width, y: lhs.Y - rhs.Height)
    }
    
    public static func -=(lhs: inout Position, rhs: Size) {
        lhs = lhs - rhs
    }
    
    public static func +(lhs: Position, rhs: Position) -> Position {
        return Position(x: lhs.X + rhs.X, y: lhs.Y + rhs.Y)
    }
    
    public static func +=(lhs: inout Position, rhs: Position) {
        lhs = lhs + rhs
    }
    
    public static func *(lhs: Position, rhs: Int) -> Position {
        return Position(x: lhs.X * rhs, y: lhs.Y * rhs)
    }
    
    public static func *=(lhs: inout Position, rhs: Int) {
        lhs = lhs * rhs
    }
}

/**
 Structure used to represent a size of an object
 */
public struct Size {
    /**
     The Width of an object
     */
    fileprivate(set) public var Width:  Int
    /**
     The Height of an object
     */
    fileprivate(set) public var Height: Int
    
    public init(width: Int, height: Int) {
        Width  = width
        Height = height
    }
    
    public static func <(lhs: Size, rhs: Size) -> Bool {
        return lhs.Width < rhs.Width && lhs.Height < rhs.Height
    }
    
    public static func <=(lhs: Size, rhs: Size) -> Bool {
        return lhs.Width <= rhs.Width && lhs.Height <= rhs.Height
    }
    
    public static func >(lhs: Size, rhs: Size) -> Bool {
        return lhs.Width > rhs.Width && lhs.Height > rhs.Height
    }
    
    public static func >=(lhs: Size, rhs: Size) -> Bool {
        return lhs.Width >= rhs.Width && lhs.Height >= rhs.Height
    }
    
    public static func ==(lhs: Size, rhs: Size) -> Bool {
        return lhs.Width == rhs.Width && lhs.Height == rhs.Height
    }
    
    public static func /(lhs: Size, rhs: Int) -> Size {
        return Size(width: lhs.Width / rhs, height: lhs.Height / rhs)
    }
    
    public static func +(lhs: Size, rhs: Int) -> Size {
        return Size(width: lhs.Width + rhs, height: lhs.Height + rhs)
    }
    
    public static func -(lhs: Size, rhs: Int) -> Size {
        return Size(width: lhs.Width - rhs, height: lhs.Height - rhs)
    }
}

/**
 The simplest graphical structure used to represent a pixel
 */
public struct Point {
    /**
     Position of Point to draw
     */
    fileprivate(set) public var position: Position
    /**
     Color of Point
     */
    fileprivate(set) public var color:    Color
    
    public init(position: Position, color: Color) {
        self.position = position
        self.color    = color
    }
}

/**
 Graphical structure used to represent a line
 */
public struct Line {
    /**
     Position of the first Point to draw
     */
    fileprivate(set) public var from:  Position
    /**
     Position of the last Point to draw
     */
    fileprivate(set) public var to:    Position
    /**
     Color of the Line
     */
    fileprivate(set) public var color: Color
    
    public init(from: Position, to: Position, color: Color) {
        self.from   = from
        self.to     = to
        self.color  = color
    }
}

public final class QuadraticBézier {
    /**
     Position of P0
     */
    fileprivate(set) public var position0: Position
    /**
     Position of P1
     */
    fileprivate(set) public var position1: Position
    /**
     Position of P2
     */
    fileprivate(set) public var position2: Position
    /**
     Color
     */
    fileprivate(set) public var color: Color
    
    public init(p0: Position, p1: Position, p2: Position, color: Color) {
        position0 = p0
        position1 = p1
        position2 = p2
        self.color = color
    }
}

/**
 Graphical structure for representing circles
 */
public final class Circle: ShapeBase {
    /**
     Position of Midpoint
     */
    fileprivate(set) public var position: Position
    /**
     Length of radius
     */
    fileprivate(set) public var radius:   Int
    /**
     Color
     */
    fileprivate(set) public var color:    Color
    /**
     Boolean indicating if circle should be filled
     */
    fileprivate(set) public var filled:   Bool
    
    /**
     - parameter position: Position of Midpoint
     - parameter color: Color
     - parameter filled: Boolean indicating if circle should be filled
     - parameter radius: Length of radius
     */
    public init(position: Position, color: Color, filled: Bool, radius: Int) {
        self.position = position
        self.radius   = radius
        self.color    = color
        self.filled   = filled
    }
}

/**
 Graphical structure for representing rectangles
 */
public final class Rectangle {
    /**
     Position of Upper Left Corner
     */
    fileprivate(set) public var positionTop: Position
    /**
     Position of Lower Right Corner
     */
    fileprivate(set) public var positionBottom: Position
    /**
     Size of Rectangle
     */
    fileprivate(set) public var size:     Size
    /**
     Color of Rectangle
     */
    fileprivate(set) public var color:    Color
    /**
     Boolean indicating if Rectangle should be filled
     */
    fileprivate(set) public var filled:   Bool
    
    public init(position: Position, size: Size, color: Color, filled: Bool) {
        self.positionTop    = position
        self.positionBottom = Position(x: position.X + size.Width, y: position.Y + size.Height)
        self.size           = size
        self.color          = color
        self.filled         = filled
    }
    
    public init(positionOfUpperLeftCorner: Position, positionOfLowerRightCorner: Position, size: Size, color: Color, filled: Bool) {
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
public final class RoundedRectangle: RectangleBase {
    /**
     Position of Upper Left Corner
     */
    fileprivate(set) public var position: Position
    /**
     Size of Rectangle
     */
    fileprivate(set) public var size:     Size
    /**
     Color of Rectangle
     */
    fileprivate(set) public var color:    Color
    /**
     Radius of Rectangle's Corners
     */
    fileprivate(set) public var radius:   Int
    /**
     Boolean indicating if Rectangle should be filled
     */
    fileprivate(set) public var filled:   Bool
    
    public init(position: Position, size: Size, color: Color, radius: Int, filled: Bool) {
        self.position = position
        self.size     = size
        self.color    = color
        self.radius   = radius
        self.filled   = filled
    }
}
