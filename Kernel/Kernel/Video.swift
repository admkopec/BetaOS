//
//  Video.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import Addressing
import Loggable

protocol Video {
    var Display: (Resolution: Size, Depth: Int) { get }
    
    func draw(point: Point) -> Void
    func draw(circle: Circle) -> Void
    func draw(rectangle: Rectangle) -> Void
    func draw(line: Line, thickness: Int) -> Void
    func draw(roundedRectangle: RoundedRectangle) -> Void
    func draw(character: UnicodeScalar, position: Position) -> Void
}

extension Video {
    func draw(line: Line) -> Void {
        draw(line: line, thickness: 1)
    }
}

struct VESA: Video, Loggable {
    let Name: String = "VESA"
    fileprivate(set) var Display: (Resolution: Size, Depth: Int)
    fileprivate var BAR: Address
    
    init() {
        let BootVideo = Platform_state.video
        
        Display.Resolution = Size(width: Int(BootVideo.v_width), height: Int(BootVideo.v_height))
        Display.Depth      = Int(BootVideo.v_depth)

        BAR = Address(BootVideo.v_baseAddr)
    }
    
    func draw(point: Point) -> Void {
        if ((point.position.X >= 0) && (point.position.X <= Display.Resolution.Width)) && ((point.position.Y >= 0) && (point.position.Y <= Display.Resolution.Height)) {
            let val = get_pixel(UInt32(point.position.X), UInt32(point.position.Y))
            let prevRed = UInt8(truncatingIfNeeded: val >> 16)
            let prevGreen = UInt8(truncatingIfNeeded: val >> 8)
            let prevBlue = UInt8(truncatingIfNeeded: val)
            let valRed = UInt8(point.color.alpha * Float(point.color.red) + (1 - point.color.alpha) * Float(prevRed))
            let valGreen = UInt8(point.color.alpha * Float(point.color.green) + (1 - point.color.alpha) * Float(prevGreen))
            let valBlue = UInt8(point.color.alpha * Float(point.color.blue) + (1 - point.color.alpha) * Float(prevBlue))
            let value = UInt32((0xFF * 0x1000000) + (UInt32(valRed) * 0x10000) + (UInt32(valGreen) * 0x100) + (UInt32(valBlue) * 0x1))

            paint_pixel(UInt32(point.position.X), UInt32(point.position.Y), value)
        }
    }
    
    func draw(line: Line, thickness: Int) -> Void {
        if line.from.X == line.to.X {
            for y in line.from.Y ... line.to.Y {
                for x in line.to.X ... line.to.X + thickness {
                    draw(point: Point(position: Position.init(x: x, y: y), color: line.color))
                }
            }
        } else if line.from.Y == line.to.Y {
            for x in line.from.X ... line.to.X {
                for y in line.to.Y ... line.to.Y + thickness {
                    draw(point: Point(position: Position.init(x: x, y: y), color: line.color))
                }
            }
        } else {
            draw(diagonalLine: line, thickness: thickness)
        }
    }
    
    func draw(rectangle: Rectangle) -> Void {
        draw(line: Line(from: rectangle.positionTop, to: Position.init(x: rectangle.positionBottom.X, y: rectangle.positionTop.Y), color: rectangle.color))
        for y in 1 ... rectangle.size.Height - 1 {
            if rectangle.filled == true {
                draw(line: Line(from: Position.init(x: rectangle.positionTop.X, y: y + rectangle.positionTop.Y), to: Position.init(x: rectangle.positionBottom.X, y: y + rectangle.positionTop.Y), color: rectangle.color))
            } else {
                draw(point: Point(position: Position.init(x: rectangle.positionTop.X, y: y + rectangle.positionTop.Y), color: rectangle.color))
                draw(point: Point(position: Position.init(x: rectangle.positionBottom.X, y: y + rectangle.positionTop.Y), color: rectangle.color))
            }
        }
        draw(line: Line(from: Position.init(x: rectangle.positionTop.X, y: rectangle.positionBottom.Y), to: rectangle.positionBottom, color: rectangle.color))
    }
    
