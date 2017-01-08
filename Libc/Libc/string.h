//
//  string.h
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void itoa(char *buf, unsigned long int n, int base);
size_t strlen(const char*);
int strcmp(const char* str1, const char* str2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlcpy(char * dst, const char * src, size_t maxlen);
    
void* memcpy  (void* dstptr, const void* srcptr, size_t size);
void* memmove (void* dstptr, const void* srcptr, size_t size);
void* memset  (void* bufptr, int         value,  size_t size);
int   memcmp  (const void* s1, const void* s2,   size_t size);

#ifdef __cplusplus
}
#endif

#endif /* string_h */
