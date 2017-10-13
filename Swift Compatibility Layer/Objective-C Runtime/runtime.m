//
//  runtime.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import "runtime.h"
#import "objc.h"

void objc_release(id);
id objc_autorelease(id);
id objc_retain(id);

id
objc_retain_autorelease(id obj) {
    return objc_autorelease(objc_retain(obj));
}

id
objc_retainAutorelease(id obj) {
    return objc_retain_autorelease(obj);
}

__attribute__((noinline))
static id
objc_retainAutoreleaseAndReturn(id obj) {
    return objc_retainAutorelease(obj);
}

id
objc_retainAutoreleaseReturnValue(id obj) {
    return objc_retainAutoreleaseAndReturn(obj);
}

void
objc_storeStrong(id *location, id obj) {
    id prev = *location;
    if (obj == prev) {
        return;
    }
    
    objc_retain(obj);
    *location = obj;
    objc_release(prev);
}

void
objc_getProperty() {
    
}

Class
objc_getClass(const char* aClassName) {
    if (!aClassName) {
        return Nil;
    }
    return Nil;
}

Class
object_getClass(id object) {
    if (object != nil) {
        return object->isa;
    } else {
        return Nil;
    }
}
