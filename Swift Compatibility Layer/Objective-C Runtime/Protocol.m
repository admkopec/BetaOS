//
//  Protocol.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import <objc/NSObject.h>
#import <objc/objc.h>
#import <objc/runtime.h>
#import <objc/Protocol.h>

@interface __IncompleteProtocol : NSObject
@end

@implementation __IncompleteProtocol
+ (void) load { }
@end

//@implementation Protocol
//+ (void) load { }
//
//- (BOOL) conformsTo:(Protocol *)aProtocol {
//    return NO;
//}
//
//- (BOOL) isEqual:other {
//    Class cls;
//    Class protoClass = objc_getClass("Protocol");
//    for (cls = object_getClass(other); cls; cls = cls->isa) {
//        if (cls == protoClass) {
//            break;
//        }
//    }
//    if (!cls) {
//        return NO;
//    }
//    //    return protocol_isEqual(self, other);
//    return NO;
//}
//@end

