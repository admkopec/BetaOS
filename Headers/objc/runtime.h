//
//  runtime.h
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#ifndef runtime_h
#define runtime_h

/// An opaque type that represents an Objective-C class.
typedef struct objc_class *Class;

/// A pointer to an instance of a class.
typedef struct objc_object *id;

typedef struct objc_selector *SEL;
#ifdef __cplusplus
extern "C" {
#endif
    Class object_getClass(id object);
    Class objc_getClass(const char *aClassName);
    
#ifdef __cplusplus
}
#endif
#endif /* runtime_h */
