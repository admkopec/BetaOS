//
//  GlobalObjects.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/26/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import Loggable

extension ACPI {
    class GlobalObjects: Loggable {
        let Name: String = "ACPI.GlobalObjects"
        class ObjectNode: Hashable, Loggable {
            let Name: String = "ACPI.GlobalObjects.ObjectNode"
            private(set) var childNodes: [ObjectNode]   // Need to check if it works (Array of classes)
                                                        // Should be a dictionary of [name: ObjectNode]
            fileprivate(set) var object: AMLObject
            var name: String   { return object.name.value }
            var hashValue: Int { return object.name.value.hashValue }
            
            init(name: String, object: AMLObject, childNodes: [ObjectNode]) {
                guard name == object.name.shortName.value else {
                    self.object = AMLDefMutex(name: AMLNameString("NULL"), flags: AMLMutexFlags())
                    self.childNodes = []
                    Log(" Node Name (\(name)) != Object Name (\(object.name.shortName.value))", level: .Error)
                    fatalError()
                }
                self.object = object
                self.childNodes = childNodes
            }
            
            static func == (lhs: ACPI.GlobalObjects.ObjectNode, rhs: ACPI.GlobalObjects.ObjectNode) -> Bool {
                return lhs.name == rhs.name
            }
            
            subscript(index: String) -> ObjectNode? {
                get {
                    for node in childNodes {
                        if node.name == index {
                            return node
                        }
                    }
                    return nil
                }
            }
            
            fileprivate func addChildNode(_ node: ObjectNode) {
                childNodes.append(node)
            }
        }
        
        // Predefined objects
        private var globalObjects = ObjectNode(
            name: "\\",
            object: AMLDefName(name: AMLNameString("\\"), value: AMLIntegerData(0)),
            childNodes: [
                ObjectNode(name: "_OSI", object: AMLMethod(name: AMLNameString("_OSI"), flags: AMLMethodFlags(flags: 1), parser: nil), childNodes: []),
                ObjectNode(name: "_GL", object: AMLDefMutex(name: AMLNameString("_GL"), flags: AMLMutexFlags()), childNodes: []),
                ObjectNode(name: "_REV", object: AMLDefName(name: AMLNameString("_REV"), value: AMLIntegerData(2)), childNodes: []),
                ObjectNode(name: "_OS", object: AMLDefName(name: AMLNameString("_OS"), value: AMLString("Darwin")), childNodes: [])
            ])
        
        private func findNode(named: String, parent: ObjectNode) -> ObjectNode? {
            for node in parent.childNodes {
                if node.name == named {
                    return node
                }
            }
            return nil
        }
        
        // Remove leading '\\'
        private func removeRootChar(name: String) -> String {
            if let f = name.first, f == "\\" {
                var name2 = name
                name2.remove(at: name2.startIndex)
                return name2
            }
            return name
        }
        
        func add(_ name: String, _ object: AMLObject) {
            Log("Adding \(name) -> \(object.name.value) \(type(of: object))", level: .Info)
            //FXIME: Add properly
            return
            var parent = globalObjects
            var nameParts = removeRootChar(name: name).split(separator: AMLNameString.PathSeparatorChar)
            guard let nodeName = nameParts.last else {
                Log("\(name) has no last segment", level: .Error)
                fatalError()
            }
            
            while nameParts.count > 0 {
                let part = nameParts.removeFirst()
                if let childNode = findNode(named: String(part), parent: parent) {
                    parent = childNode
                } else {
                    let tmpScope = AMLDefScope(name: AMLNameString(String(part)), value: [])
                    let newNode = ObjectNode(name: String(part), object: tmpScope, childNodes: [])
                    parent.addChildNode(newNode)
                    parent = newNode
                }
            }
            
            guard parent.name == nodeName else {
                Log("Bad Node", level: .Error)
                fatalError()
            }
            parent.object = object
        }
        
