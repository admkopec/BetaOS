//
//  Window.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/23/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Graphics

final class Window {
    var titleString: String
    var visible: Bool
    var view: View
    var windowView: View
    fileprivate static let defaultBordersColor = #colorLiteral(red: 0.6000000238, green: 0.6000000238, blue: 0.6000000238, alpha: 0.95) as Color
    
    fileprivate init(position: Position, size: Size, titlebarHeight: Int, bordersWidth: Int, bordersColor: Color, title: String) {
        titleString = title
        windowView = View(position: position, size: size)
        view = View(position: Position.init(x: bordersWidth, y: titlebarHeight), size: Size.init(width: size.Width - bordersWidth * 2, height: size.Height - (titlebarHeight + bordersWidth)))
        visible = true
        windowView.draw(roundedRectangle: RoundedRectangle(position: Position(x: 0, y: 0), size: size, color: bordersColor, radius: 15, filled: true))
    }
    
    convenience init(position: Position, size: Size, title: String = "", thinBorders: Bool = false, bordersColor: Color = defaultBordersColor) {
        var borderWidth = 3
        if thinBorders {
            borderWidth = 0
        }
        self.init(position: position, size: size, titlebarHeight: 10, bordersWidth: borderWidth, bordersColor: bordersColor, title: title)
    }
    
    convenience init(size: Size, title: String = "") {
        let position = Position.Center - size / 2
        self.init(position: position, size: size, title: title)
    }
    
    static var Popup: Window {
        let size = Size(width: 500, height: 275)
        let position = Position.Center - (size / 2)
        return self.init(position: position, size: size, titlebarHeight: 3, bordersWidth: 3, bordersColor: defaultBordersColor, title: "")
    }
}
