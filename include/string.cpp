//
//  string.cpp
//  OS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include <string.h>

size_t strlen(const char* str) {
    size_t ret = 0;
    while ( str[ret] != 0 )
    ret++;
    return ret;
}

bool streql(const char* str1, const char* str2) {
    if (strlen(str1)!=strlen(str2)) {
        return false;
    }
    else {
        for (uint8_t i=0; i<=strlen(str1); i++) {
            if (str1[i] != str2[i]) {
                return false;
            }
        }
    }
    return true;
}