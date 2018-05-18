//
//  objc-private.h
//  Swift Compatibility Layer
//
//  Created by Adam Kopeć on 11/19/17.
//

#ifndef objc_private_h
#define objc_private_h

#ifdef _OBJC_OBJC_H_
#error include objc-private.h before other headers
#endif

#define OBJC_TYPES_DEFINED 1
#define OBJC_OLD_DISPATCH_PROTOTYPES 1

#include <stdint.h>
#include <assert.h>

struct objc_class;
struct objc_object;

typedef struct objc_class *Class;
typedef struct objc_object *id;

namespace {
    struct SideTable;
};


union isa_t {
    isa_t() { }
    isa_t(uintptr_t value) : bits(value) { }
    
    Class cls;
    uintptr_t bits;
    
    // extra_rc must be the MSB-most field (so it matches carry/overflow flags)
    // indexed must be the LSB (fixme or get rid of it)
    // shiftcls must occupy the same bits that a real class pointer would
    // bits + RC_ONE is equivalent to extra_rc + 1
    // RC_HALF is the high bit of extra_rc (i.e. half of its range)
    
    // future expansion:
    // uintptr_t fast_rr : 1;     // no r/r overrides
    // uintptr_t lock : 2;        // lock for atomic property, @synch
    // uintptr_t extraBytes : 1;  // allocated with extra bytes
    
//#   define ISA_MASK        0x00007ffffffffff8ULL
#   define ISA_MASK        0xfffffffffffffff8ULL
#   define ISA_MAGIC_MASK  0x001f800000000001ULL
#   define ISA_MAGIC_VALUE 0x001d800000000001ULL
    struct {
        uintptr_t indexed           : 1;
        uintptr_t has_assoc         : 1;
        uintptr_t has_cxx_dtor      : 1;
        uintptr_t shiftcls          : 44; // MACH_VM_MAX_ADDRESS 0x7fffffe00000
        uintptr_t magic             : 6;
        uintptr_t weakly_referenced : 1;
        uintptr_t deallocating      : 1;
        uintptr_t has_sidetable_rc  : 1;
        uintptr_t extra_rc          : 8;
#       define RC_ONE   (1ULL<<56)
#       define RC_HALF  (1ULL<<7)
    };
};


struct objc_object {
private:
    isa_t isa;
    
public:
    
    // ISA() assumes this is NOT a tagged pointer object
    Class ISA();
    
    // getIsa() allows this to be a tagged pointer object
    Class getIsa();
    
    // initIsa() should be used to init the isa of new objects only.
    // If this object already has an isa, use changeIsa() for correctness.
    // initInstanceIsa(): objects with no custom RR/AWZ
    // initClassIsa(): class objects
    // initProtocolIsa(): protocol objects
    // initIsa(): other objects
    void initIsa(Class cls /*indexed=false*/);
    void initClassIsa(Class cls /*indexed=maybe*/);
    void initProtocolIsa(Class cls /*indexed=maybe*/);
    void initInstanceIsa(Class cls, bool hasCxxDtor);
    
    // changeIsa() should be used to change the isa of existing objects.
    // If this is a new object, use initIsa() for performance.
    Class changeIsa(Class newCls);
    
    bool hasIndexedIsa();
    bool isTaggedPointer();
    bool isClass();
    
    // object may have associated objects?
    bool hasAssociatedObjects();
    void setHasAssociatedObjects();
    
    // object may be weakly referenced?
    bool isWeaklyReferenced();
    void setWeaklyReferenced_nolock();
    
    // object may have -.cxx_destruct implementation?
    bool hasCxxDtor();
    
    // Optimized calls to retain/release methods
    id retain();
    void release();
    id autorelease();
    
    // Implementations of retain/release methods
    id rootRetain();
    bool rootRelease();
    id rootAutorelease();
    bool rootTryRetain();
    bool rootReleaseShouldDealloc();
    uintptr_t rootRetainCount();
    
    // Implementation of dealloc methods
    bool rootIsDeallocating();
    void clearDeallocating();
    void rootDealloc();
    
private:
    void initIsa(Class newCls, bool indexed, bool hasCxxDtor);
    
    // Slow paths for inline control
    id rootAutorelease2();
    bool overrelease_error();
    
    // Unified retain count manipulation for nonpointer isa
    id rootRetain(bool tryRetain, bool handleOverflow);
    bool rootRelease(bool performDealloc, bool handleUnderflow);
    id rootRetain_overflow(bool tryRetain);
    bool rootRelease_underflow(bool performDealloc);
    
    void clearDeallocating_slow();
    
    // Side table retain count overflow for nonpointer isa
    void sidetable_lock();
    void sidetable_unlock();
    
    void sidetable_moveExtraRC_nolock(size_t extra_rc, bool isDeallocating, bool weaklyReferenced);
    bool sidetable_addExtraRC_nolock(size_t delta_rc);
    size_t sidetable_subExtraRC_nolock(size_t delta_rc);
    size_t sidetable_getExtraRC_nolock();
    
    // Side-table-only retain count
    bool sidetable_isDeallocating();
    void sidetable_clearDeallocating();
    
    bool sidetable_isWeaklyReferenced();
    void sidetable_setWeaklyReferenced_nolock();
    
    id sidetable_retain();
    id sidetable_retain_slow(SideTable& table);
    
    uintptr_t sidetable_release(bool performDealloc = true);
    uintptr_t sidetable_release_slow(SideTable& table, bool performDealloc = true);
    
    bool sidetable_tryRetain();
    
    uintptr_t sidetable_retainCount();
#if DEBUG
    bool sidetable_present();
#endif
};


#endif /* objc_private_h */
