//
//  OSObject.cpp
//  Libc++
//
//  Created by Adam Kopeć on 6/7/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "OSObject.hpp"

void OSObject::release() {
    delete this;
}

OSObject* OSObject::alloc() {
    OSObject* me = new OSObject;
    if (!me) {
        me->release();
        return 0;
    }
    
    return me;
}

OSObject::~OSObject() {
    
}
