//
//  Types.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/26/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

typealias AMLByteBuffer  = Data
typealias AMLInteger     = UInt64
typealias AMLTermList    = [AMLTermObj]
typealias AMLByteData    = UInt8
typealias AMLByteList    = [AMLByteData]
typealias AMLWordData    = UInt16
typealias AMLDWordData   = UInt32
typealias AMLQWordData   = UInt64
typealias AMLTermArgList = [AMLTermArg]
typealias AMLPkgLength   = UInt
typealias AMLObjectList  = [AMLObject] // FIXME: ObjectList should be more specific

protocol AMLTermObj {
}

protocol AMLTermArg {
    func canBeConverted(to: AMLDataRefObject) -> Bool
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg
}

extension AMLTermArg {
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        return false
    }
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        return self
    }
}

protocol AMLObject {
    var name: AMLNameString { get }
}

protocol AMLBuffPkgStrObj: AMLTermArg {
}

protocol AMLDataRefObject: AMLBuffPkgStrObj {
    var isReadOnly: Bool { get }
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext)
}

extension AMLDataRefObject {
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        if isReadOnly {
            fatalError("updateValue denied")
        } else {
            fatalError("Missing updateValue function for \(self)")
        }
    }
}

protocol AMLTarget {
    //var value: AMLDataRefObject { get set }
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext)
}

protocol AMLSuperName: AMLTarget {
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext)
}

extension AMLSuperName {
    // func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
    //     fatalError("\(self) can not be read from")
    // }
    
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        fatalError("\(self) can not be written to")
    }
}

protocol AMLNameSpaceModifierObj: AMLTermObj, AMLObject {
    //var name: AMLNameString { get }
    func execute(context: inout ACPI.AMLExecutionContext) throws
}

protocol AMLSimpleName: AMLSuperName {}
protocol AMLType6Opcode: AMLSuperName, AMLBuffPkgStrObj {}
protocol AMLDataObject: AMLDataRefObject {}
protocol AMLComputationalData: AMLDataObject {}
protocol AMLFieldElement {}
protocol AMLConnectField: AMLFieldElement {}
protocol AMLConstObj: AMLComputationalData {}

extension AMLConstObj {
    var isReadOnly: Bool { return true }
}

typealias AMLFieldList = [AMLFieldElement]
typealias AMLPredicate = AMLTermArg // => Integer
typealias AMLDDBHandleObject = AMLSuperName
typealias AMLMutexObject = AMLSuperName
typealias AMLEventObject = AMLSuperName
typealias AMLObjectReference = AMLInteger

class AMLIntegerData: AMLDataObject, AMLTermArg, AMLTermObj {
    var value: AMLInteger
    let isReadOnly = false
    
    init(_ value: AMLInteger) {
        self.value = value
    }
    
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        if to is AMLIntegerData {
            return true
        }
        if let _to = to as? AMLNamedField {
            return _to.bitWidth <= AMLInteger.bitWidth
        }
        
        return false
    }
    
    func updateValue(to operand: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        guard let result = operand.evaluate(context: &context) as? AMLIntegerData else {
            fatalError("\(operand) does not evaluate to an integer")
        }
        value = result.value
    }
}

struct AMLNameString: AMLSimpleName, AMLBuffPkgStrObj, AMLTermArg {
    let value: String
    var isNameSeg: Bool { return (value.count <= 4) }
    // Name starts with '\\'
    var isFullPath: Bool { return value.first == AMLNameString.RootChar }
    
    init(_ value: String) {
        self.value = value
        // self.value = AMLString(value)
    }
    
