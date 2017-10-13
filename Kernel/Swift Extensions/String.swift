//
//  String.swift
//  Kernel
//
//  Created by Adam Kopeć on 10/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

extension String {
    func leftPadding(toLength: Int, withPad: String = " ") -> String {
        
        guard toLength > self.characters.count else { return self }
        
        let padding = String(repeating: withPad, count: toLength - self.characters.count)
        
        return padding + self
    }
}
