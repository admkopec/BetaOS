//
//  Symbols.swift
//  Kernel
//
//  Created by Adam Kopeć on 1/25/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import CustomArrays

// Opcode or Character
protocol AMLSymbol { }

// ASCII 'A'-'Z' 0x41 - 0x5A
enum AMLChar {
    case NullChar, LeadNameChar, DigitChar, RootChar, ParentPrefixChar, DualNamePrefix, MultiNamePrefix
}

extension AMLNameString {
    static let RootChar = Character(UnicodeScalar("\\"))
    static let ParentPrefixChar = Character(UnicodeScalar("^"))
    static let PathSeparatorChar = Character(UnicodeScalar("."))
}

struct AMLCharSymbol: AMLSymbol, Equatable {
    let value: UInt8
    let CharType: AMLChar
    
    init?(byte: UInt8) {
        switch byte {
        case 0x00:
            CharType = .NullChar
            
        // A-Z
        case 0x41...0x5A:
            CharType = .LeadNameChar
            
        // 0-9
        case 0x30...0x39:
            CharType = .DigitChar
            
        // '_'
        case 0x5F:
            CharType = .LeadNameChar
            
        // '\'
        case 0x5C:
            CharType = .RootChar
            
        // '^'
        case 0x5E:
            CharType = .ParentPrefixChar
            
        case 0x2E:
            CharType = .DualNamePrefix
            
        case 0x2F:
            CharType = .MultiNamePrefix
            
        default:
            return nil
        }
        value = byte
    }
    
    // '_' is trailing padding
    static let paddingChar = Character(UnicodeScalar(0x5F))
    
    var character: Character { return Character(UnicodeScalar(value)) }
    var isPaddingChar: Bool { return character == AMLCharSymbol.paddingChar }
    
    var numericValueInclHex: Int? {
        if CharType == .DigitChar {
            return Int(value) - 0x30
        } else if value >= 0x41 && value <= 46 { // 'A' - 'F'
            return Int(value) - 0x41 + 10
        } else {
            return nil
        }
    }
    
    var numericValue: Int? {
        if let value = numericValueInclHex, value < 10 {
            return value
        }
        return nil
    }
}


func ==(lhs: AMLCharSymbol, rhs: AMLCharSymbol) -> Bool {
    return lhs.character == rhs.character
}


enum AMLOpcode: UInt16, AMLSymbol {
    case zeroOp             = 0x00
    case oneOp              = 0x01
    case aliasOp            = 0x06
    case nameOp             = 0x08
    case bytePrefix         = 0x0a
    case wordPrefix         = 0x0b
    case dwordPrefix        = 0x0c
    case stringPrefix       = 0x0d
    case qwordPrefix        = 0x0e     /* ACPI 2.0 */
    case scopeOp            = 0x10
    case bufferOp           = 0x11
    case packageOp          = 0x12
    case varPackageOp       = 0x13     /* ACPI 2.0 */
    case methodOp           = 0x14
    case externalOp         = 0x15
    case extendedOpPrefix   = 0x5b
    
    // Extended 2byte opcodes
    case mutexOp            = 0x5b01
    case eventOp            = 0x5b02
    case condRefOfOp        = 0x5b12
    case createFieldOp      = 0x5b13
    case loadTableOp        = 0x5b1f
    case loadOp             = 0x5b20
    case stallOp            = 0x5b21
    case sleepOp            = 0x5b22
    case acquireOp          = 0x5b23
    case signalOp           = 0x5b24
    case waitOp             = 0x5b25
    case resetOp            = 0x5b26
    case releaseOp          = 0x5b27
    case fromBCDOp          = 0x5b28
    case toBCDOp            = 0x5b29
    case unloadOp           = 0x5b2a
    case revisionOp         = 0x5b30
    case debugOp            = 0x5b31
    case fatalOp            = 0x5b32
    case timerOp            = 0x5b33
    case opRegionOp         = 0x5b80
    case fieldOp            = 0x5b81
    case deviceOp           = 0x5b82
    case processorOp        = 0x5b83
    case powerResOp         = 0x5b84
    case thermalZoneOp      = 0x5b85
    case indexFieldOp       = 0x5b86
    case bankFieldOp        = 0x5b87
    case dataRegionOp       = 0x5b88
    
    case local0Op           = 0x60
    case local1Op           = 0x61
    case local2Op           = 0x62
    case local3Op           = 0x63
    case local4Op           = 0x64
    case local5Op           = 0x65
    case local6Op           = 0x66
    case local7Op           = 0x67
    case arg0Op             = 0x68
    case arg1Op             = 0x69
    case arg2Op             = 0x6a
    case arg3Op             = 0x6b
    case arg4Op             = 0x6c
    case arg5Op             = 0x6d
    case arg6Op             = 0x6e
    case storeOp            = 0x70
    case refOfOp            = 0x71
    case addOp              = 0x72
    case concatOp           = 0x73
    case subtractOp         = 0x74
    case incrementOp        = 0x75
    case decrementOp        = 0x76
    case multiplyOp         = 0x77
    case divideOp           = 0x78
    case shiftLeftOp        = 0x79
    case shiftRightOp       = 0x7a
    case andOp              = 0x7b
    case nandOp             = 0x7c
    case orOp               = 0x7d
    case norOp              = 0x7e
    case xorOp              = 0x7f
    case notOp              = 0x80
    case findSetLeftBitOp   = 0x81
    case findSetRightBitOp  = 0x82
    case derefOfOp          = 0x83
    case concatResOp        = 0x84     /* ACPI 2.0 */
    case modOp              = 0x85     /* ACPI 2.0 */
    case notifyOp           = 0x86
    case sizeOfOp           = 0x87
    case indexOp            = 0x88
    case matchOp            = 0x89
    case createDWordFieldOp = 0x8a
    case createWordFieldOp  = 0x8b
    case createByteFieldOp  = 0x8c
    case createBitFieldOp   = 0x8d
    case objectTypeOp       = 0x8e
    case createQWordFieldOp = 0x8f     /* ACPI 2.0 */
    case lAndOp             = 0x90
    case lOrOp              = 0x91
    case lNotOp             = 0x92
    case lNotEqualOp        = 0x9293    // Combination
    case lLessEqualOp       = 0x9294    // Combination
    case lGreaterEqualOp    = 0x9295    // Combination
    
    case lEqualOp           = 0x93
    case lGreaterOp         = 0x94
    case lLessOp            = 0x95
    case toBufferOp         = 0x96     /* ACPI 2.0 */
    case toDecimalStringOp  = 0x97     /* ACPI 2.0 */
    case toHexStringOp      = 0x98     /* ACPI 2.0 */
    case toIntegerOp        = 0x99     /* ACPI 2.0 */
    case toStringOp         = 0x9c     /* ACPI 2.0 */
    case copyObjectOp       = 0x9d     /* ACPI 2.0 */
    case midOp              = 0x9e     /* ACPI 2.0 */
    case continueOp         = 0x9f     /* ACPI 2.0 */
    case ifOp               = 0xa0
    case elseOp             = 0xa1
    case whileOp            = 0xa2
    case noopOp             = 0xa3
    case returnOp           = 0xa4
    case breakOp            = 0xa5
    case breakPointOp       = 0xcc
    case onesOp             = 0xff
    
    
    init?(byte: UInt8) {
        self.init(rawValue: UInt16(byte))
    }
    
    
    var isTwoByteOpcode: Bool {
        return ByteArray(self.rawValue)[0] == AMLOpcode.extendedOpPrefix.rawValue
    }
}
