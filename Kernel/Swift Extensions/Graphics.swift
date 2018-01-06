//
//  Graphics.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/10/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

final class Color {
    fileprivate(set) var Red: Float
    fileprivate(set) var Green: Float
    fileprivate(set) var Blue: Float
    fileprivate(set) var Alpha: Float
    
    var value: UInt32 {
        return UInt32((Int(Alpha * 255) * 0x1000000) + (Int(Red * 255) * 0x10000) + (Int(Green * 255) * 0x100) + (Int(Blue * 255) * 0x1))
    }
    
    init(red: Float, green: Float, blue: Float, alpha: Float) {
        Red   = red
        Green = green
        Blue  = blue
        Alpha = alpha
    }
    
    init(red: Int, green: Int, blue: Int, alpha: Int) {
        Red   = Float(red)   / 255
        Green = Float(green) / 255
        Blue  = Float(blue)  / 255
        Alpha = Float(alpha) / 255
    }
    
    @nonobjc public convenience init(colorLiteralRed red: Float, green: Float, blue: Float, alpha: Float) {
        self.init(red: red, green: green, blue: blue, alpha: alpha)
    }
    
    public convenience init(displayP3Red: Float, green: Float, blue: Float, alpha: Float) {
        self.init(red: displayP3Red, green: green, blue: blue, alpha: alpha)
    }
    
