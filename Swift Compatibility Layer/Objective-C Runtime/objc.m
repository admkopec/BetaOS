//
//  objc.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 10/13/17.
//

#define UNIMPLEMENTED(x) void x() { ULTDBG(__func__); }

#ifdef ULTDEBUG
#define ULTDBG(x...) printf(x)
#else
#define ULTDBG(x...)
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

id _Nullable
objc_msgSend(id _Nullable self, SEL _Nonnull op, ...){
    ULTDBG("objc_msgSend!\n");
    return 0;
}

#import <objc/objc.h>
#import <objc/runtime.h>
#import <stdlib.h>
#import <string.h>
#import <stdio.h>
#import <objc/Protocol.h>
#undef strlcpy
#undef strcpy
#undef memcpy

int kCFNull;

// Temp values

// Default FILE * for stdio, faked values
// OSX symbol names
FILE *__stderrp = (void *)0xF2;
FILE *__stdinp  = (void *)0xF0;
FILE *__stdoutp = (void *)0xF1;

// Linux symbol names
//FILE *stderr = (void *)0xF2;
//FILE *stdin  = (void *)0xF0;
//FILE *stdout = (void *)0xF1;

extern void panic(const char*, ...);

UNIMPLEMENTED(CFErrorGetTypeID)
UNIMPLEMENTED(CFGetTypeID)
UNIMPLEMENTED(CFSetGetValues)
UNIMPLEMENTED(CFStringCompare)
UNIMPLEMENTED(CFStringCreateCopy)
UNIMPLEMENTED(CFStringCreateWithSubstring)
UNIMPLEMENTED(CFStringFindWithOptions)
UNIMPLEMENTED(CFStringGetCStringPtr)
UNIMPLEMENTED(CFStringGetCharacterAtIndex)
UNIMPLEMENTED(CFStringGetCharacters)
UNIMPLEMENTED(CFStringGetCharactersPtr)
UNIMPLEMENTED(CFStringGetLength)
UNIMPLEMENTED(NSClassFromString)
UNIMPLEMENTED(CFStringHashCString)
UNIMPLEMENTED(NSStringFromClass)
UNIMPLEMENTED(OBJC_CLASS_$_NSArray)
UNIMPLEMENTED(OBJC_CLASS_$_NSData)
UNIMPLEMENTED(OBJC_CLASS_$_NSIndexSet)
UNIMPLEMENTED(OBJC_CLASS_$_NSDictionary)
UNIMPLEMENTED(OBJC_CLASS_$_NSEnumerator)
UNIMPLEMENTED(OBJC_CLASS_$_NSError)
UNIMPLEMENTED(OBJC_CLASS_$_NSNumber)
UNIMPLEMENTED(OBJC_CLASS_$_NSProcessInfo)
UNIMPLEMENTED(OBJC_CLASS_$_NSSet)
UNIMPLEMENTED(OBJC_CLASS_$_NSString)
UNIMPLEMENTED(OBJC_METACLASS_$_NSArray)
UNIMPLEMENTED(OBJC_METACLASS_$_NSData)
UNIMPLEMENTED(OBJC_METACLASS_$_NSIndexSet)
UNIMPLEMENTED(OBJC_METACLASS_$_NSDictionary)
UNIMPLEMENTED(OBJC_METACLASS_$_NSEnumerator)
UNIMPLEMENTED(OBJC_METACLASS_$_NSError)
UNIMPLEMENTED(OBJC_METACLASS_$_NSSet)
UNIMPLEMENTED(OBJC_METACLASS_$_NSString)
UNIMPLEMENTED(__CFConstantStringClassReference)
UNIMPLEMENTED(_objc_empty_cache)

Ivar _Nonnull * _Nullable
class_copyIvarList(Class _Nullable cls, unsigned int * _Nullable outCount)  {
    return (Ivar *) 0;
}

id
class_createInstance(Class cls, size_t extraBytes) {
    return nil;
}

size_t
class_getInstanceSize(Class _Nullable cls) {
    return 0;
}

const char * _Nonnull
class_getName(Class _Nullable cls) {
    return "NSObject";
}

Class _Nullable
class_getSuperclass(Class _Nullable cls) {
    return Nil;
}

BOOL
class_isMetaClass(Class _Nullable cls) {
    return NO;
}

BOOL
class_respondsToSelector(Class _Nullable cls, SEL _Nonnull sel) {
    return NO;
}

ptrdiff_t
ivar_getOffset(Ivar _Nonnull v) {
    return (ptrdiff_t) 0;
}

