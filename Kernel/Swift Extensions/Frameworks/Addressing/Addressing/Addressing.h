//
//  Addressing.h
//  Addressing
//
//  Created by Adam Kopeć on 11/5/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

//#import <Cocoa/Cocoa.h>

//! Project version number for Addressing.
//FOUNDATION_EXPORT double AddressingVersionNumber;

//! Project version string for Addressing.
//FOUNDATION_EXPORT const unsigned char AddressingVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <Addressing/PublicHeader.h>

extern unsigned long long io_map(unsigned long long phys_addr, unsigned long size, unsigned int flags);
