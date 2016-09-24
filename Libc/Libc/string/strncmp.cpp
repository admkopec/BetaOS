//
//  strncmp.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/22/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <string.h>
#include <stddef.h>

int strncmp(const char *s1, const char *s2, size_t n) {
    for ( ; n > 0; s1++, s2++, --n)
        if (*s1 != *s2)
            return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
        else if (*s1 == '\0')
            return 0;
    return 0;
}