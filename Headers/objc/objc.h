//
//  objc.h
//  Swift Compatibility Layer
//
//  Created by Adam Kopeć on 10/13/17.
//

#ifndef objc_h
#define objc_h
#undef KERNEL
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <objc/runtime.h>

struct objc_class {
    struct objc_class* isa;
    struct objc_class* superclass;
    const char* name;
    long version;
    unsigned long info;
    long instance_size;
};

struct objc_object {
    Class isa;
};

struct objc_selector {
    
};

#undef NSInteger
#undef NSUInteger
typedef          long NSInteger;
typedef unsigned long NSUInteger;

#define NSIntegerMax    LONG_MAX
#define NSIntegerMin    LONG_MIN
#define NSUIntegerMax   ULONG_MAX

#undef BOOL
#define OBJC_BOOL_IS_BOOL 1
typedef bool  BOOL;

#ifdef DEBUG
#define DBG(x...) kprintf(x)
#else
#define DBG(x...)
#endif

#ifdef ULTDEBUG
#define ULTDBG(x...) kprintf(x)
#else
#define ULTDBG(x...)
#endif

#if __has_feature(objc_bool)
#define YES   __objc_yes
#define NO    __objc_no
#else
#define YES   (BOOL)1
#define NO    (BOOL)0
#endif

/* 'nil' is the null object.  Messages to nil do nothing and always
 return 0.  */
#define nil (id)0

/* 'Nil' is the null class.  Since classes are objects too, this is
 actually the same object as 'nil' (and behaves in the same way),
 but it has a type of Class, so it is good to use it instead of
 'nil' if you are comparing a Class object to nil as it enables the
 compiler to do some type-checking.  */
#define Nil (Class)0

#ifndef __OBJC__

typedef struct objc_object Protocol;
#else /* __OBJC__ */
//@class Protocol
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#define UNIMPLEMENTED(x) void x() { ULTDBG(__func__); }

#ifdef __cplusplus
extern "C" {
#endif
    extern void kprintf(const char*, ...);
    extern void panic(const char*, ...);
    extern int  putchar(int);
    extern int  puts(const char*);
    extern int strcmp(const char*, const char*);
    extern void* malloc(size_t size);
    extern void free(void* ptr);
    extern size_t strlen(const char*);
    extern int strcmp(const char* str1, const char* str2);
    extern int strncmp(const char *s1, const char *s2, size_t n);
    extern size_t strlcpy(char * dst, const char * src, size_t maxlen);
    extern char * strncpy(char * dst, const char * src, size_t maxlen);
    extern char * strcpy(char *dest, const char *src);
    extern char * strcat(char *dest, const char *src);
    
    extern void* memcpy  (void* dstptr,   const void* srcptr, size_t size);
    extern void* memmove (void* dstptr,   const void* srcptr, size_t size);
    extern void* memset  (void* bufptr,   int         value,  size_t size);
    extern void* memchr  (const void* s1, int         c,      size_t size);
    extern int   memcmp  (const void* s1, const void* s2,     size_t size);
#ifdef __cplusplus
}
#endif

#endif /* objc_h */
