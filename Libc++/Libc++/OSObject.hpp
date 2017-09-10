//
//  OSObject.hpp
//  Libc++
//
//  Created by Adam Kopeć on 6/7/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef OSObject_hpp
#define OSObject_hpp

#include "OSRuntime.hpp"

class OSObject {
protected:
    virtual void release();
    //virtual void free();
    virtual OSObject* alloc();
    
public:
    virtual ~OSObject();
};

#endif /* OSObject_hpp */
