//
//  streql.cpp
//  OS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <string.h>

bool strcmp(const char* str1, const char* str2) {
    if (strlen(str1)!=strlen(str2)) {
        return false;
    }
    else {
        for (unsigned char i=0; i<=strlen(str1); i++) {
            if (str1[i] != str2[i]) {
                return false;
            }
        }
    }
    return true;
}