//UNIMPLEMENTED(class_copyIvarList)
//UNIMPLEMENTED(class_createInstance)
//UNIMPLEMENTED(class_getInstanceSize)
//UNIMPLEMENTED(class_getName)
//UNIMPLEMENTED(class_getSuperclass)
//UNIMPLEMENTED(class_isMetaClass)
//UNIMPLEMENTED(class_respondsToSelector)
//UNIMPLEMENTED(ivar_getOffset)

UNIMPLEMENTED(objc_autorelease)
UNIMPLEMENTED(objc_autoreleaseReturnValue)
UNIMPLEMENTED(objc_copyWeak)
UNIMPLEMENTED(objc_debug_isa_class_mask)
UNIMPLEMENTED(objc_destroyWeak)

void * _Nullable
objc_destructInstance(id _Nullable obj) {
    return NULL;
}

//UNIMPLEMENTED(objc_destructInstance)

UNIMPLEMENTED(objc_initWeak)
UNIMPLEMENTED(objc_loadWeakRetained)
UNIMPLEMENTED(objc_moveWeak)
UNIMPLEMENTED(objc_msgSendSuper2)
UNIMPLEMENTED(objc_msgSend_stret)
UNIMPLEMENTED(objc_readClassPair)
UNIMPLEMENTED(objc_retainAutoreleasedReturnValue)

id _Nullable
objc_storeWeak(id _Nullable * _Nonnull location, id _Nullable obj) {
    return nil;
}

Class _Nullable
objc_lookUpClass(const char * _Nonnull name) {
    return Nil;
}

id _Nullable
objc_constructInstance(Class _Nullable cls, void * _Nullable bytes) {
    return nil;
}

id _Nullable
object_dispose(id _Nullable obj) {
    return nil;
}

Class _Nullable
object_setClass(id _Nullable obj, Class _Nonnull cls) {
    return Nil;
}

const char * _Nonnull
protocol_getName(Protocol * _Nonnull proto) {
    return "Protocol";
}

const char * _Nonnull
sel_getName(SEL _Nonnull sel) {
    return "SEL";
}

//UNIMPLEMENTED(objc_storeWeak)
//UNIMPLEMENTED(objc_lookUpClass)
//UNIMPLEMENTED(objc_constructInstance)
//UNIMPLEMENTED(object_dispose)
//UNIMPLEMENTED(object_setClass)
//UNIMPLEMENTED(protocol_getName)
//UNIMPLEMENTED(sel_getName)

void *
_Block_copy(const void *aBlock) {
    return NULL;
}

//UNIMPLEMENTED(_Block_copy)

UNIMPLEMENTED(_Block_release)
UNIMPLEMENTED(_NSGetArgc)
UNIMPLEMENTED(_NSGetArgv)

UNIMPLEMENTED(_ZTVNSt3__114__shared_countE)
UNIMPLEMENTED(_ZTVNSt3__119__shared_weak_countE)
UNIMPLEMENTED(_ZNSt3__119__shared_weak_count14__release_weakEv)
UNIMPLEMENTED(_ZNSt3__119__shared_weak_count16__release_sharedEv)
UNIMPLEMENTED(_ZNSt3__119__shared_weak_countD2Ev)
UNIMPLEMENTED(_ZNKSt3__119__shared_weak_count13__get_deleterERKSt9type_info)
UNIMPLEMENTED(_ZNKSt3__120__vector_base_commonILb1EE20__throw_length_errorEv)

UNIMPLEMENTED(_dyld_register_func_for_add_image)
UNIMPLEMENTED(dyld_stub_binder)

void
flockfile(FILE *stream) {
    if (stream != stderr && stream != stdout) {
        panic("flockfile stream = %p", stream);
    }
    
}

void
funlockfile(FILE *stream) {
    if (stream != stderr && stream != stdout) {
        panic("funlockfile stream = %p", stream);
    }
}

extern ssize_t write(int fd, const void *buf, size_t nbyte);

size_t
fwrite(const void* buf, size_t size, size_t count, FILE *stream) {
    flockfile(stream);
    write(1, buf, size);
    funlockfile(stream);
    return count;
}

BOOL class_conformsToProtocol(Class cls, Protocol *protocol) {
    return YES;
}

UNIMPLEMENTED(__divti3)
UNIMPLEMENTED(__udivti3)
UNIMPLEMENTED(__modti3)
UNIMPLEMENTED(__umodti3)

UNIMPLEMENTED(backtrace)

UNIMPLEMENTED(__cxa_demangle)
UNIMPLEMENTED(__gxx_personality_v0)
UNIMPLEMENTED(_Unwind_Resume)

