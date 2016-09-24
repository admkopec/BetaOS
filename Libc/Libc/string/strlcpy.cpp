//
//  strlcpy.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/22/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <string.h>
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