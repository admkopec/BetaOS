//
//  strlcpy.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/22/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

size_t strlcpy(char * dst, const char * src, size_t maxlen) {
    const size_t srclen = strlen(src);
    if (srclen < maxlen) {
        memcpy(dst, src, srclen+1); // memcpy is not working
    } else if (maxlen != 0) {
        memcpy(dst, src, maxlen-1);
        dst[maxlen-1] = '\0';
    }
    return srclen;
}

char *
strncpy(char *dest, const char *src, size_t maxlen) {
    strlcpy(dest, src, maxlen);
    return dest;
}

char *
strcpy(char *dest, const char *src) {
    unsigned i;
    for (i = 0; src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    return dest;
}

char *
strcat(char *dest, const char *src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

char *
strchr(const char *s, int c) {
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char *)s;
}

char *
strdup(const char *s) {
    size_t len = strlen(s);
    char *dup = (char*)malloc(len + 1);
    if (dup != NULL) {
        strcpy(dup, s);
    }
    
    return dup;
}


char *
strndup(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) {
        len = n;
    }
    
    char *dup = (char*)malloc(len + 1);
    if (dup != NULL) {
        memcpy(dup, s, len);
        *(dup + len) = '\0';
    }
    
    return dup;
}
