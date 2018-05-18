//
//  UUIDTests.swift
//  UUIDTests
//
//  Created by Adam Kopeć on 1/23/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import XCTest
@testable import UUID
import struct UUID.UUID
class UUIDTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testStringLiteral() {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct results.
        let uuid = UUID(asString: "")
        XCTAssert(uuid?.description == "C12A7328-F81F-11D2-BA4B-00A0C93EC93B", "uuid == \(uuid?.description)")
    }
    
    func testPerformanceExample() {
        // This is an example of a performance test case.
        self.measure {
            _ = "C12A7328-F81F-11D2-BA4B-00A0C93EC93B" as UUID
        }
    }
    
}
