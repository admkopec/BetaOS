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
    
    func testNSObject_conformsToProtocol() {
        // Use XCTAssert and related functions to verify your tests produce the correct results.
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
