//
//  View.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/10/18.
//  Copyright © 2018 Adam Kopeć. All rights
//

import Graphics

protocol ViewProto: AnyObject {
    var FrameBuffer: UnsafeMutableBufferPointer<UInt32> { get set }
    var Display: DisplayInfo { get }
    var position: Position { get }
    var ShouldRedraw: Bool { get set }
}

class MainView: View {
    fileprivate var childViews = [ViewProto]()
    
    init(size: Size, depth: Int) {
        super.init(position: Position(x: 0, y: 0), size: size, depth: depth)
        guard depth != 0 else {
            return
        }
        if (Int(Platform_state.video.v_width), Int(Platform_state.video.v_height), Int(Platform_state.video.v_depth)) == (Display.Resolution.Width, Display.Resolution.Height, Display.Depth) {
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
            Platform_state.video.v_rowBytes = UInt(Display.Resolution.Width) * (Platform_state.video.v_rowBytes / Platform_state.video.v_width)
            Platform_state.video.v_depth    = UInt(Display.Depth)
            Platform_state.video.v_width    = UInt(Display.Resolution.Width)
            Platform_state.video.v_height   = UInt(Display.Resolution.Height)
        }
        Screen = UInt(bitPattern: FrameBuffer.baseAddress!)
    }
    
    func redraw() {
        for childView in childViews {
            if childView.ShouldRedraw {
                self += (childView as! View)
            }
        }
    }
    
    func add(view: View) {
        childViews.append(view)
    }
}

class View: Video, ViewProto {
    var FrameBuffer: UnsafeMutableBufferPointer<UInt32>
    fileprivate(set) public var Display: DisplayInfo
    fileprivate(set) public var position: Position
    var ShouldRedraw: Bool
    
    fileprivate init(position: Position, size: Size, depth: Int) {
        Display = DisplayInfo(Resolution: size, Depth: depth)
        self.position       = position
        ShouldRedraw        = true
        let startAddr = UnsafeMutablePointer<UInt32>.allocate(capacity: (Display.Resolution.Height + 1) * Display.Resolution.Width)
        FrameBuffer = UnsafeMutableBufferPointer<UInt32>(start: startAddr, count: (Display.Resolution.Height + 1) * Display.Resolution.Width)
    }
    
    public convenience init(position: Position = Position(x: 0, y: 0), size: Size) {
        self.init(position: position, size: size, depth: System.sharedInstance.Video.mainView.Display.Depth)
        System.sharedInstance.Video.mainView.add(view: self)
    }
    
    deinit {
        FrameBuffer.baseAddress!.deinitialize(count: (Display.Resolution.Height + 1) * Display.Resolution.Width)
    }
    
    public func draw(point: Point) -> Void {
//        guard position < Display.Resolution && position >= Position(x: 0, y: 0) else { return }
        cli()
        let val = FrameBuffer[getOffsetFrom(position: point.position)]
        let prevRed = UInt8(truncatingIfNeeded: val >> 16)
        let prevGreen = UInt8(truncatingIfNeeded: val >> 8)
        let prevBlue = UInt8(truncatingIfNeeded: val)
        let valRed = UInt8(point.color.Alpha * (point.color.Red * 255) + (1 - point.color.Alpha) * Float(prevRed))
        let valGreen = UInt8(point.color.Alpha * (point.color.Green * 255) + (1 - point.color.Alpha) * Float(prevGreen))
        let valBlue = UInt8(point.color.Alpha * (point.color.Blue * 255) + (1 - point.color.Alpha) * Float(prevBlue))
        let value = UInt32((0xFF * 0x1000000) + (UInt32(valRed) * 0x10000) + (UInt32(valGreen) * 0x100) + (UInt32(valBlue) * 0x1))
        FrameBuffer[getOffsetFrom(position: point.position)] = value
        sti()
        ShouldRedraw = true
    }
    
    public func draw(character: UnicodeScalar, position: Position) -> Void {
        guard position < Display.Resolution && position >= Position(x: 0, y: 0) else { return }
        guard character.value < 256 else {
            kprint("Character is bigger than maximum element in font!")
            return
        }
        let ScreenBackup = Screen
        Screen = UInt(bitPattern: FrameBuffer.baseAddress!)
        paint_char(UInt32(position.X), UInt32(position.Y), UInt8(character.value))
        Screen = ScreenBackup
        ShouldRedraw = true
    }
}

extension View {
    static func +=(lhs: View, rhs: View) {
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

fileprivate extension ViewProto {
    func getOffsetFrom(position: Position) -> Int {
        let ret = (position.Y * Display.Resolution.Width) + position.X
        if ret > Display.Resolution.Width * (Display.Resolution.Height + 1) {
            return 0
        }
        return ret
    }
}

extension ViewProto {
    static func +=(lhs: inout ViewProto, rhs: ViewProto) {
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
