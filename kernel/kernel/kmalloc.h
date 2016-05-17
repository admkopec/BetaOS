//
//  kmalloc.h
//  BetaOS
//
//  Created by Adam Kopeć on 5/14/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef kmalloc_h
#define kmalloc_h

#include <stddef.h>

void* memcpy  (void* dstptr, const void* srcptr, size_t size);
void* memmove (void* dstptr, const void* srcptr, size_t size);
void* memset  (void* bufptr, int         value,  size_t size);

#endif /* kmalloc_h */
