//
//  NSObject.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import <objc/NSObject.h>
#import <objc/objc.h>
#import <objc/runtime.h>

id _objc_rootRetain(id a) {
    return nil;
}

void _objc_rootRelease(id a) {
    
}

id _objc_rootAutorelease(id a) {
    return nil;
}

@implementation NSObject

+ (void) initialize {
//    return self;
}

- (id) class {
    return self;
}

- (id) self {
    return self;
}

- (id) copy {
    return nil;
}

- (BOOL) isEqual:(id)object {
    return NO;
}

- (BOOL) conformsToProtocol:(Protocol *)aProtocol {
    if (!aProtocol) return NO;
    for (Class tcls = [self class]; tcls; tcls = [[tcls superclass] class]) {
        if (class_conformsToProtocol(tcls, aProtocol)) return YES;
    }
    return NO;
}

+ (BOOL) conformsToProtocol:(Protocol *)aProtocol {
    if (!aProtocol) return NO;
    for (Class tcls = self; tcls; tcls = [tcls superclass]) {
        if (class_conformsToProtocol(tcls, aProtocol)) return YES;
    }
    return NO;
}

- (BOOL) isKindOfClass:(Class)aClass {
    return NO;
}

+ (BOOL) isKindOfClass:(Class)aClass {
    return NO;
}

- (id) retain {
    return _objc_rootRetain(self);
}

- (NSUInteger) retainCount {
    return 0;
}

- (oneway void) release {
    _objc_rootRelease(self);
}

- (id) autorelease {
    return _objc_rootAutorelease(self);
}

- (void) dealloc {
    
}

+ (id) copy {
    return self;
}

+ (id) retain {
    return self;
}

+ (void) release {
    
}

+ (id) autorelease {
    return self;
}

+ (void) dealloc {
    
}

@end