    // The last segment. If only one segment, removes the root '\\'
    var shortName: AMLNameString {
        if value == String(AMLNameString.RootChar) {
            return self
        }
        
//        let segs = value.components(separatedBy: AMLNameString.PathSeparatorChar)
        let segs = value.split(separator: AMLNameString.PathSeparatorChar)
        if segs.count > 1 {
            return AMLNameString(String(segs.last!))
        } else {
            if value.first == AMLNameString.RootChar {
                var name = value
                name.remove(at: value.startIndex)
                return AMLNameString(name)
            }
        }
        return AMLNameString(value)
    }
    
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        if to is AMLFieldElement {
            return true
        }
        return false
    }
    
    func parent() -> AMLNameString {
        let seperator = AMLNameString.PathSeparatorChar
        var parentSegs = value.split(separator: seperator)
//        var parentSegs = value.components(separatedBy: seperator)
        parentSegs.removeLast()
        let result = parentSegs.joined(separator: String(seperator))
        return AMLNameString(result)
    }
    
    func replaceLastSeg(with newSeg: AMLNameString?) -> AMLNameString {
        let seperator = AMLNameString.PathSeparatorChar
//        var parentSegs = value.components(separatedBy: seperator)
        var parentSegs = value.split(separator: seperator)
        //let child = newSeg._value.components(separatedBy: seperator).last()
        parentSegs.removeLast()
        if let segment = newSeg {
            parentSegs.append(Substring(segment.value))
        }
        if parentSegs.count == 0 {
            return AMLNameString("\\")
        }
        let result = parentSegs.joined(separator: String(seperator))
        return AMLNameString(result)
    }
    
    func removeLastSeg() -> AMLNameString {
        return replaceLastSeg(with: nil)
    }
    
    static func ==(lhs: AMLNameString, rhs: AMLNameString) -> Bool {
        return lhs.value == rhs.value
    }
    
    static func ==(lhs: AMLNameString, rhs: String) -> Bool {
        return lhs.value == rhs
    }
    
    static func ==(lhs: String, rhs: AMLNameString) -> Bool {
        return lhs == rhs.value
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        let scope = context.scope
        guard let (node, fullPath) = context.globalObjects.getGlobalObject(currentScope: scope, name: self) else {
            fatalError("Cant find node: \(value)")
        }

        let namedObject = node.object
        if let fieldElement = namedObject as? AMLNamedField {
            let resolvedScope = AMLNameString(fullPath).removeLastSeg()
            var tmpContext = ACPI.AMLExecutionContext(scope: resolvedScope,
                                                      args: [],
                                                      globalObjects: context.globalObjects)
            return fieldElement.evaluate(context: &tmpContext)
            //fieldElement.setOpRegion(context: tmpContext)
            //return AMLIntegerData(fieldElement.resultAsInteger ?? 0)
        } else if let n = namedObject as? AMLNamedObj {
            return n.readValue(context: &context)
        } else if let termArg = namedObject as? AMLTermArg {
            return termArg
        } else if let namedObj = namedObject as? AMLDefName {
            return namedObj.value
        } else {
            fatalError("namedObject: \(namedObject) could not execute")
        }
    }
    
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        print("Updating value of \(self) to \(to)")
        // Update Value
    }
}


struct AMLNullName: AMLTarget {
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        // Ignore Updates to nullname
    }
}

// Subtypes used in structs
struct AMLMethodFlags {
    // bit 0-2: ArgCount (0-7)
    // bit 3: SerializeFlag: 0 NotSerialized 1 Serialized
    // bit 4-7: SyncLevel (0x00-0x0f)
    
    let flags: AMLByteData
    var argCount: Int { return Int(flags & 7) }
    var isSerialized: Bool { return flags.bit(3) }
    var syncLevel: Int { return Int(flags >> 4) }
    
    init(flags: AMLByteData) {
        self.flags = flags
    }
    
    init(argCount: Int, isSerialized: Bool, syncLevel: Int) {
        var f = UInt8(UInt8(argCount) & 0x7)
        f |= isSerialized ? 8 : 0
        f |= UInt8((syncLevel & 0xf) << 4)
        flags = f
    }
}

struct AMLMutexFlags {
    // bit 0-3: SyncLevel (0x00-0x0f)
    // bit 4-7: Reserved (must be 0)
    
    let flags: AMLByteData
    
    init() {
        self.flags = 0
    }
    
    init?(flags: AMLByteData) /*throws*/ {
        /*try*/ self.init(syncLevel: flags)
    }
    
    init?(syncLevel: UInt8) /*throws*/ {
        guard syncLevel & 0x0f == syncLevel else {
//            throw AMLError.invalidData(reason: "Invalid synclevel \(syncLevel)")
            return nil
        }
        self.flags = syncLevel
    }
}


