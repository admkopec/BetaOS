//
//  Method.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/26/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

// ACPI Method Invocation

extension ACPI {
    struct AMLExecutionContext {
        let scope: AMLNameString
        let args: AMLTermArgList
        let globalObjects: ACPI.GlobalObjects
        var endOfMethod = false
        
        private var _returnValue: AMLTermArg? = nil
        var returnValue: AMLTermArg? {
            mutating get {
                let ret = _returnValue
                _returnValue = nil
                return ret
            }
            set {
                _returnValue = newValue
            }
        }
        var localObjects: [AMLTermArg?] = Array(repeatElement(nil, count: 8))
        
        
        init(scope: AMLNameString, args: AMLTermArgList, globalObjects: ACPI.GlobalObjects) {
            self.scope = scope
            self.args = args
            self.globalObjects = globalObjects
        }
        
        func withNewScope(_ newScope: AMLNameString) -> AMLExecutionContext {
            return AMLExecutionContext(scope: newScope, args: [], globalObjects: globalObjects)
        }
        
        mutating func execute(termList: AMLTermList) throws {
            for termObj in termList {
                if let op = termObj as? AMLType2Opcode {
                    // FIXME, should something be done with the result or maybe it should
                    // only be returned in the context
                    _ = try op.execute(context: &self)
                } else if let op = termObj as? AMLType1Opcode {
                    try op.execute(context: &self)
                } else if let op = termObj as? AMLNamedObj {
                    try op.createNamedObject(context: &self)
                } else if let op = termObj as? AMLNameSpaceModifierObj {
                    try op.execute(context: &self)
                } else {
                    fatalError("Unknown op: \(type(of: termObj))")
                }
                if endOfMethod {
                    return
                }
            }
        }
    }
    
    func invokeMethod(name: String, _ args: Any...) throws -> AMLTermArg? {
        var methodArgs: AMLTermArgList = []
        for arg in args {
            if let arg = arg as? String {
                methodArgs.append(AMLString(arg))
            } else if let arg = arg as? AMLInteger {
                methodArgs.append(AMLIntegerData(AMLInteger(arg)))
            } else {
                throw AMLError.invalidData(reason: "Bad data: \(arg)")
            }
        }
        guard let mi = /*try*/ AMLMethodInvocation(method: AMLNameString(name), args: methodArgs) else { return nil }
        var context = AMLExecutionContext(scope: mi.method, args: [], globalObjects: globalObjects)
        
        return try mi.execute(context: &context)
    }
    
    
    static func _OSI_Method(_ args: AMLTermArgList) throws -> AMLTermArg {
        guard args.count == 1 else {
            throw AMLError.invalidData(reason: "_OSI: Should only be 1 arg")
        }
        guard let arg = args[0] as? AMLString else {
            throw AMLError.invalidData(reason: "_OSI: is not a string")
        }
        if arg.value == "Darwin" {
            return AMLIntegerData(0xffffffff)
        } else {
            return AMLIntegerData(0)
        }
    }
}
