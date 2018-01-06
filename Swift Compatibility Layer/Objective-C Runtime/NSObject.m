//
//  NSObject.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import <objc/NSObject.h>
#import <objc/objc.h>
#import <objc/runtime.h>
#import <assert.h>

extern id _objc_rootRetain(id a);
extern void _objc_rootRelease(id a);
extern id _objc_rootAutorelease(id a);
extern id _objc_rootInit(id a);
extern void _objc_rootDealloc(id a);
extern BOOL _objc_rootTryRetain(id a);
extern NSUInteger _objc_rootRetainCount(id a);

@implementation NSObject

+ (void) initialize {
//    return self;
}

+ (Class) class {
    return self;
}

- (Class) class {
    return object_getClass(self);
}

- (id) self {
    return (id)self;
}

+ (id) self {
    return self;
}

+ (BOOL)_tryRetain {
    return YES;
}

- (BOOL) _tryRetain {
    return _objc_rootTryRetain((id)self);
}

- (Class)superclass {
    return class_getSuperclass(object_getClass(self));
}

+ (Class)superclass {
    return class_getSuperclass(self);
}

+ (id)copy {
    return (id)self;
}

+ (id)copyWithZone:(struct _NSZone *)zone {
    return (id)self;
}

- (id) copy {
    return [(id)self copyWithZone:nil];
}

- (BOOL)isEqual:(id)object {
    return object == self;
}

+ (BOOL)isEqual:(id)object {
    return object == (id)self;
}

- (BOOL)isFault {
    return NO;
}

+ (BOOL)isFault {
    return NO;
}

- (BOOL)isProxy {
    return NO;
}

+ (BOOL)isProxy {
    return NO;
}

- (BOOL) conformsToProtocol:(Protocol *)aProtocol {
    if (!aProtocol) return NO;
    for (Class tcls = [self class]; tcls; tcls = class_getSuperclass(object_getClass(tcls))) {
        if (class_conformsToProtocol(tcls, aProtocol)) return YES;
    }
    return NO;
}

+ (BOOL) conformsToProtocol:(Protocol *)aProtocol {
    if (!aProtocol) return NO;
    for (Class tcls = self; tcls; tcls = class_getSuperclass(tcls)) {
        if (class_conformsToProtocol(tcls, aProtocol)) return YES;
    }
    return NO;
}

- (BOOL)respondsToSelector:(SEL)aSelector {
    if (!aSelector) {
        return NO;
    }
    return class_respondsToSelector([self class], aSelector);
}

+ (BOOL)respondsToSelector:(SEL)aSelector {
    if (!aSelector) {
        return NO;
    }
    return class_respondsToSelector(object_getClass(self), aSelector);
}

- (BOOL) isMemberOfClass:(Class)aClass {
    return [self class] == aClass;
}

+ (BOOL) isMemberOfClass:(Class)aClass {
    return object_getClass((id)self) == aClass;
}

- (BOOL) isKindOfClass:(Class)aClass {
    for (Class tcls = [self class]; tcls; tcls = class_getSuperclass(object_getClass(tcls))) {
        if (tcls == aClass) {
            return YES;
        }
    }
    return NO;
}

+ (BOOL) isKindOfClass:(Class)aClass {
    for (Class tcls = object_getClass((id)self); tcls; tcls = class_getSuperclass(tcls)) {
        if (tcls == aClass) {
            return YES;
        }
    }
    return NO;
}

- (id) retain {
    return _objc_rootRetain((id)self);
}

+ (id) retain {
    return self;
}

- (NSUInteger) retainCount {
    return _objc_rootRetainCount((id)self);
}

- (oneway void) release {
    _objc_rootRelease(self);
}

- (id) autorelease {
    return _objc_rootAutorelease(self);
}

- (void) dealloc {
    _objc_rootDealloc(self);
}

- (id)init {
    return _objc_rootInit(self);
}

+ (id)init {
    return (id)self;
}

+ (void) release {
    
}

+ (id) autorelease {
    return self;
}

+ (void) dealloc {
    
}

@end
