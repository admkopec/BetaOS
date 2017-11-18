//
//  OSRuntime.hpp
//  Libc++
//
//  Created by Adam Kopeć on 6/7/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef OSRuntime_hpp
#define OSRuntime_hpp

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class OSRuntime {
public:
    static void* OSMalloc(size_t size);
    static void  OSFree(void * addr);
};

typedef int          OSReturn;
extern  const char * OSReturnStrings[4];

#define kOSReturnSuccess  0
#define kOSReturnTimeout -1
#define kOSReturnFailed  -2
#define kOSReturnError   -3

#endif /* OSRuntime_hpp */