    func draw(circle: Circle) -> Void {
        var x   = circle.radius - 1
        var y   = 0
        var dx  = 1
        var dy  = 1
        var err = dx - (circle.radius << 1)
        while x >= y {
            if circle.filled == true {
                draw(line: Line(from: Position.init(x: circle.position.X - x, y: circle.position.Y + y), to: Position.init(x: circle.position.X + x, y: circle.position.Y + y), color: circle.color))
                draw(line: Line(from: Position.init(x: circle.position.X - y, y: circle.position.Y + x), to: Position.init(x: circle.position.X + y, y: circle.position.Y + x), color: circle.color))
                draw(line: Line(from: Position.init(x: circle.position.X - x, y: circle.position.Y - y), to: Position.init(x: circle.position.X + x, y: circle.position.Y - y), color: circle.color))
                draw(line: Line(from: Position.init(x: circle.position.X - y, y: circle.position.Y - x), to: Position.init(x: circle.position.X + y, y: circle.position.Y - x), color: circle.color))
            } else {
                draw(point: Point(position: Position.init(x: circle.position.X + x, y: circle.position.Y + y), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X - x, y: circle.position.Y + y), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X + y, y: circle.position.Y + x), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X - y, y: circle.position.Y + x), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X + x, y: circle.position.Y - y), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X - x, y: circle.position.Y - y), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X + y, y: circle.position.Y - x), color: circle.color))
                draw(point: Point(position: Position.init(x: circle.position.X - y, y: circle.position.Y - x), color: circle.color))
            }
            if err <= 0 {
                y   += 1
                err += dy
                dy  += 2
            }
            if err > 0 {
                x   -= 1
                dx  += 2
                err += dx - (circle.radius << 1)
            }
        }
    }
    
    func draw(diagonalLine: Line, thickness: Int) -> Void {
        let deltax = (diagonalLine.to.X - diagonalLine.from.X)
        let deltay = (diagonalLine.to.Y - diagonalLine.from.Y)
        let sx = (diagonalLine.from.X < diagonalLine.to.X) ? 1 : -1
        let sy = (diagonalLine.from.Y < diagonalLine.to.Y) ? 1 : -1
        var error = deltax - deltay
        var x = diagonalLine.from.X
        var y = diagonalLine.from.Y
        while x != diagonalLine.to.X && y != diagonalLine.to.Y {
            for j in -thickness ... thickness {
                for i in -thickness ... thickness {
                    draw(point: Point(position: Position.init(x: x + i, y: y + j), color: diagonalLine.color))
                }
            }
            let e2 = error * 2
            if e2 > -deltay {
                error -= deltay
                x += sx
            }
            if e2 < deltax {
                error += deltax
                y += sy
            }
        }
    }
    
    func draw(roundedRectangle: RoundedRectangle) {
        var f = 1 - roundedRectangle.radius
        var ddF_x = 1
        var ddF_y = -2 * roundedRectangle.radius
        var xx = 0
        var yy = roundedRectangle.radius
        
        while xx < yy {
            if f >= 0 {
                yy    -= 1
                ddF_y += 2
                f     += ddF_y
            }
            xx    += 1
            ddF_x += 2
            f     += ddF_x
            if roundedRectangle.filled == true {
                draw(line: Line(from: Position.init(x: roundedRectangle.position.X - xx + roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + yy - roundedRectangle.radius), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + xx - roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + yy - roundedRectangle.radius) , color: roundedRectangle.color))
                draw(line: Line(from: Position.init(x: roundedRectangle.position.X - yy + roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + xx - roundedRectangle.radius), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + yy - roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + xx - roundedRectangle.radius), color: roundedRectangle.color))
                draw(line: Line(from: Position.init(x: roundedRectangle.position.X - xx + roundedRectangle.radius, y: roundedRectangle.position.Y - yy + roundedRectangle.radius), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + xx - roundedRectangle.radius, y: roundedRectangle.position.Y - yy + roundedRectangle.radius), color: roundedRectangle.color))
                draw(line: Line(from: Position.init(x: roundedRectangle.position.X - yy + roundedRectangle.radius, y: roundedRectangle.position.Y - xx + roundedRectangle.radius), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + yy - roundedRectangle.radius, y: roundedRectangle.position.Y - xx + roundedRectangle.radius), color: roundedRectangle.color))
            } else {
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + xx - roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + yy - roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X - xx + roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + yy - roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + yy - roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + xx - roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X - yy + roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height + xx - roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + xx - roundedRectangle.radius, y: roundedRectangle.position.Y - yy + roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X - xx + roundedRectangle.radius, y: roundedRectangle.position.Y - yy + roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width + yy - roundedRectangle.radius, y: roundedRectangle.position.Y - xx + roundedRectangle.radius), color: roundedRectangle.color))
                draw(point: Point(position: Position.init(x: roundedRectangle.position.X - yy + roundedRectangle.radius, y: roundedRectangle.position.Y - xx + roundedRectangle.radius), color: roundedRectangle.color))
            }
        }
        if roundedRectangle.filled == true {
            draw(rectangle: Rectangle(position: Position.init(x: roundedRectangle.position.X, y: roundedRectangle.position.Y + roundedRectangle.radius), size: Size.init(width: roundedRectangle.size.Width, height: roundedRectangle.size.Height - 2 * roundedRectangle.radius), color: roundedRectangle.color, filled: true))
        } else {
            draw(line: Line(from: Position.init(x: roundedRectangle.position.X + roundedRectangle.radius, y: roundedRectangle.position.Y), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width - roundedRectangle.radius, y: roundedRectangle.position.Y), color: roundedRectangle.color))
            draw(line: Line(from: Position.init(x: roundedRectangle.position.X + roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width - roundedRectangle.radius, y: roundedRectangle.position.Y + roundedRectangle.size.Height), color: roundedRectangle.color))
            draw(line: Line(from: Position.init(x: roundedRectangle.position.X, y: roundedRectangle.position.Y + roundedRectangle.radius), to: Position.init(x: roundedRectangle.position.X, y: roundedRectangle.position.Y + roundedRectangle.size.Height - roundedRectangle.radius), color: roundedRectangle.color))
            draw(line: Line(from: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width, y: roundedRectangle.position.Y + roundedRectangle.radius), to: Position.init(x: roundedRectangle.position.X + roundedRectangle.size.Width, y: roundedRectangle.position.Y + roundedRectangle.size.Height - roundedRectangle.radius), color: roundedRectangle.color))
        }
    }
    
    func draw(character: UnicodeScalar, position: Position) -> Void {
        if character.value > 255 {
            Log("Character is bigger than maximum element in font!", level: .Error)
            return
        }
        paint_char(UInt32(position.X), UInt32(position.Y), UInt8(character.value))
    }
}

internal struct _Stdout: TextOutputStream {
    mutating func write(_ string: String) {
        for scalar in string.unicodeScalars {
            System.sharedInstance.Video.draw(character: scalar, position: Position(x: 0, y: 0))
        }
    }
}