UNIMPLEMENTED(_ZNSt3__113random_deviceC1ERKNS_12basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE)
UNIMPLEMENTED(_ZNSt3__113random_deviceD1Ev)
UNIMPLEMENTED(_ZNSt3__113random_deviceclEv)
UNIMPLEMENTED(_ZNSt3__16thread20hardware_concurrencyEv)

int
__cxa_guard_abord(void *guard) {
    ULTDBG("__cxa_guard_abort(%p\n", guard);
    return 0;
}

int
__cxa_guard_acquire(void *guard) {
    ULTDBG("__cxa_guard_acquire(%p)\n", guard);
    return 0;
}

void
__cxa_guard_release(void *guard) {
    ULTDBG("__cxa_guard_release(%p)\n", guard);
}


int
__cxa_atexit(void (*func) (void *), void *arg, void *dso_handle) {
    ULTDBG("__cxa_atexit(%p, %p, %p)\n", func, arg, dso_handle);
    // Exit never occurs so ignore this handler, return success
    return 0;
}

/*
 * mutex
 */



// std::__1::mutex::lock()
void _ZNSt3__15mutex4lockEv(void *this) {
    ULTDBG("(_ZNSt3__15mutex4lockEv)mutex_lock this=%p\n", this);
}

void _ZNSt3__15mutex6unlockEv(void *this) {
    ULTDBG("(_ZNSt3__15mutex6unlockEv)mutex_unlock this=%p\n", this);
}

void _ZNSt3__111__call_onceERVmPvPFvS2_E() {
    panic("Calling _ZNSt3__111__call_onceERVmPvPFvS2_E\n");
}


/*
 * hash
 */

size_t
_ZNSt3__112__next_primeEm(size_t n) {
    size_t i, j, count;
    for (i = n + 1; 1; i++) {
        count = 0;
        for (j = 2;j < i; j++) {
            if (i%j == 0) { // found a divisor
                count++;
                break;  // break for (j = 2,j < i; j++) loop
            }
        }
        if (count == 0) {
            return i;
        }
    }
}


void _ZNSt3__16__sortIRNS_6__lessImmEEPmEEvT0_S5_T_(unsigned long *start, unsigned long *end, void *cmpfunc) {
    ULTDBG("Calling _ZNSt3__16__sortIRNS_6__lessImmEEPmEEvT0_S5_T_(%p, %p, %p)\n", start, end, cmpfunc);
    if (start == end) {
        return; // no sort needed
    } else {
        // FIXME
        __asm__("hlt");
    }
}


/*
 * basic_string
 *
 */


// 24bytes
const size_t SMALL_STR_CAPACITY = 23;
static const size_t MAX_STRING_SIZE = SIZE_MAX - 3;  // allow lowest bit to be used for flag

struct short_string {
    uint8_t len;
    char string[SMALL_STR_CAPACITY];
};

struct long_string {
    size_t capacity;        // size of malloc'd 'data'
    size_t curlen;
    char *data;
};


struct basic_string {
    // bit 0 of len/capacity = 0 is an inline_string 1 if an allocated_string
    union {
        struct short_string ss;
        struct long_string ls;
    };
};


static inline int
is_long_string(struct basic_string *this) {
    return this->ss.len & 1;
}


/* string capacity */

static inline size_t
best_str_capacity(size_t len) {
    return (len + 16) & ~0xf;
}


static inline size_t
get_long_str_capacity(struct basic_string *this) {
    return this->ls.capacity;
}


static inline size_t
get_short_str_capacity(struct basic_string *this) {
    return SMALL_STR_CAPACITY;
}


static inline size_t
get_str_capacity(struct basic_string *this) {
    size_t capacity = is_long_string(this) ? get_long_str_capacity(this) : get_short_str_capacity(this);
    return capacity - 1;
}


static inline void
set_long_str_capacity(struct basic_string *this, size_t capacity) {
    this->ls.capacity = capacity | 1;
}


static inline size_t
get_str_size(struct basic_string *this) {
    return is_long_string(this) ? this->ls.curlen : this->ss.len >> 1;
}


static inline void
set_short_str_size(struct basic_string *this, size_t len) {
    this->ss.len = len << 1;
}


static inline void
set_long_str_size(struct basic_string *this, size_t len) {
    this->ls.curlen = len;
}


static inline void
set_str_size(struct basic_string *this, size_t size) {
    if (is_long_string(this)) {
        set_long_str_size(this, size);
    } else {
        set_short_str_size(this, size);
    }
}


static inline char *
get_short_str_ptr(struct basic_string *this) {
    return this->ss.string;
}


