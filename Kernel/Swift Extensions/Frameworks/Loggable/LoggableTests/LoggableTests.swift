//
//  LoggableTests.swift
//  LoggableTests
//
//  Created by Adam Kopeć on 11/5/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

import XCTest
@testable import Loggable

class LoggableTests: XCTestCase {
    class LoggableTestClass: Loggable {
        let Name: String = "LoggableTests"
    }
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testExample() {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct results.
    }
    
    func testPerformanceExample() {
        // This is an example of a performance test case.
        let loggableClass = LoggableTestClass()
        self.measure {
            loggableClass.Log("Test of Loggable", level: .Info)
            // Put the code you want to measure the time of here.
        }
    }
    
}

@_silgen_name("change_font_color")
func change_font_color(_ color: UInt32) -> Void {
    
}

@_silgen_name("get_font_color")
func get_font_color() -> UInt32 {
    return 0xFFFFFFFF
}
