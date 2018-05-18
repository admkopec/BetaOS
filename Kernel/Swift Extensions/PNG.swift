//
//  PNG.swift
//  Kernel
//
//  Created by Adam Kopeć on 3/28/18.
//  Copyright © 2018 Adam Kopeć. All rights reserved.
//

import CustomArrays
import Loggable

class PNG: File, Loggable {
    let Name: String = "PNG"
    fileprivate(set) var view: View!
    
    required init?(partition: Partition, path: String) {
        super.init(partition: partition, path: path)
        if !file.Data[0 ... 7].elementsEqual([0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A]) {
            Log("Supplied file is not PNG Image", level: .Error)
            return nil
        }
        let header = Header(data: Data(rebasing: file.Data[8 ... 28]))
        Log("Length: \(header.Length), Type: \(header.TypeName), Width: \(header.Width), Height: \(header.Height), Depth: \(header.Depth)\nColor: \(header.ColorType), Compression: \(header.Compression), Filter: \(header.Filter), Interlace: \(header.Interlace)", level: .Info)
    }
    
    class Chunk {
        let Length: UInt32 // Big Endian
        let TypeName: String
        // Data ...
        
        required init(data: Data) {
            Length = ByteArray(data[0 ... 3]).BigEndianasUInt32
            TypeName = "\(UnicodeScalar(data[4]))\(UnicodeScalar(data[5]))\(UnicodeScalar(data[6]))\(UnicodeScalar(data[7]))"
        }
    }
    
    class Header: Chunk {
        let Width: UInt32  // Big Endian
        let Height: UInt32 // Big Endian
        let Depth: UInt8
        let ColorType: UInt8
        let Compression: UInt8
        let Filter: UInt8
        let Interlace: UInt8
        
        required init(data: Data) {
            Width = ByteArray(data[8 ... 11]).BigEndianasUInt32
            Height = ByteArray(data[12 ... 15]).BigEndianasUInt32
            Depth = data[16]
            ColorType = data[17]
            Compression = data[18]
            Filter = data[19]
            Interlace = data[20]
            
            super.init(data: data)
        }
    }
}
