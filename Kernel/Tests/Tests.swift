//
//  Tests.swift
//  Tests
//
//  Created by Adam Kopeć on 11/3/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import XCTest

class Tests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
//    func testColor() {
//        let col = Color(red: 1.0, green: 0, blue: 0, alpha: 1.0)
////        let color = #colorLiteral(red: 1, green: 0, blue: 0, alpha: 1)
////        let col2 = Color(_colorLiteralRed: Float(color.redComponent), green: Float(color.greenComponent), blue: Float(color.blueComponent), alpha: Float(color.alphaComponent))
//        let col2 = #colorLiteral(red: 1, green: 0, blue: 0, alpha: 1) as Color
//        XCTAssert(col.Red == col2.Red, "Colors Red don't match!")
//        XCTAssert(col.Blue == col2.Blue, "Colors Blue don't match!")
//        XCTAssert(col.Green == col2.Green, "Colors Green don't match!")
//        XCTAssert(col.Alpha == col2.Alpha, "Colors Alpha don't match!")
//    }
    
    func testNSObject_conformsToProtocol() {
        // Use XCTAssert and related functions to verify your tests produce the correct results.
        let nsObj = NSObject()
        XCTAssert(nsObj.isKind(of: NSObject.self))
        var item = ""
        let int: Any = Int(1)
        if case let printable as CustomStringConvertible = int {
            printable.description.write(to: &item)
        }
        XCTAssert(item == "1", "item is \(item)")
    }
    
    func testPerformance() {
        // This is an example of a performance test case.
        self.measure {
            var item = ""
            let int: Any = Int(1)
            if case let printable as CustomStringConvertible = int {
                printable.description.write(to: &item)
            }
        }
    }
    
}
