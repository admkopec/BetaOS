//
//  VESA.swift
//  Kernel
//
//  Created by Adam Kopeć on 12/16/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

import Addressing
import CommonExtensions
import Loggable
import Graphics

class VESA: VideoModule {
    let Name: String = "VESA"
    fileprivate let VRAMSize: Int
    fileprivate(set) var Display: (Resolution: Size, Depth: Int)
    var mainView: MainView
    
    fileprivate var FrameBufferBase: Address
    
    var description: String {
        return "Generic VESA compatible Video Device \(VRAMSize/MB)MB"
    }
    
    init() {
        let BootVideo = Platform_state.video
        
        Display.Resolution = Size(width: Int(BootVideo.v_width), height: Int(BootVideo.v_height))
        Display.Depth      = Int(BootVideo.v_depth)
        mainView = MainView(size: Display.Resolution, depth: Display.Depth)
        FrameBufferBase = Address(BootVideo.v_baseAddr, size: vm_size_t(BootVideo.v_length))
        VRAMSize = Int(BootVideo.v_length)
    }
    
    func refresh() {
        guard mainView.ShouldRedraw || modified else {
            return
        }
        let ptr = UnsafeMutablePointer<UInt32>(bitPattern: FrameBufferBase.virtual)
        canUseSSEmemcpy = true
        memcpy(ptr!, mainView.FrameBuffer.baseAddress!, mainView.Display.Resolution.Height * mainView.Display.Resolution.Width * (mainView.Display.Depth / 8))
        canUseSSEmemcpy = false
        mainView.ShouldRedraw = false
        modified = false
    }
}
