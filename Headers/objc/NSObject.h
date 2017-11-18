//
//  NSObject.h
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#import "objc.h"

@class NSString, NSMethodSignature, NSInvocation;

@protocol NSObject
- (BOOL)isEqual:(id)object;
@property (readonly) NSUInteger hash;

@property (readonly) Class superclass;
- (Class)class;
- (instancetype)self;

//- (id)performSelector:(SEL)aSelector;
//- (id)performSelector:(SEL)aSelector withObject:(id)object;
//- (id)performSelector:(SEL)aSelector withObject:(id)object1 withObject:(id)object2;
//
//- (BOOL)isProxy;
//
- (BOOL)isKindOfClass:(Class)aClass;
//- (BOOL)isMemberOfClass:(Class)aClass;
- (BOOL)conformsToProtocol:(Protocol *)aProtocol;
//
//- (BOOL)respondsToSelector:(SEL)aSelector;

- (instancetype)retain;
- (oneway void)release;
- (instancetype)autorelease;
- (NSUInteger)retainCount;

//- (struct _NSZone *)zone;

@property (readonly, copy) NSString *description;
@optional
@property (readonly, copy) NSString *debugDescription;

@end

@interface NSObject <NSObject>
@property (readonly) NSUInteger hash;
@property (readonly) Class superclass;
@property (readonly, copy) NSString *description;
-retain;
-copy;
-(void)release;
-autorelease;
-(void)dealloc;
@end
