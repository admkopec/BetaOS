//
//  memcpy.cpp
//  OS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <string.h>

void* memcpy(void* dstptr, const void* srcptr, size_t size)
{
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for ( size_t i = 0; i < size; i++ )
		dst[i] = src[i];
	return dstptr;
}