// AMLTermArg
struct AMLArgObj: AMLTermArg, AMLSimpleName, AMLBuffPkgStrObj, AMLTermObj {
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        fatalError("\(self) is readOnly")
    }
    
    
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        return false
    }
    
    
    let opcode: AMLOpcode      // FIXME needs better type
    var argIdx: Int { return Int(opcode.rawValue - AMLOpcode.arg0Op.rawValue) }
    
    init?(argOp: AMLOpcode) /*throws*/ {
        switch argOp {
        case .arg0Op, .arg1Op, .arg2Op, .arg3Op, .arg4Op, .arg5Op, .arg6Op:
            opcode = argOp
            
        default: return nil//throw AMLError.invalidData(reason: "Invalid arg")
        }
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        return context.args[argIdx]
    }
}


struct AMLLocalObj: AMLTermArg, AMLSimpleName, AMLBuffPkgStrObj, AMLTermObj {
    let opcode: AMLOpcode      // FIXME needs better type
    var argIdx: Int { return Int(opcode.rawValue - AMLOpcode.local0Op.rawValue) }
    
    init?(localOp: AMLOpcode) /*throws*/ {
        switch localOp {
        case .local0Op, .local1Op, .local2Op, .local3Op,
             .local4Op, .local5Op, .local6Op, .local7Op:
            opcode = localOp
            
        default: return nil//throw AMLError.invalidData(reason: "Invalid arg")
        }
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        let v = context.localObjects[argIdx]!
        let r = v.evaluate(context: &context)
        return r
    }
    
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        context.localObjects[argIdx] = to
    }
}


struct AMLDebugObj: AMLSuperName, AMLDataRefObject, AMLTarget {
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        return false
    }
    
    var isReadOnly: Bool  { return false }
    
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        print("DEBUG:", to)
    }
}

struct AMLNamedField: AMLFieldElement, AMLDataObject, AMLNamedObj {
    var isReadOnly: Bool = false
    let name: AMLNameString
    let bitOffset: UInt
    let bitWidth: UInt
    let fieldRef: AMLDefFieldRef
    
    init?(name: AMLNameString, bitOffset: UInt, bitWidth: UInt, fieldRef: AMLDefFieldRef) /*throws*/ {
        guard name.isNameSeg else {
            return nil
//            throw AMLError.invalidData(reason: "\(name) is not a NameSeg")
        }
        self.name = name
        self.bitOffset = bitOffset
        self.bitWidth = bitWidth
        self.fieldRef = fieldRef
    }
    
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        let value = operandAsInteger(operand: to, context: &context)
        setOpRegion(context: context)
        let region = fieldRef.getRegionSpace(context: &context)
        region.write(bitOffset: Int(bitOffset),
                     width: Int(bitWidth),
                     value: value)
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        setOpRegion(context: context)
        let region = fieldRef.getRegionSpace(context: &context)
        let value = region.read(bitOffset: Int(bitOffset), width: Int(bitWidth))
        return AMLIntegerData(value)
    }
    
    private func setOpRegion(context: ACPI.AMLExecutionContext) {
        if fieldRef.opRegion == nil {
            guard let opRegionName = fieldRef.amlDefField?.name else {
                fatalError("cant get opRegionanme")
            }
            if let (opNode, _) = context.globalObjects.getGlobalObject(currentScope: context.scope, name: opRegionName) {
                if let opRegion = opNode.object as? AMLDefOpRegion {
                    fieldRef.opRegion = opRegion
                    return
                } else {
                    kprint("opNode \(opNode)")
                }
            } else {
                fatalError("Cant find \(opRegionName) in \(context.scope)")
            }
            fatalError("No valid opRegion found")
        }
    }
}

struct AMLReservedField: AMLFieldElement {
    let pkglen: AMLPkgLength
}

struct AMLAccessType {
    let value: AMLByteData
}

struct AMLAccessField: AMLFieldElement {
    let type: AMLAccessType
    let attrib: AMLByteData
}

enum AMLExtendedAccessAttrib: AMLByteData {
    case attribBytes = 0x0B
    case attribRawBytes = 0x0E
    case attribRawProcess = 0x0F
}