        // Needs to be a full path starting with \
        func get(_ name: String) -> ObjectNode? {
            var name2 = name
            guard name2.remove(at: name2.startIndex) == "\\" else {
                return nil
            }
            var parent = globalObjects
            var nameParts = name2.split(separator: AMLNameString.PathSeparatorChar)
            while nameParts.count > 0 {
                let part = nameParts.removeFirst()
                if let childNode = findNode(named: String(part), parent: parent) {
                    parent = childNode
                } else {
                    return nil
                }
            }
            return parent
        }
        
        func getDataRefObject(_ name: String) -> AMLDataRefObject? {
            if let node = get(name) {
                if let o = node.object as? AMLDataRefObject {
                    return o
                }
                if let o = node.object as? AMLDefName {
                    return o.value
                }
            }
            return nil
        }
        
        func getGlobalObject(currentScope: AMLNameString, name: AMLNameString) -> (ObjectNode, String)? {
                let nameStr = name.value
                guard nameStr.first != nil else {
                    fatalError("string is empty")
                }
                
                let fullPath = resolveNameTo(scope: currentScope, path: name)
                if let obj = get(fullPath.value) {
                    return (obj, fullPath.value)
                }
                // Do a search up the tree
                guard name.isNameSeg else {
                    return nil
                }
                let seperator = AMLNameString.PathSeparatorChar
                var nameSegs = currentScope.value.split(separator: seperator)
                while nameSegs.count > 1 {
                    _ = nameSegs.popLast()
                    var tmpName = nameSegs.joined(separator: String(seperator))
                    tmpName.append(AMLNameString.PathSeparatorChar)
                    tmpName.append(name.value)
                    if let obj = get(tmpName) {
                        return (obj, tmpName)
                    }
                }
                if nameSegs.count == 1 {
                    var tmpName = "\\"
                    tmpName.append(name.value)
                    if let obj =  get(tmpName) {
                        return (obj, tmpName)
                    }
                }
                return nil
        }
        
        func walkNode(name: String, node: ObjectNode, _ body: (String, ObjectNode) -> Void) {
            body(name, node)
            node.childNodes.forEach {
                let child = $0
                let fullName = (name == "\\") ? name + child.name :
                    name + String(AMLNameString.PathSeparatorChar) + child.name
                walkNode(name: fullName, node: child, body)
            }
        }
        
        func dumpObjects() {
            walkNode(name: "\\", node: globalObjects) { (path, node) in
                Log("\(path)", level: .Debug)
            }
        }
        
        func dumpDevices() {
            let devices = getDevices()
            for device in devices {
                Log("(\(device.0), \(device.1))", level: .Debug)
            }
            Log("Have \(devices.count) devices", level: .Debug)
        }
        
        func runBody(root: String, body: (String, AMLNamedObj) -> ()) {
            guard let sb = get("\\") else {
                Log("\\ not found", level: .Error)
                fatalError()
            }
            walkNode(name: root, node: sb) { (path, node) in
                if let obj = node.object as? AMLNamedObj {
                    body(path, obj)
                }
            }
        }
        
        func getDevices() -> [(String, AMLDefDevice)] {
            guard let sb = get("\\_SB") else {
                Log("No \\_SB system bus node", level: .Error)
                fatalError()
            }
            var devices: [(String, AMLDefDevice)] = []
            walkNode(name: "\\_SB", node: sb) { (path, node) in
                if let device = node.object as? AMLDefDevice {
                    devices.append((path, device))
                }
            }
            return devices
        }
        
        // Find all of the PNP devices and call a closure with the PNP name and resource settings
        func pnpDevices(_ closure: (String, String, [AMLResourceSetting]) -> Void) {
            getDevices().forEach { (fullName, device) in
                var context = ACPI.AMLExecutionContext(scope: AMLNameString(fullName), args: [], globalObjects: self)
                if let pnpName = device.pnpName(context: &context), let crs = device.currentResourceSettings(context: &context) {
                    closure(fullName, pnpName, crs)
                }
            }
        }
    }
}