static inline char *
get_long_str_ptr(struct basic_string *this) {
    return this->ls.data;
}


static inline char *
get_str_ptr(struct basic_string *this) {
    return is_long_string(this) ? get_long_str_ptr(this) : get_short_str_ptr(this);
}


static inline void
set_long_str_ptr(struct basic_string *this, char *p) {
    this->ls.data = p;
}


void
_ZNKSt3__121__basic_string_commonILb1EE20__throw_length_errorEv(struct basic_string *this) {
    panic("string too long!");
    __builtin_unreachable ();
}


int
_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7compareEPKc(struct basic_string *this, const char *str) {
    int result = strcmp(get_str_ptr(this), str);
    return result;
}

// std::__1::basic_string<char, std::__1::char_traits<char>,
void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6__initEPKcm(struct basic_string *this, char const *str, size_t len) {
    char *dest;
    
    if (unlikely(len == (uint32_t)~0)) {
        _ZNKSt3__121__basic_string_commonILb1EE20__throw_length_errorEv(this);
        __builtin_unreachable ();
    }
    
    
    else if (likely(len <= SMALL_STR_CAPACITY)) {
        dest = get_short_str_ptr(this);
        set_short_str_size(this, len);
    } else {
        size_t capacity = best_str_capacity(len);
        dest = malloc(capacity+1);
        set_long_str_capacity(this, capacity+1);
        set_long_str_ptr(this, dest);
        set_long_str_size(this, len);
    }
    memcpy(dest, str, len);
    dest[len+1] = '\0';
}


// std::__1::allocator<char> >::__init(char const*, unsigned long)
void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6__initEmc(struct basic_string *this, size_t len, char ch) {
    if (len > MAX_STRING_SIZE) {
        _ZNKSt3__121__basic_string_commonILb1EE20__throw_length_errorEv(this);
    }
    
    char *dest;
    if (len < SMALL_STR_CAPACITY) {
        set_short_str_size(this, len);
        dest = get_short_str_ptr(this);
    } else {
        size_t capacity = best_str_capacity(len);
        dest = malloc(capacity+1);
        set_long_str_ptr(this, dest);
        set_long_str_capacity(this, capacity+1);
        set_long_str_size(this, len);
    }
}


void
__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE21__grow_by_and_replaceEmmmmmmPKc(struct basic_string *this,
                                                                                                  size_t old_capacity,
                                                                                                  size_t extra_capacity,
                                                                                                  size_t cursize,
                                                                                                  size_t bytes_to_copy,
                                                                                                  size_t bytes_to_del,
                                                                                                  size_t bytes_to_add,
                                                                                                  const char *string) {
    if (old_capacity + extra_capacity > MAX_STRING_SIZE) {
        _ZNKSt3__121__basic_string_commonILb1EE20__throw_length_errorEv(this);
    }
    
    size_t new_capacity = old_capacity + extra_capacity;
    char *old_str = get_str_ptr(this);
    char *p = malloc(new_capacity + 1);
    if (bytes_to_copy != 0) {
        memcpy(p, old_str, bytes_to_copy);
        p[bytes_to_copy] = '\0';
    }
    if (bytes_to_add != 0) {
        memcpy(p + bytes_to_copy, string, bytes_to_add);
        p[bytes_to_copy + bytes_to_add] = '\0';
    }
    size_t sec_cp_sz = cursize - bytes_to_del - bytes_to_copy;
    if (sec_cp_sz != 0) {
        memcpy(p + bytes_to_copy + bytes_to_add, old_str + bytes_to_copy + bytes_to_del, sec_cp_sz);
        p[bytes_to_copy + bytes_to_add + sec_cp_sz] = '\0';
    }
    if (is_long_string(this)) {
        free(old_str);
    }
    set_long_str_ptr(this, p);
    set_long_str_capacity(this, new_capacity + 1);
    set_long_str_size(this, bytes_to_copy + bytes_to_add + sec_cp_sz);
}


struct basic_string *
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKcm(struct basic_string *this,
                                                                            const char *string,
                                                                            size_t len) {
    size_t size = get_str_size(this);
    size_t capacity = get_str_capacity(this);
    if (capacity - size >= len) {
        if (len) {
            char *p = get_str_ptr(this);
            memcpy(p + size, string, len);
            p[size + len] = '\0';
            set_str_size(this, size + len);
        }
    } else {
        __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE21__grow_by_and_replaceEmmmmmmPKc(this, capacity, size + len - capacity,
                                                                                                          size, size, 0, len, string);
    }
    return this;
}


struct basic_string *
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKc(struct basic_string *this,
                                                                           const char *string) {
    _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKcm(this, string, strlen(string));
    return this;
}