struct AMLExtendedAccessField: AMLFieldElement {
    let type: AMLAccessType
    let attrib: AMLExtendedAccessAttrib
    let length: AMLIntegerData
}

// AMLNameSpaceModifierObj
struct AMLDefAlias: AMLNameSpaceModifierObj {
    func execute(context: inout ACPI.AMLExecutionContext) throws {
        
    }
    
    var name: AMLNameString { return aliasObject }
    let sourceObject: AMLNameString
    let aliasObject: AMLNameString
}

struct AMLDefName: AMLNameSpaceModifierObj {
    let name: AMLNameString
    let value: AMLDataRefObject
    
    func execute(context: inout ACPI.AMLExecutionContext) throws {
        let fullPath = resolveNameTo(scope: context.scope, path: name)
        context.globalObjects.add(fullPath.value, self)
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) throws -> AMLTermArg {
        return value
    }
}

struct AMLDefScope: AMLNameSpaceModifierObj {
    // ScopeOp PkgLength NameString TermList
    let name: AMLNameString
    let value: AMLTermList
    
    
    func execute(context: inout ACPI.AMLExecutionContext) throws {
        throw AMLError.unimplemented("\(type(of: self))")
        
    }
}

struct AMLEvent {
    // EventOp NameString
    let name: AMLNameString
}

// AMLType6Opcode
struct AMLUserTermObj: AMLType6Opcode {
    func updateValue(to: AMLTermArg, context: inout ACPI.AMLExecutionContext) {
        fatalError("Here")
    }
}

func AMLByteConst(_ v: AMLByteData) -> AMLIntegerData {
    return AMLIntegerData(AMLInteger(v))
}

func AMLWordConst(_ v: AMLWordData) -> AMLIntegerData {
    return AMLIntegerData(AMLInteger(v))
}

func AMLDWordConst(_ v: AMLDWordData) -> AMLIntegerData {
    return AMLIntegerData(AMLInteger(v))
}

func AMLQWordConst(_ v: AMLQWordData) -> AMLIntegerData {
    return AMLIntegerData(AMLInteger(v))
}

func resolveNameTo(scope: AMLNameString, path: AMLNameString) -> AMLNameString {
    if let x = path.value.first {
        if path.isFullPath {
            return path
        }
        var newScope = scope.value
        var newPath = path.value
        if x == AMLNameString.ParentPrefixChar {
            newPath = ""
            var parts = scope.value.split(separator: AMLNameString.PathSeparatorChar)
//            var parts = scope.value.components(separatedBy: AMLNameString.PathSeparatorChar)
            for ch in path.value {
                if ch == AMLNameString.ParentPrefixChar {
                    _ = parts.popLast()
                } else {
                    newPath.append(ch)
                }
            }
            newScope = parts.joined(separator: String(AMLNameString.PathSeparatorChar))
        }
        if !newScope.isEmpty, newScope != String(AMLNameString.RootChar) {
            newScope.append(AMLNameString.PathSeparatorChar)
        }
        newScope.append(newPath)
        return AMLNameString(newScope)
    } else {
        return scope // path is empty
    }
}


struct AMLString: AMLDataRefObject, AMLTermObj {
    var isReadOnly: Bool { return false }
    var value: String
    
    init(_ v: String) {
        value = v
    }
}

struct AMLZeroOp: AMLConstObj {
    // ZeroOp
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        return true
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        return AMLIntegerData(0)
    }
}

struct AMLOneOp: AMLConstObj {
    // OneOp
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        return true
    }
    
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        return AMLIntegerData(1)
    }
}

struct AMLOnesOp: AMLConstObj {
    // OnesOp
    func canBeConverted(to: AMLDataRefObject) -> Bool {
        return true
    }
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        return AMLIntegerData(0xff)
    }
}

struct AMLRevisionOp: AMLConstObj {
    // RevisionOp - AML interpreter supports revision 2
    func evaluate(context: inout ACPI.AMLExecutionContext) -> AMLTermArg {
        return AMLIntegerData(2)
    }
}


// AMLDataObject
struct AMLDDBHandle: AMLDataRefObject {
    let isReadOnly = true
    
    let value: AMLInteger
}
