//
//  mem.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/22/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

//void* memcpy(void* dstptr, const void* srcptr, size_t size) {
//    unsigned char* dst = (unsigned char*) dstptr;
//    const unsigned char* src = (const unsigned char*) srcptr;
//    for ( size_t i = 0; i < size; i++ ) {
//        dst[i] = src[i];
//        printf("");
//    }
//    return dstptr;
//}

extern "C" { extern bool isSSEenabled; extern bool canUseSSEmemcpy; extern void* memcpy_(void * const to, const void * const from, size_t n); }

void *memcpy(void *to, const void *from, size_t n) {
    uintptr_t _from = (uintptr_t)from;
    uintptr_t _to = (uintptr_t)to;
    if(isSSEenabled && canUseSSEmemcpy) {
        size_t i;
        for(i=0; i < n/16; i++) {
            __asm__ __volatile__ ("movups (%0), %%xmm0\n" "movntdq %%xmm0, (%1)\n"::"r"(_from), "r"(_to) : "memory");

            _from += 16;
            _to += 16;
        }
    } else {
        return memcpy_(to, from, n);
    }
    if(n & 7) {
        n = n&7;
        int d0, d1, d2;
        __asm__ __volatile__(
                             "rep ; movsl\n\t"
                             "testb $2,%b4\n\t"
                             "je 1f\n\t"
                             "movsw\n"
                             "1:\ttestb $1,%b4\n\t"
                             "je 2f\n\t"
                             "movsb\n"
                             "2:"
                             : "=&c" (d0), "=&D" (d1), "=&S" (d2)
                             :"0" (n/4), "q" (n),"1" ((long) _to),"2" ((long) _from)
                             : "memory");
    }
    return (to);
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

void* memsetw(void* dest, uint16_t w, size_t count) {
    return memset(dest, w, count);
}

/*
 * Compare memory regions.
 */
int memcmp(const void *s1, const void *s2, size_t size) {
    if (size != 0) {
        const unsigned char *p1 = (const uint8_t *)s1, *p2 = (const uint8_t *)s2;
        do {
            if (*p1++ != *p2++)
                return (*--p1 - *--p2);
        } while (--size != 0);
    }
    return (0);
}

void* memchr(const void *s1, int c, size_t size) {
    unsigned char *p = (unsigned char*)s1;
    while( size-- )
        if( *p != (unsigned char)c )
            p++;
        else
            return p;
    return 0;
}
