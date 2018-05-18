//
//  NSObject.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import <objc/NSObject.h>
#import <objc/objc.h>
#import <objc/runtime.h>
#import <malloc/malloc.h>
#import <assert.h>

extern id _objc_rootRetain(id a);
extern void _objc_rootRelease(id a);
extern id _objc_rootAutorelease(id a);
extern id _objc_rootInit(id a);
extern id _objc_rootAllocWithZone(Class cls, malloc_zone_t *zone);
extern void _objc_rootDealloc(id a);
extern BOOL _objc_rootTryRetain(id a);
extern malloc_zone_t * _objc_rootZone(id a);
extern uintptr_t _objc_rootHash(id a);
extern NSUInteger _objc_rootRetainCount(id a);

@implementation NSObject

+ (void) initialize { }

+ (id) new {
    return [[self alloc] init];
}

+ (Class) class {
    return self;
}

- (Class) class {
    return object_getClass(self);
}

- (id) self {
    return self;
}

+ (id) self {
    return (id)self;
}

- (Class)superclass {
    return class_getSuperclass([self class]);
}

+ (Class)superclass {
    return class_getSuperclass(self);
}

- (NSUInteger) hash {
    return _objc_rootHash(self);
}

+ (NSUInteger) hash {
    return _objc_rootHash(self);
}

- (BOOL) isEqual:(id)object {
    return object == self;
}

+ (BOOL) isEqual:(id)object {
    return object == (id)self;
}

- (BOOL) isFault {
    return NO;
}

+ (BOOL) isFault {
    return NO;
}

- (BOOL) isProxy {
    return NO;
}

+ (BOOL) isProxy {
    return NO;
}

+ (BOOL) isBlock {
    return NO;
}

- (BOOL) isBlock {
    return NO;
}

- (BOOL) isMemberOfClass:(Class)aClass {
    return [self class] == aClass;
}

+ (BOOL) isMemberOfClass:(Class)aClass {
    return object_getClass((id)self) == aClass;
}

- (BOOL) isKindOfClass:(Class)aClass {
    for (Class tcls = [self class]; tcls; tcls = class_getSuperclass(tcls)) {
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

+ (BOOL) isSubclassOfClass:(Class)aClass {
    for (Class tcls = self; tcls; tcls = class_getSuperclass(tcls)) {
        if (tcls == aClass) {
            return YES;
        }
    }
    return NO;
}

+ (BOOL) instancesRespondToSelector:(SEL)aSelector {
    if (!aSelector) {
        return NO;
    }
    return class_respondsToSelector(self, aSelector);
}

- (BOOL) respondsToSelector:(SEL)aSelector {
    if (!aSelector) {
        return NO;
    }
    return class_respondsToSelector([self class], aSelector);
}

+ (BOOL) respondsToSelector:(SEL)aSelector {
    if (!aSelector) {
        return NO;
    }
    return class_respondsToSelector(object_getClass(self), aSelector);
}

- (BOOL) conformsToProtocol:(Protocol *)aProtocol {
    if (!aProtocol) return NO;
    for (Class tcls = [self class]; tcls; tcls = class_getSuperclass(tcls)) {
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

- (id) retain {
    return _objc_rootRetain((id)self);
}

+ (id) retain {
    return self;
}

- (BOOL) _tryRetain {
    return _objc_rootTryRetain((id)self);
}

+ (BOOL) _tryRetain {
    return YES;
}

+ (NSUInteger) retainCount {
    return __LONG_MAX__;
}

- (NSUInteger) retainCount {
    return _objc_rootRetainCount(self);
}

+ (id)copyWithZone:(struct _NSZone *)zone {
    return (id)self;
}

- (id) copy {
    return [(id)self copyWithZone: nil];
}

+ (id)copy {
    return (id)self;
}

+ (id) mutableCopyWithZone:(struct _NSZone *)zone {
    return (id)self;
}

- (id) mutableCopy {
    return [(id)self mutableCopyWithZone: nil];
}

+ (id) mutableCopy {
    return (id)self;
}

+ (id) alloc {
    return [self allocWithZone: nil];
}

+ (id) allocWithZone:(struct _NSZone *)zone {
    return _objc_rootAllocWithZone(self, (malloc_zone_t *)zone);
}

- (struct _NSZone *)zone {
    return (struct _NSZone *)_objc_rootZone(self);
}

+ (struct _NSZone *)zone {
    return (struct _NSZone *)_objc_rootZone(self);
}

- (oneway void) release {
    _objc_rootRelease(self);
    [self dealloc];
}

+ (oneway void) release { }

- (id) autorelease {
    return _objc_rootAutorelease(self);
}

- (void) dealloc {
    _objc_rootDealloc(self);
}

+ (void) dealloc { }

- (id)init {
    return _objc_rootInit(self);
}

+ (id)init {
    return (id)self;
}

+ (id) autorelease {
    return self;
}

@end
