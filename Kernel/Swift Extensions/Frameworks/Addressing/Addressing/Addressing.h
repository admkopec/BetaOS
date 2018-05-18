//
//  Addressing.h
//  Addressing
//
//  Created by Adam Kopeć on 11/5/17.
//  Copyright © 2017-2018 Adam Kopeć. All rights reserved.
//

#import <stdbool.h>

extern unsigned long long io_map(unsigned long long phys_addr, unsigned long size, unsigned int flags);
extern unsigned long long kvtophys(unsigned long long addr);
extern bool isAHCI; // A very, very, very ugly hack for AHCI, but it's at least working