struct basic_string *
_ZNSsC1EPKcmRKSaIcE(struct basic_string *this, const char *string, size_t len, void *allocator) {
    return _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKcm(this, string, len);
}


void
__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm(struct basic_string *this,
                                                                                  size_t old_capacity,
                                                                                  size_t extra_capacity,
                                                                                  size_t cursize,
                                                                                  size_t bytes_to_copy,
                                                                                  size_t bytes_to_del,
                                                                                  size_t bytes_to_add) {
    if (old_capacity + extra_capacity > MAX_STRING_SIZE) {
        _ZNKSt3__121__basic_string_commonILb1EE20__throw_length_errorEv(this);
    }
    
    size_t new_capacity = best_str_capacity(old_capacity + extra_capacity);
    char *p = malloc(new_capacity + 1);
    char *old_str = get_str_ptr(this);
    if (bytes_to_copy != 0) {
        memcpy(p, old_str, bytes_to_copy);
        p[bytes_to_copy] = '\0';
    }
    size_t sec_cp_sz = cursize - bytes_to_del - bytes_to_copy;
    if (sec_cp_sz !=  0) {
        memcpy(p + bytes_to_copy + bytes_to_add, old_str + bytes_to_copy + bytes_to_del, sec_cp_sz);
        p[bytes_to_copy + bytes_to_add + sec_cp_sz] = '\0';
    }
    if (is_long_string(this)) {
        free(old_str);
    }
    set_long_str_ptr(this, p);
    set_long_str_capacity(this, new_capacity + 1);
}


struct basic_string *
__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEmc(struct basic_string *this,
                                                                           unsigned long newlen, char ch) {
    if (newlen) {
        size_t capacity = get_str_capacity(this);
        size_t size = get_str_size(this);
        if (capacity - size < newlen) {
            __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm(this, capacity, size + newlen - capacity,
                                                                                              size, size, 0, 0);
            //char *p = get_str_ptr(this);
            set_str_size(this, size + newlen);
        }
    }
    return this;
}


void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6resizeEmc(struct basic_string *this, unsigned long newlen, char ch) {
    size_t size = get_str_size(this);
    if (size < newlen) {
        char *p = get_str_ptr(this);
        p[newlen+1] = '\0';
    } else {
        __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEmc(this, newlen - size, ch);
    }
}


void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7reserveEm(struct basic_string *this, size_t new_cap) {
    ULTDBG("Calling _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7reserveEm\nThis function doesn't work\n");
    if (new_cap != get_str_capacity(this)) {
        if (new_cap < get_str_size(this)) {
            new_cap = get_str_size(this);
        }
        __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm(this, get_str_capacity(this), new_cap - get_str_size(this), get_str_size(this), get_str_size(this), 0, new_cap - get_str_size(this));
    }
}


void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9push_backEc(struct basic_string *this, char ch) {
    size_t capacity = get_str_capacity(this);
    size_t size = get_str_size(this);
    if (size == capacity) {
        __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm(this, capacity, 1, size, size, 0, 0);
    }
    set_str_size(this, size + 1);
    char *p = get_str_ptr(this);
    p[size] = ch;
    p[size + 1] = '\0';
}


void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_() {
    panic("Calling _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_\n");
}


void
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev(struct basic_string *this) {
    if (likely(is_long_string(this))) {
        free(get_str_ptr(this));
    }
}

UNIMPLEMENTED(_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKc)
UNIMPLEMENTED(_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6insertEmPKc)

struct basic_string *
__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKcm(struct basic_string *this, const char *string, size_t len) {
    size_t capacity = get_str_capacity(this);
    if (capacity >= len) {
        char *p = get_str_ptr(this);
        memcpy(p, string, len);
        p[len] = '\0';
        set_str_size(this, len);
    } else {
        size_t size = get_str_size(this);
        __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE21__grow_by_and_replaceEmmmmmmPKc(this, capacity, len - capacity, size,
                                                                                                          0, size, len, string);
    }
    return this;
}


// std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::operator=(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&)
struct basic_string *
_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEaSERKS5_(struct basic_string *this, struct basic_string *that) {
    if (this != that) {
        __ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKcm(this, get_str_ptr(that), get_str_size(that));
    }
    return this;
}


// std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string const&)
void
_ZNSsC1ERKSs(struct basic_string *this, struct basic_string *that) {
    if (is_long_string(that)) {
        _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6__initEPKcm(this, get_long_str_ptr(that), get_str_size(that));
    } else {
        this->ss = that->ss;
    }
}
