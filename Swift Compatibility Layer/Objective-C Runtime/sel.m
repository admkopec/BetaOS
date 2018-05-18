//
//  sel.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 2/1/18.
//

#import <objc/objc.h>
#import <string.h>

const char * _Nonnull
sel_getName(SEL _Nonnull sel) {
    if (!sel) {
        return "<null selector>";
    }
    return (const char *)(const void*)sel;
}

BOOL
sel_isEqual(SEL lhs, SEL rhs) {
    return lhs == rhs;
}

static SEL
sel_alloc(const char *name, BOOL copy) {
    return (SEL)(copy ? strdup(name) : name);
}

SEL
sel_registerName(const char *name) {
    return sel_alloc(name, YES);
}
