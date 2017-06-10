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

/*#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif*/

class OSObject {
protected:
    virtual void release();
    //virtual void free();
    virtual OSObject* alloc();
    
public:
    virtual ~OSObject();
};

#endif /* OSObject_hpp */
