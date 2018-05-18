//
//  runtime.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import <objc/NSObject.h>
#import <objc/runtime.h>
#import <objc/objc.h>

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

id
objc_alloc(Class cls) {
    if (!cls) {
        return Nil;
    }
    return [cls alloc];
}

id
objc_allocWithZone(Class cls) {
    if (!cls) {
        return Nil;
    }
    return [cls alloc];
//    return [cls allocWithZone];
}

id
objc_storeWeak(id *location, id obj) {
    *location = obj;
    return obj;
}

void
objc_getProperty() {
    
}

id
objc_initWeak(id *location, id newObj) {
    if (!newObj) {
        *location = nil;
        return nil;
    }
    *location = [[newObj alloc] init];
    return newObj;
}

Class
objc_getClass(const char* aClassName) {
    if (!aClassName) {
        return Nil;
    }
    return objc_lookUpClass(aClassName);
}
