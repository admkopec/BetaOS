//
//  Video.swift
//  Kernel
//
//  Created by Adam Kopeć on 11/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

protocol Video {
    var Display: (Resolution: Size, Depth: Int) { get }
    
    func draw(point: Point) -> Void
    func draw(character: UnicodeScalar, position: Position) -> Void
}

extension Video {
    func draw(line: Line) -> Void {
        draw(line: line, thickness: 1)
    }
    
    func draw(line: Line, thickness: Int) -> Void {
        if line.from.X == line.to.X {
            for y in line.from.Y ... line.to.Y {
                for x in line.to.X ... line.to.X + thickness - 1 {
                    draw(point: Point(position: Position.init(x: x, y: y), color: line.color))
                }
            }
        } else if line.from.Y == line.to.Y {
            for x in line.from.X ... line.to.X {
                for y in line.to.Y ... line.to.Y + thickness - 1 {
                    draw(point: Point(position: Position.init(x: x, y: y), color: line.color))
                }
            }
        } else {
            let deltax = +(line.to.X - line.from.X)
            let deltay = +(line.to.Y - line.from.Y)
            let sx = (line.from.X < line.to.X) ? 1 : -1
            let sy = (line.from.Y < line.to.Y) ? 1 : -1
            var error = deltax - deltay
            var x = line.from.X
            var y = line.from.Y
            while x != line.to.X && y != line.to.Y {
                for j in -(thickness - 1) ... (thickness - 1) {
                    for i in -(thickness - 1) ... (thickness - 1) {
                        draw(point: Point(position: Position.init(x: x + i, y: y + j), color: line.color))
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
        if !circle.filled {
            return draw(circle3: circle)
        }
        var x   = circle.radius - 1
        var y   = 0
        var dx  = 1
        var dy  = 1
        var err = dx - (circle.radius << 1)
        while x >= y {
            draw(line: Line(from: Position.init(x: circle.position.X - x, y: circle.position.Y + y), to: Position.init(x: circle.position.X + x, y: circle.position.Y + y), color: circle.color))
            draw(line: Line(from: Position.init(x: circle.position.X - y, y: circle.position.Y + x), to: Position.init(x: circle.position.X + y, y: circle.position.Y + x), color: circle.color))
            draw(line: Line(from: Position.init(x: circle.position.X - x, y: circle.position.Y - y), to: Position.init(x: circle.position.X + x, y: circle.position.Y - y), color: circle.color))
            draw(line: Line(from: Position.init(x: circle.position.X - y, y: circle.position.Y - x), to: Position.init(x: circle.position.X + y, y: circle.position.Y - x), color: circle.color))
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
    
    func draw(quadraticBézier: QuadraticBézier) -> Void {
        let NUMBER_OF_SEGMENTS = quadraticBézier.position2.X - quadraticBézier.position0.X - 1
        var pts = [Position]()
        for i in 0 ... NUMBER_OF_SEGMENTS {
            let t = Double(i) / Double(NUMBER_OF_SEGMENTS)
            let a = (1.0 - t) * (1.0 - t)
            let b = 2.0 * t * (1.0 - t)
            let c = t * t
            let x = a * Double(quadraticBézier.position0.X) + b * Double(quadraticBézier.position1.X) + c * Double(quadraticBézier.position2.X)
            let y = a * Double(quadraticBézier.position0.Y) + b * Double(quadraticBézier.position1.Y) + c * Double(quadraticBézier.position2.Y)
            pts.append(Position(x: Int(x), y: Int(y)))
        }
        for i in 0 ... NUMBER_OF_SEGMENTS-1 {
//            draw(point: Point(position: pts[i], color: quadraticBézier.color))
            let j = i + 1
            draw(line: Line(from: pts[i], to: pts[j], color: quadraticBézier.color))
        }
    }
    
    fileprivate func point4(center: Position, delta: Position, color: Color) -> Void {
        draw(point: Point(position: Position.init(x: center.X + delta.X, y: center.Y + delta.Y), color: color))
        draw(point: Point(position: Position.init(x: center.X - delta.X, y: center.Y + delta.Y), color: color))
        draw(point: Point(position: Position.init(x: center.X + delta.X, y: center.Y - delta.Y), color: color))
        draw(point: Point(position: Position.init(x: center.X - delta.X, y: center.Y - delta.Y), color: color))
    }
    
//    fileprivate func line4(center: Position, delta: Position, color: Color) -> Void {
//        draw(line: Line(from: Position(x: center.X - delta.X, y: center.Y + delta.Y), to: Position(x: center.X + delta.X, y: center.Y + delta.Y), color: color))
//        draw(line: Line(from: Position(x: center.X - delta.X, y: center.Y - delta.Y), to: Position(x: center.X + delta.X, y: center.Y - delta.Y), color: color))
//    }
    
    fileprivate func draw(circle3: Circle) -> Void {
        let diameter = circle3.radius * circle3.radius
        let quarter = Int(Float(diameter) / sqrtf(Float(2*diameter)))
        for x in 0 ... quarter {
            let y = sqrtf(+(Float(circle3.radius * circle3.radius) - Float(x * x)))
            
            let error  = y - floorf(y)
            let alpha  = error
            let alpha2 = (1 - error)

            point4(center: circle3.position, delta: Position.init(x: x, y: Int(floorf(y))), color: Color(red: circle3.color.Red, green: circle3.color.Green, blue: circle3.color.Blue, alpha: alpha))
            point4(center: circle3.position, delta: Position.init(x: x, y: Int(floorf(y) - 1)), color: Color(red: circle3.color.Red, green: circle3.color.Green, blue: circle3.color.Blue, alpha: alpha2))
        }
//        quarter = Int(Float(diameter) / sqrtf(Float(2*diameter)))
        for y in 0 ... quarter {
            let x = sqrtf(+(Float(circle3.radius * circle3.radius) - Float(y * y)))
            
            let error  = x - floorf(x)
            let alpha  = error
            let alpha2 = (1 - error)
        
            point4(center: circle3.position, delta: Position.init(x: Int(floorf(x)), y: y), color: Color(red: circle3.color.Red, green: circle3.color.Green, blue: circle3.color.Blue, alpha: alpha))
            point4(center: circle3.position, delta: Position.init(x: Int(floorf(x) - 1), y: y), color: Color(red: circle3.color.Red, green: circle3.color.Green, blue: circle3.color.Blue, alpha: alpha2))
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
}

internal struct _Stdout: TextOutputStream {
    mutating func write(_ string: String) {
        for scalar in string.unicodeScalars {
            System.sharedInstance.Video.mainView.draw(character: scalar, position: Position(x: 0, y: 0))
        }
    }
}

@_silgen_name("refresh_screen")
public func refresh_screen() -> Void {
    System.sharedInstance.Video.refresh()
}
