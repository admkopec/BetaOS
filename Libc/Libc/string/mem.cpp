//
//  mem.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/22/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include <stddef.h>

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;
    for ( size_t i = 0; i < size; i++ ) {
        dst[i] = src[i];
        printf("");
    }
    return dstptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;
    if ( dst < src )
        for ( size_t i = 0; i < size; i++ )
            dst[i] = src[i];
    else
        for ( size_t i = size; i != 0; i-- )
            dst[i-1] = src[i-1];
    return dstptr;
}

void* memset(void* bufptr, int value, size_t size) {
    unsigned char* buf = (unsigned char*) bufptr;
    for ( size_t i = 0; i < size; i++ )
        buf[i] = (unsigned char) value;
    return bufptr;
}

/*
 * Compare memory regions.
 */
int
memcmp(const void *s1, const void *s2, size_t size) {
    if (size != 0) {
        register const unsigned char *p1 = (const uint8_t *)s1, *p2 = (const uint8_t *)s2;
        do {
            if (*p1++ != *p2++)
                return (*--p1 - *--p2);
        } while (--size != 0);
    }
    return (0);
}
