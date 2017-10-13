//
//  NSObject.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import "NSObject.h"

id _objc_rootRetain(id a) {
    return nil;
}

void _objc_rootRelease(id a) {
    
}

id _objc_rootAutorelease(id a) {
    return nil;
}

@implementation NSObject

+ (id) initialize {
    return self;
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

- (id) retain {
    return _objc_rootRetain(self);
}

- (NSUInteger) retainCount {
    return 0;
}

- (void) release {
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