    internal convenience init(rawPixelData: UInt32) {
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

final class Window {
    var titleString: String
    var visible: Bool
    var view: View
    var windowView: View
    fileprivate static let defaultBordersColor = Color(red: 0.6000000238, green: 0.6000000238, blue: 0.6000000238, alpha: 0.95)
    
    fileprivate init(position: Position, size: Size, titlebarHeight: Int, bordersWidth: Int, bordersColor: Color, title: String) {
        titleString = title
        windowView = View(position: position, size: size)
        windowView.draw(roundedRectangle: RoundedRectangle(position: Position(x: 0, y: 0), size: size, color: bordersColor, radius: 15, filled: true))
        view = View(position: Position.init(x: bordersWidth, y: titlebarHeight), size: Size.init(width: size.Width - bordersWidth * 2, height: size.Height - (titlebarHeight + bordersWidth)))
        visible = true
    }
    
    init(position: Position, size: Size, title: String = "", thinBorders: Bool = false, bordersColor: Color = defaultBordersColor) {
        var borderWidth = 3
        if thinBorders {
            borderWidth = 0
        }
//        self.init(position: position, size: size, titlebarHeight: 10, bordersWidth: borderWidth, bordersColor: bordersColor, title: title)
        titleString = title
        windowView = View(position: position, size: size)
        windowView.draw(roundedRectangle: RoundedRectangle(position: Position(x: 0, y: 0), size: size, color: bordersColor, radius: 15, filled: true))
        view = View(position: Position.init(x: borderWidth, y: 15), size: Size.init(width: size.Width - borderWidth * 2, height: size.Height - (15 + borderWidth)))
        visible = true
    }
    
    init(size: Size, title: String = "") {
        let position = Position.Center - size / 2
        
        titleString = title
        windowView = View(position: position, size: size)
        windowView.draw(roundedRectangle: RoundedRectangle(position: Position(x: 0, y: 0), size: size, color: Window.defaultBordersColor, radius: 15, filled: true))
        view = View(position: Position.init(x: 3, y: 15), size: Size.init(width: size.Width - 6, height: size.Height - 18))
        visible = true
    }
    
    static var Popup: Window {
        let size = Size(width: 500, height: 275)
        let position = Position.Center - (size / 2)
        return self.init(position: position, size: size, titlebarHeight: 3, bordersWidth: 3, bordersColor: defaultBordersColor, title: "")
    }
}

final class View: Video {
    var FrameBuffer: UnsafeMutableBufferPointer<UInt32>
    fileprivate var isMainView: Bool = false
    fileprivate(set) var Display: (Resolution: Size, Depth: Int)
    fileprivate(set) var position: Position
    var ShouldRedraw: Bool
    
    init(mainView: (size: Size, depth: Int)) {
        Display.Resolution  = mainView.size
        Display.Depth       = mainView.depth
        position            = Position.init(x: 0, y: 0)
        ShouldRedraw        = true
        isMainView          = true
        
        let startaddr: UnsafeMutableRawPointer! = malloc(Display.Resolution.Height * Display.Resolution.Width * (Display.Depth / 8))
        let startAddr = UnsafeMutablePointer<UInt32>(bitPattern: UInt(bitPattern: startaddr))
        FrameBuffer = UnsafeMutableBufferPointer<UInt32>(start: startAddr, count: Display.Resolution.Height * Display.Resolution.Width * (Display.Depth / 8))
        guard mainView.depth != 0 else {
            return
        }
        if Platform_state.video.v_length == Display.Resolution.Height * Display.Resolution.Width * (Display.Depth / 8) {
            canUseSSEmemcpy = true
            memcpy(FrameBuffer.baseAddress!, UnsafeMutableRawPointer(bitPattern: Screen), Display.Resolution.Height * Display.Resolution.Width * (Display.Depth / 8))
            canUseSSEmemcpy = false
        } else {
            let ScreenPtr = UnsafeMutablePointer<UInt32>(bitPattern: Screen)
            for y in 0 ... Int(Platform_state.video.v_height - 1) {
                for x in 0 ... Int(Platform_state.video.v_width - 1) {
                    let position = Position(x: x, y: y)
                    let newVal = ScreenPtr!.advanced(by: (position.Y * Int(Platform_state.video.v_width)) + position.X).pointee
                        FrameBuffer.baseAddress!.advanced(by: getOffsetFrom(position: position)).pointee = newVal
                }
            }
            Platform_state.video.v_depth    = UInt(Display.Depth)
            Platform_state.video.v_width    = UInt(Display.Resolution.Width)
            Platform_state.video.v_height   = UInt(Display.Resolution.Height)
            Platform_state.video.v_rowBytes = UInt(Display.Resolution.Width * (Display.Depth / 8))
        }
        Screen = UInt(bitPattern: FrameBuffer.baseAddress!)
    }
    
    init(position: Position, size: Size) {
        Display.Resolution  = size
        Display.Depth       = System.sharedInstance.Video.mainView.Display.Depth
        self.position       = position
        ShouldRedraw        = true
        
        let startAddr = UnsafeMutablePointer<UInt32>.allocate(capacity: (Display.Resolution.Height + 1) * Display.Resolution.Width)
        FrameBuffer = UnsafeMutableBufferPointer<UInt32>(start: startAddr, count: (Display.Resolution.Height + 1) * Display.Resolution.Width)
        
//        System.sharedInstance.Video.childViews.append(self)
    }
    
    deinit {
        free(FrameBuffer.baseAddress!)
    }
    
    func draw(point: Point) -> Void {
        if ((point.position.X >= 0) && (point.position.X < Display.Resolution.Width)) && ((point.position.Y >= 0) && (point.position.Y < Display.Resolution.Height)) {
            let val = FrameBuffer[getOffsetFrom(position: point.position)]
            let prevRed = UInt8(truncatingIfNeeded: val >> 16)
            let prevGreen = UInt8(truncatingIfNeeded: val >> 8)
            let prevBlue = UInt8(truncatingIfNeeded: val)
            let valRed = UInt8(point.color.Alpha * (point.color.Red * 255) + (1 - point.color.Alpha) * Float(prevRed))
            let valGreen = UInt8(point.color.Alpha * (point.color.Green * 255) + (1 - point.color.Alpha) * Float(prevGreen))
            let valBlue = UInt8(point.color.Alpha * (point.color.Blue * 255) + (1 - point.color.Alpha) * Float(prevBlue))
            let value = UInt32((0xFF * 0x1000000) + (UInt32(valRed) * 0x10000) + (UInt32(valGreen) * 0x100) + (UInt32(valBlue) * 0x1))
            FrameBuffer[getOffsetFrom(position: point.position)] = value
            ShouldRedraw = true
        }
    }
    
    func draw(character: UnicodeScalar, position: Position) -> Void {
        guard isMainView else {
            kprint("View is not Main View!")
            return
        }
        if character.value > 255 {
            kprint("Character is bigger than maximum element in font!")
            return
        }
        paint_char(UInt32(position.X), UInt32(position.Y), UInt8(character.value))
        ShouldRedraw = true
    }
    
    fileprivate func getOffsetFrom(position: Position) -> Int {
        let ret = (position.Y * Display.Resolution.Width) + position.X
        if ret > Display.Resolution.Width * (Display.Resolution.Height + 1) {
            return 0
        }
        return ret
    }
    
    static func +=(lhs: inout View, rhs: View) {
        guard rhs.ShouldRedraw else {
            return
        }
        for y in 0 ... rhs.Display.Resolution.Height {
            for x in 0 ... rhs.Display.Resolution.Width {
                let position = Position(x: x, y: y)
                let prevVal = lhs.FrameBuffer[lhs.getOffsetFrom(position: rhs.position + position)]
                var newVal  = rhs.FrameBuffer[rhs.getOffsetFrom(position: position)]
                let newAlpha = UInt8(truncatingIfNeeded: newVal >> 24)
                if newAlpha == 0x00 {
                    continue
                } else if newAlpha == 0xFF {
                    lhs.FrameBuffer[lhs.getOffsetFrom(position: rhs.position + position)] = newVal
                } else {
                    var newRed    = UInt8(truncatingIfNeeded: newVal >> 16)
                    var newGreen  = UInt8(truncatingIfNeeded: newVal >> 8)
                    var newBlue   = UInt8(truncatingIfNeeded: newVal)
                    let prevRed   = UInt8(truncatingIfNeeded: prevVal >> 16)
                    let prevGreen = UInt8(truncatingIfNeeded: prevVal >> 8)
                    let prevBlue  = UInt8(truncatingIfNeeded: prevVal)
                    newRed = UInt8((Float(newAlpha) / 255) * Float(newRed) + (1 - (Float(newAlpha) / 255)) * Float(prevRed))
                    newGreen = UInt8((Float(newAlpha) / 255) * Float(newGreen) + (1 - (Float(newAlpha) / 255)) * Float(prevGreen))
                    newBlue = UInt8((Float(newAlpha) / 255) * Float(newBlue) + (1 - (Float(newAlpha) / 255)) * Float(prevBlue))
                    newVal = UInt32((0xFF * 0x1000000) + (UInt32(newRed) * 0x10000) + (UInt32(newGreen) * 0x100) + (UInt32(newBlue) * 0x1))
                    lhs.FrameBuffer[lhs.getOffsetFrom(position: rhs.position + position)] = newVal
                }
            }
        }
        rhs.ShouldRedraw = false
        lhs.ShouldRedraw = true
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
struct Position: Comparable {
    /**
     The X Coordinate
     */
    fileprivate(set) var X: Int
    /**
     The Y Coordinate
     */
    fileprivate(set) var Y: Int
    
    static var Center: Position {   // You could try making it center of active view instead of mainView
        return self.init(x: System.sharedInstance.Video.mainView.Display.Resolution.Width/2, y: System.sharedInstance.Video.mainView.Display.Resolution.Height/2)
    }
    
    init(x: Int, y: Int) {
        X = x
        Y = y
    }
    
    static func <(lhs: Position, rhs: Position) -> Bool {
        return lhs.X < rhs.X && lhs.Y < rhs.Y
    }
    
    static func ==(lhs: Position, rhs: Position) -> Bool {
        return lhs.X == rhs.X && lhs.Y == rhs.Y
    }
    
    static func <(lhs: Position, rhs: Size) -> Bool {
        return lhs.X < rhs.Width && lhs.Y < rhs.Height
    }
    
    static func ==(lhs: Position, rhs: Size) -> Bool {
        return lhs.X == rhs.Width && lhs.Y == rhs.Height
    }
    
    static func -(lhs: Position, rhs: Size) -> Position {
        return Position(x: lhs.X - rhs.Width, y: lhs.Y - rhs.Height)
    }
    
    static func -=(lhs: inout Position, rhs: Size) {
        lhs = lhs - rhs
    }
    
    static func +(lhs: Position, rhs: Position) -> Position {
        return Position(x: lhs.X + rhs.X, y: lhs.Y + rhs.Y)
    }
    
    static func +=(lhs: inout Position, rhs: Position) {
        lhs = lhs + rhs
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
    
    static func /(lhs: Size, rhs: Int) -> Size {
        return Size(width: lhs.Width / rhs, height: lhs.Height / rhs)
    }
    
    static func +(lhs: Size, rhs: Int) -> Size {
        return Size(width: lhs.Width + rhs, height: lhs.Height + rhs)
    }
    
    static func -(lhs: Size, rhs: Int) -> Size {
        return Size(width: lhs.Width - rhs, height: lhs.Height - rhs)
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

final class QuadraticBézier {
    /**
     Position of P0
     */
    fileprivate(set) var position0: Position
    /**
     Position of P1
     */
    fileprivate(set) var position1: Position
    /**
     Position of P2
     */
    fileprivate(set) var position2: Position
    /**
     Color
     */
    fileprivate(set) var color: Color
    
    init(p0: Position, p1: Position, p2: Position, color: Color) {
        position0 = p0
        position1 = p1
        position2 = p2
        self.color = color
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
