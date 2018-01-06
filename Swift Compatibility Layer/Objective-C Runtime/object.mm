//
//  object.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 11/19/17.
//

#import "objc-private.h"
#import "runtime-private.h"
#import <objc/objc.h>
#import <stdio.h>
#import <ctype.h>

extern "C" {
    extern id _Nullable
    objc_msgSend(id _Nullable self, SEL _Nonnull op, ...);
    extern id
    _objc_constructOrFree(id bytes, Class cls);
    extern void *objc_destructInstance(id obj);
    extern id object_dispose(id obj);
}

extern bool DisableNonpointerIsa;


#define fastpath(x) (__builtin_expect(bool(x), 1))
#define slowpath(x) (__builtin_expect(bool(x), 0))

#define TAG_COUNT 8
#define TAG_SLOT_MASK 0xf

//#if SUPPORT_MSB_TAGGED_POINTERS
//#   define TAG_MASK (1ULL<<63)
//#   define TAG_SLOT_SHIFT 60
//#   define TAG_PAYLOAD_LSHIFT 4
//#   define TAG_PAYLOAD_RSHIFT 4
//#else
#   define TAG_MASK 1
#   define TAG_SLOT_SHIFT 0
#   define TAG_PAYLOAD_LSHIFT 0
#   define TAG_PAYLOAD_RSHIFT 4
//#endif

// Selectors
SEL SEL_load = NULL;
SEL SEL_initialize = NULL;
SEL SEL_resolveInstanceMethod = NULL;
SEL SEL_resolveClassMethod = NULL;
SEL SEL_cxx_construct = NULL;
SEL SEL_cxx_destruct = NULL;
SEL SEL_retain = NULL;
SEL SEL_release = NULL;
SEL SEL_autorelease = NULL;
SEL SEL_retainCount = NULL;
SEL SEL_alloc = NULL;
SEL SEL_allocWithZone = NULL;
SEL SEL_dealloc = NULL;
SEL SEL_copy = NULL;
SEL SEL_new = NULL;
SEL SEL_finalize = NULL;
SEL SEL_forwardInvocation = NULL;
SEL SEL_tryRetain = NULL;
SEL SEL_isDeallocating = NULL;
SEL SEL_retainWeakReference = NULL;
SEL SEL_allowsWeakReference = NULL;

static inline uintptr_t
addc(uintptr_t lhs, uintptr_t rhs, uintptr_t carryin, uintptr_t *carryout) {
    return __builtin_addcl(lhs, rhs, carryin, (unsigned long *)carryout);
}

static inline uintptr_t
subc(uintptr_t lhs, uintptr_t rhs, uintptr_t carryin, uintptr_t *carryout) {
    return __builtin_subcl(lhs, rhs, carryin, (unsigned long *)carryout);
}

static inline
uintptr_t
LoadExclusive(uintptr_t *src) {
    return *src;
}

static inline
bool
StoreExclusive(uintptr_t *dst, uintptr_t oldvalue, uintptr_t value) {
    return __sync_bool_compare_and_swap((void **)dst, (void *)oldvalue, (void *)value);
}

static inline
bool
StoreReleaseExclusive(uintptr_t *dst, uintptr_t oldvalue, uintptr_t value) {
    return StoreExclusive(dst, oldvalue, value);
}

inline bool
objc_object::isClass() {
    if (isTaggedPointer()) return false;
    return ISA()->isMetaClass();
}

inline bool
objc_object::isTaggedPointer() {
    return ((uintptr_t)this & TAG_MASK);
}

inline Class
objc_object::ISA() {
    assert(!isTaggedPointer());
//    return (Class)(isa.bits & ISA_MASK);
    return (Class)(isa.bits);
}

inline void
objc_object::initIsa(Class cls) {
    initIsa(cls, false, false);
}

inline void
objc_object::initClassIsa(Class cls) {
    if (DisableNonpointerIsa  ||  cls->requiresRawIsa()) {
        initIsa(cls, false/*not nonpointer*/, false);
    } else {
        initIsa(cls, true/*nonpointer*/, false);
    }
}

inline void
objc_object::initProtocolIsa(Class cls) {
    return initClassIsa(cls);
}

inline void
objc_object::initInstanceIsa(Class cls, bool hasCxxDtor) {
    assert(!cls->requiresRawIsa());
    assert(hasCxxDtor == cls->hasCxxDtor());
    
    initIsa(cls, true, hasCxxDtor);
}

inline void
objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor) {
    assert(!isTaggedPointer());
    
    if (!nonpointer) {
        isa.cls = cls;
    } else {
        assert(!DisableNonpointerIsa);
        assert(!cls->requiresRawIsa());
        
        isa_t newisa(0);
        
//#if SUPPORT_INDEXED_ISA
//        assert(cls->classArrayIndex() > 0);
//        newisa.bits = ISA_INDEX_MAGIC_VALUE;
//        // isa.magic is part of ISA_MAGIC_VALUE
//        // isa.nonpointer is part of ISA_MAGIC_VALUE
//        newisa.has_cxx_dtor = hasCxxDtor;
//        newisa.indexcls = (uintptr_t)cls->classArrayIndex();
//#else
        newisa.bits = ISA_MAGIC_VALUE;
        // isa.magic is part of ISA_MAGIC_VALUE
        // isa.nonpointer is part of ISA_MAGIC_VALUE
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.shiftcls = (uintptr_t)cls >> 3;
//#endif
        
        // This write must be performed in a single store in some cases
        // (for example when realizing a class because other threads
        // may simultaneously try to use the class).
        // fixme use atomics here to guarantee single-store and to
        // guarantee memory order w.r.t. the class index table
        // ...but not too atomic because we don't want to hurt instantiation
        isa = newisa;
    }
}

inline void
objc_object::rootDealloc() {
    if (isTaggedPointer()) return;  // fixme necessary?
    
    if (fastpath(isa.indexed  &&
                 !isa.weakly_referenced  &&
                 !isa.has_assoc  &&
                 !isa.has_cxx_dtor  &&
                 !isa.has_sidetable_rc)) {
//        assert(!sidetable_present());
        free(this);
    }
    else {
        object_dispose((id)this);
    }
}

// Equivalent to calling [this retain], with shortcuts if there is no override
inline id
objc_object::retain() {
    // UseGC is allowed here, but requires hasCustomRR.
//    assert(!UseGC  || ISA()->hasCustomRR());
    assert(!isTaggedPointer());
    
    if (!ISA()->hasCustomRR()) {
        return rootRetain();
    }
    
    return ((id(*)(objc_object *, SEL))objc_msgSend)(this, SEL_retain);
}

inline void
objc_object::release() {
    // UseGC is allowed here, but requires hasCustomRR.
    //    assert(!UseGC  ||  ISA()->hasCustomRR());
    assert(!isTaggedPointer());
    
    if (! ISA()->hasCustomRR()) {
        rootRelease();
        return;
    }
    
    ((void(*)(objc_object *, SEL))objc_msgSend)(this, SEL_release);
}

inline id
objc_object::autorelease() {
    if (isTaggedPointer()) {
        return (id)this;
    }
    if (fastpath(!ISA()->hasCustomRR())) return rootAutorelease();
    return ((id(*)(objc_object *, SEL))objc_msgSend)(this, SEL_autorelease);
}

__attribute__((noinline,used))
id
objc_object::rootAutorelease2() {
    assert(!isTaggedPointer());
    id obj = (id)this;
    assert(obj);
    assert(!obj->isTaggedPointer());
//    id *dest __unused = autoreleaseFast(obj);
//    assert(!dest  ||  dest == ((id*)1)  ||  *dest == obj);
    return obj;
}

bool
objc_object::overrelease_error() {
//    _objc_inform_now_and_on_crash("%s object %p overreleased while already deallocating; break on objc_overrelease_during_dealloc_error to debug", object_getClassName((id)this), this);
//    objc_overrelease_during_dealloc_error();
    return false;  // allow rootRelease() to tail-call this
}

inline id
objc_object::rootRetain() {
    return rootRetain(false, false);
}

inline bool
objc_object::rootTryRetain() {
    return rootRetain(true, false) ? true : false;
}

inline bool
objc_object::rootRelease() {
    return rootRelease(true, false);
}

// Base autorelease implementation, ignoring overrides.
inline id
objc_object::rootAutorelease() {
    if (isTaggedPointer()) return (id)this;
//    if (prepareOptimizedReturn(ReturnAtPlus1)) return (id)this;
    
    return rootAutorelease2();
}

id
objc_object::rootRetain_overflow(bool tryRetain) {
    return rootRetain(tryRetain, true);
}

bool
objc_object::rootRelease_underflow(bool performDealloc) {
    return rootRelease(performDealloc, true);
}

inline bool
objc_object::rootRelease(bool performDealloc, bool handleUnderflow) {
//    assert(!UseGC);
    if (isTaggedPointer()) return false;
    
    bool sideTableLocked = false;
    
    isa_t oldisa;
    isa_t newisa;
    
retry:
    do {
        oldisa = LoadExclusive(&isa.bits);
        newisa = oldisa;
        if (!newisa.indexed) goto unindexed;
        // don't check newisa.fast_rr; we already called any RR overrides
        uintptr_t carry;
        newisa.bits = subc(newisa.bits, RC_ONE, 0, &carry);  // extra_rc--
        if (carry) goto underflow;
    } while (!StoreReleaseExclusive(&isa.bits, oldisa.bits, newisa.bits));
    
    if (sideTableLocked) sidetable_unlock();
    return false;
    
underflow:
    // newisa.extra_rc-- underflowed: borrow from side table or deallocate
    
    // abandon newisa to undo the decrement
    newisa = oldisa;
    
    if (newisa.has_sidetable_rc) {
        if (!handleUnderflow) {
            return rootRelease_underflow(performDealloc);
        }
        
        // Transfer retain count from side table to inline storage.
        
        if (!sideTableLocked) {
            sidetable_lock();
            sideTableLocked = true;
            if (!isa.indexed) {
                // Lost a race vs the indexed -> not indexed transition
                // before we got the side table lock. Stop now to avoid
                // breaking the safety checks in the sidetable ExtraRC code.
                goto unindexed;
            }
        }
        
        // Try to remove some retain counts from the side table.
        size_t borrowed = sidetable_subExtraRC_nolock(RC_HALF);
        
        // To avoid races, has_sidetable_rc must remain set
        // even if the side table count is now zero.
        
        if (borrowed > 0) {
            // Side table retain count decreased.
            // Try to add them to the inline count.
            newisa.extra_rc = borrowed - 1;  // redo the original decrement too
            bool stored = StoreExclusive(&isa.bits, oldisa.bits, newisa.bits);
            if (!stored) {
                // Inline update failed.
                // Try it again right now. This prevents livelock on LL/SC
                // architectures where the side table access itself may have
                // dropped the reservation.
                isa_t oldisa2 = LoadExclusive(&isa.bits);
                isa_t newisa2 = oldisa2;
                if (newisa2.indexed) {
                    uintptr_t overflow;
                    newisa2.bits =
                    addc(newisa2.bits, RC_ONE * (borrowed-1), 0, &overflow);
                    if (!overflow) {
                        stored = StoreReleaseExclusive(&isa.bits, oldisa2.bits,
                                                       newisa2.bits);
                    }
                }
            }
            
            if (!stored) {
                // Inline update failed.
                // Put the retains back in the side table.
                sidetable_addExtraRC_nolock(borrowed);
                goto retry;
            }
            
            // Decrement successful after borrowing from side table.
            // This decrement cannot be the deallocating decrement - the side
            // table lock and has_sidetable_rc bit ensure that if everyone
            // else tried to -release while we worked, the last one would block.
            sidetable_unlock();
            return false;
        }
        else {
            // Side table is empty after all. Fall-through to the dealloc path.
        }
    }
    
    // Really deallocate.
    
    if (sideTableLocked) sidetable_unlock();
    
    if (newisa.deallocating) {
        return overrelease_error();
    }
    newisa.deallocating = true;
    if (!StoreExclusive(&isa.bits, oldisa.bits, newisa.bits)) goto retry;
    __sync_synchronize();
    if (performDealloc) {
        ((void(*)(objc_object *, SEL))objc_msgSend)(this, SEL_dealloc);
    }
    return true;
    
unindexed:
    if (sideTableLocked) sidetable_unlock();
    return sidetable_release(performDealloc);
}

inline id
objc_object::rootRetain(bool tryRetain, bool handleOverflow) {
//    assert(!UseGC);
    if (isTaggedPointer()) return (id)this;
    
    bool sideTableLocked = false;
    bool transcribeToSideTable = false;
    
    isa_t oldisa;
    isa_t newisa;
    
    do {
        transcribeToSideTable = false;
        oldisa = LoadExclusive(&isa.bits);
        newisa = oldisa;
        if (!newisa.indexed) goto unindexed;
        // don't check newisa.fast_rr; we already called any RR overrides
        if (tryRetain && newisa.deallocating) goto tryfail;
        uintptr_t carry;
        newisa.bits = addc(newisa.bits, RC_ONE, 0, &carry);  // extra_rc++
        
        if (carry) {
            // newisa.extra_rc++ overflowed
            if (!handleOverflow) return rootRetain_overflow(tryRetain);
            // Leave half of the retain counts inline and
            // prepare to copy the other half to the side table.
            if (!tryRetain && !sideTableLocked) sidetable_lock();
            sideTableLocked = true;
            transcribeToSideTable = true;
            newisa.extra_rc = RC_HALF;
            newisa.has_sidetable_rc = true;
        }
    } while (!StoreExclusive(&isa.bits, oldisa.bits, newisa.bits));
    
    if (transcribeToSideTable) {
        // Copy the other half of the retain counts to the side table.
        sidetable_addExtraRC_nolock(RC_HALF);
    }
    
    if (!tryRetain && sideTableLocked) sidetable_unlock();
    return (id)this;
    
tryfail:
    if (!tryRetain && sideTableLocked) sidetable_unlock();
    return nil;
    
unindexed:
    if (!tryRetain && sideTableLocked) sidetable_unlock();
    if (tryRetain) return sidetable_tryRetain() ? (id)this : nil;
    else return sidetable_retain();
}

inline bool
objc_object::hasIndexedIsa() {
    return isa.indexed;
}

inline Class
objc_object::getIsa() {
    if (isTaggedPointer()) {
//        uintptr_t slot = ((uintptr_t)this >> TAG_SLOT_SHIFT) & TAG_SLOT_MASK;
        return ISA();
//        return objc_tag_classes[slot];
    }
    return ISA();
}

inline uintptr_t
objc_object::rootRetainCount() {
    if (isTaggedPointer()) return (uintptr_t)this;
    
    sidetable_lock();
    isa_t bits = LoadExclusive(&isa.bits);
//    ClearExclusive(&isa.bits);
    if (bits.indexed) {
        uintptr_t rc = 1 + bits.extra_rc;
        if (bits.has_sidetable_rc) {
//            rc += sidetable_getExtraRC_nolock();
        }
        sidetable_unlock();
        return rc;
    }
    
    sidetable_unlock();
//    return sidetable_retainCount();
    return 1;
}

inline bool
objc_object::hasCxxDtor() {
    assert(!isTaggedPointer());
    if (isa.indexed) return isa.has_cxx_dtor;
    else return isa.cls->hasCxxDtor();
}

void
objc_object::sidetable_lock() {
//    SideTable& table = SideTables()[this];
//    table.lock();
}

void
objc_object::sidetable_unlock() {
//    SideTable& table = SideTables()[this];
//    table.unlock();
}


// Move the entire retain count to the side table,
// as well as isDeallocating and weaklyReferenced.
void
objc_object::sidetable_moveExtraRC_nolock(size_t extra_rc,
                                          bool isDeallocating,
                                          bool weaklyReferenced) {
    assert(!isa.indexed);        // should already be changed to not-indexed
//    SideTable& table = SideTables()[this];
    
//    size_t& refcntStorage = table.refcnts[this];
//    size_t oldRefcnt = refcntStorage;
    // not deallocating - that was in the isa
//    assert((oldRefcnt & SIDE_TABLE_DEALLOCATING) == 0);
//    assert((oldRefcnt & SIDE_TABLE_WEAKLY_REFERENCED) == 0);
    
//    uintptr_t carry;
//    size_t refcnt = addc(oldRefcnt, extra_rc << SIDE_TABLE_RC_SHIFT, 0, &carry);
//    if (carry) refcnt = SIDE_TABLE_RC_PINNED;
//    if (isDeallocating) refcnt |= SIDE_TABLE_DEALLOCATING;
//    if (weaklyReferenced) refcnt |= SIDE_TABLE_WEAKLY_REFERENCED;
    
//    refcntStorage = refcnt;
}


// Move some retain counts to the side table from the isa field.
// Returns true if the object is now pinned.
bool
objc_object::sidetable_addExtraRC_nolock(size_t delta_rc) {
    assert(isa.indexed);
//    SideTable& table = SideTables()[this];
    
//    size_t& refcntStorage = table.refcnts[this];
//    size_t oldRefcnt = refcntStorage;
    // isa-side bits should not be set here
//    assert((oldRefcnt & SIDE_TABLE_DEALLOCATING) == 0);
//    assert((oldRefcnt & SIDE_TABLE_WEAKLY_REFERENCED) == 0);
    
//    if (oldRefcnt & SIDE_TABLE_RC_PINNED) return true;
    
    uintptr_t carry = 0;
//    size_t newRefcnt = addc(oldRefcnt, delta_rc << SIDE_TABLE_RC_SHIFT, 0, &carry);
    if (carry) {
//        refcntStorage = SIDE_TABLE_RC_PINNED | (oldRefcnt & SIDE_TABLE_FLAG_MASK);
        return true;
    }
    else {
//        refcntStorage = newRefcnt;
        return false;
    }
}

size_t
objc_object::sidetable_subExtraRC_nolock(size_t delta_rc) {
    assert(isa.indexed);
//    SideTable& table = SideTables()[this];
    
//    RefcountMap::iterator it = table.refcnts.find(this);
//    if (it == table.refcnts.end()  ||  it->second == 0) {
        // Side table retain count is zero. Can't borrow.
//        return 0;
//    }
//    size_t oldRefcnt = it->second;
    
    // isa-side bits should not be set here
//    assert((oldRefcnt & SIDE_TABLE_DEALLOCATING) == 0);
//    assert((oldRefcnt & SIDE_TABLE_WEAKLY_REFERENCED) == 0);
    
//    size_t newRefcnt = oldRefcnt - (delta_rc << SIDE_TABLE_RC_SHIFT);
//    assert(oldRefcnt > newRefcnt);  // shouldn't underflow
//    it->second = newRefcnt;
    return delta_rc;
}

__attribute__((used,noinline,nothrow))
id
objc_object::sidetable_retain_slow(SideTable& table) {
    assert(!isa.indexed);
    
//    table.lock();
//    size_t& refcntStorage = table.refcnts[this];
//    if (! (refcntStorage & SIDE_TABLE_RC_PINNED)) {
//        refcntStorage += SIDE_TABLE_RC_ONE;
//    }
//    table.unlock();

    return (id)this;
}

id
objc_object::sidetable_retain() {
    assert(!isa.indexed);
//    SideTable& table = SideTables()[this];
    
//    if (table.trylock()) {
//        size_t& refcntStorage = table.refcnts[this];
//        if (! (refcntStorage & SIDE_TABLE_RC_PINNED)) {
//            refcntStorage += SIDE_TABLE_RC_ONE;
//        }
//        table.unlock();
        return (id)this;
//    }
//    return sidetable_retain_slow(table);
}


bool
objc_object::sidetable_tryRetain() {
    assert(!isa.indexed);
//    SideTable& table = SideTables()[this];
    
    // NO SPINLOCK HERE
    // _objc_rootTryRetain() is called exclusively by _objc_loadWeak(),
    // which already acquired the lock on our behalf.
    
    // fixme can't do this efficiently with os_lock_handoff_s
    // if (table.slock == 0) {
    //     _objc_fatal("Do not call -_tryRetain.");
    // }
    
    bool result = true;
//    RefcountMap::iterator it = table.refcnts.find(this);
//    if (it == table.refcnts.end()) {
//        table.refcnts[this] = SIDE_TABLE_RC_ONE;
//    } else if (it->second & SIDE_TABLE_DEALLOCATING) {
//        result = false;
//    } else if (! (it->second & SIDE_TABLE_RC_PINNED)) {
//        it->second += SIDE_TABLE_RC_ONE;
//    }
    
    return result;
}

__attribute__((used,noinline,nothrow))
uintptr_t
objc_object::sidetable_release_slow(SideTable& table, bool performDealloc) {
    assert(!isa.indexed);
    bool do_dealloc = false;
    
//    table.lock();
//    RefcountMap::iterator it = table.refcnts.find(this);
//    if (it == table.refcnts.end()) {
//        do_dealloc = true;
//        table.refcnts[this] = SIDE_TABLE_DEALLOCATING;
//    } else if (it->second < SIDE_TABLE_DEALLOCATING) {
        // SIDE_TABLE_WEAKLY_REFERENCED may be set. Don't change it.
        do_dealloc = true;
//        it->second |= SIDE_TABLE_DEALLOCATING;
//    } else if (! (it->second & SIDE_TABLE_RC_PINNED)) {
//        it->second -= SIDE_TABLE_RC_ONE;
//    }
//    table.unlock();
    if (do_dealloc  &&  performDealloc) {
        ((void(*)(objc_object *, SEL))objc_msgSend)(this, SEL_dealloc);
    }
    return do_dealloc;
}

uintptr_t
objc_object::sidetable_release(bool performDealloc) {
    assert(!isa.indexed);
//    SideTable& table = SideTables()[this];
    
    bool do_dealloc = false;
    
//    if (table.trylock()) {
//        RefcountMap::iterator it = table.refcnts.find(this);
//        if (it == table.refcnts.end()) {
//            do_dealloc = true;
//            table.refcnts[this] = SIDE_TABLE_DEALLOCATING;
//        } else if (it->second < SIDE_TABLE_DEALLOCATING) {
//             SIDE_TABLE_WEAKLY_REFERENCED may be set. Don't change it.
            do_dealloc = true;
//            it->second |= SIDE_TABLE_DEALLOCATING;
//        } else if (! (it->second & SIDE_TABLE_RC_PINNED)) {
//            it->second -= SIDE_TABLE_RC_ONE;
//        }
//        table.unlock();
        if (do_dealloc  &&  performDealloc) {
            ((void(*)(objc_object *, SEL))objc_msgSend)(this, SEL_dealloc);
        }
        return do_dealloc;
//    }
    
//    return sidetable_release_slow(table, performDealloc);
}



static bool
scanMangledField(const char *&string, const char *end,
                             const char *&field, int& length) {
    // Leading zero not allowed.
    if (*string == '0') return false;
    
    length = 0;
    field = string;
    while (field < end) {
        char c = *field;
        if (!isdigit(c)) break;
        field++;
        if (__builtin_smul_overflow(length, 10, &length)) return false;
        if (__builtin_sadd_overflow(length, c - '0', &length)) return false;
    }
    
    string = field + length;
    return length > 0  &&  string <= end;
}

static char *
copySwiftV1DemangledName(const char *string, bool isProtocol = false) {
    if (!string) return nil;
    
    // Swift mangling prefix.
    if (strncmp(string, isProtocol ? "_TtP" : "_TtC", 4) != 0) return nil;
    string += 4;
    
    const char *end = string + strlen(string);
    
    // Module name.
    const char *prefix;
    int prefixLength;
    if (string[0] == 's') {
        // "s" is the Swift module.
        prefix = "Swift";
        prefixLength = 5;
        string += 1;
    } else {
        if (!scanMangledField(string, end, prefix, prefixLength)) return nil;
    }
    
    // Class or protocol name.
    const char *suffix;
    int suffixLength;
    if (!scanMangledField(string, end, suffix, suffixLength)) return nil;
    
    if (isProtocol) {
        // Remainder must be "_".
        if (strcmp(string, "_") != 0) return nil;
    } else {
        // Remainder must be empty.
        if (string != end) return nil;
    }
    
    char *result;
    asprintf(&result, "%.*s.%.*s", prefixLength,prefix, suffixLength,suffix);
    return result;
}

const char *
objc_class::demangledName(bool realize) {
    // Return previously demangled name if available.
    if (isRealized()  ||  isFuture()) {
        if (data()->demangledName) return data()->demangledName;
    }
    
    // Try demangling the mangled name.
    const char *mangled = mangledName();
    char *de = copySwiftV1DemangledName(mangled);
    if (isRealized()  ||  isFuture()) {
        // Class is already realized or future.
        // Save demangling result in rw data.
        // We may not own rwlock for writing so use an atomic operation instead.
//        if (!OSAtomicCompareAndSwapPtrBarrier(nil, (void*)(de ?: mangled), (void**)&data()->demangledName)) {
//            if (de) free(de);
//        }
        return data()->demangledName;
    }
    
    // Class is not yet realized.
    if (!de) {
        // Name is not mangled. Return it without caching.
        return mangled;
    }
    
    // Class is not yet realized and name is mangled. Realize the class.
    // Only objc_copyClassNamesForImage() should get here.
    
    // fixme lldb's calls to class_getName() can also get here when
    // interrogating the dyld shared cache. (rdar://27258517)
    // fixme runtimeLock.assertWriting();
    // fixme assert(realize);
    
    if (realize) {
//        runtimeLock.assertWriting();
//        realizeClass((Class)this);
        data()->demangledName = de;
        return de;
    }
//    else {
//        // Save the string to avoid leaks.
//        char *cached;
//        {
//            mutex_locker_t lock(DemangleCacheLock);
//            if (!DemangleCache) {
//                DemangleCache = NXCreateHashTable(NXStrPrototype, 0, nil);
//            }
//            cached = (char *)NXHashInsertIfAbsent(DemangleCache, de);
//        }
//        if (cached != de) free(de);
//        return cached;
//    }
    return de;
}

static char *
copySwiftV1MangledName(const char *string, bool isProtocol = false) {
    if (!string) return nil;
    
    size_t dotCount = 0;
    size_t dotIndex = 0;
    const char *s;
    for (s = string; *s; s++) {
        if (*s == '.') {
            dotCount++;
            dotIndex = s - string;
        }
    }
    size_t stringLength = s - string;
    
    if (dotCount != 1  ||  dotIndex == 0  ||  dotIndex >= stringLength-1) {
        return nil;
    }
    
    const char *prefix = string;
    size_t prefixLength = dotIndex;
    const char *suffix = string + dotIndex + 1;
    size_t suffixLength = stringLength - (dotIndex + 1);
    
    char *name;
    
    if (prefixLength == 5  &&  memcmp(prefix, "Swift", 5) == 0) {
        asprintf(&name, "_Tt%cs%zu%.*s%s",
                 isProtocol ? 'P' : 'C',
                 suffixLength, (int)suffixLength, suffix,
                 isProtocol ? "_" : "");
    } else {
        asprintf(&name, "_Tt%c%zu%.*s%zu%.*s%s",
                 isProtocol ? 'P' : 'C',
                 prefixLength, (int)prefixLength, prefix,
                 suffixLength, (int)suffixLength, suffix,
                 isProtocol ? "_" : "");
    }
    return name;
}

static Class
getClass_impl(const char *name) {
//    runtimeLock.assertLocked();
    
    // allocated in _read_images
//    assert(gdb_objc_realized_classes);
    
    // Try runtime-allocated table
//    Class result = (Class)NXMapGet(gdb_objc_realized_classes, name);
//    if (result) return result;
    
    // Try table from dyld shared cache
//    return getPreoptimizedClass(name);
    return nil;
}

static Class
getClass(const char *name) {
//    runtimeLock.assertLocked();
    
    // Try name as-is
    Class result = getClass_impl(name);
    if (result) return result;
    
    // Try Swift-mangled equivalent of the given name.
    if (char *swName = copySwiftV1MangledName(name)) {
        result = getClass_impl(swName);
        free(swName);
        return result;
    }
    
    return nil;
}

Class
look_up_class(const char *name,
              bool includeUnconnected __attribute__((unused)),
              bool includeClassHandler __attribute__((unused))) {
    if (!name) return nil;
    
    Class result;
    bool unrealized;
    {
//        rwlock_reader_t lock(runtimeLock);
        result = getClass(name);
        unrealized = result  &&  !result->isRealized();
    }
    if (unrealized) {
//        rwlock_writer_t lock(runtimeLock);
//        realizeClass(result);
    }
    return result;
}

IMP lookupMethodInClassAndLoadCache(Class cls, SEL sel) {
//    Method meth;
//    IMP imp;
    
    // fixme this is incomplete - no resolver, +initialize -
    // but it's only used for .cxx_construct/destruct so we don't care
    assert(sel == SEL_cxx_construct  ||  sel == SEL_cxx_destruct);
    
    // Search cache first.
//    imp = cache_getImp(cls, sel);
//    if (imp) return imp;
    
    // Cache miss. Search method list.
    
//    rwlock_reader_t lock(runtimeLock);
    
//    meth = getMethodNoSuper_nolock(cls, sel);
    
//    if (meth) {
//        // Hit in method list. Cache it.
//        cache_fill(cls, sel, meth->imp, nil);
//        return meth->imp;
//    } else {
//        // Miss in method list. Cache objc_msgForward.
//        cache_fill(cls, sel, _objc_msgForward_impcache, nil);
//        return _objc_msgForward_impcache;
//    }
    return nil;
}

void _object_remove_assocations(id object) {
//    vector< ObjcAssociation,ObjcAllocator<ObjcAssociation> > elements;
//    {
//        AssociationsManager manager;
//        AssociationsHashMap &associations(manager.associations());
//        if (associations.size() == 0) return;
//        disguised_ptr_t disguised_object = DISGUISE(object);
//        AssociationsHashMap::iterator i = associations.find(disguised_object);
//        if (i != associations.end()) {
//            // copy all of the associations that need to be removed.
//            ObjectAssociationMap *refs = i->second;
//            for (ObjectAssociationMap::iterator j = refs->begin(), end = refs->end(); j != end; ++j) {
//                elements.push_back(j->second);
//            }
//            // remove the secondary table.
//            delete refs;
//            associations.erase(i);
//        }
//    }
//    // the calls to releaseValue() happen outside of the lock.
//    for_each(elements.begin(), elements.end(), ReleaseValue());
}

static void object_cxxDestructFromClass(id obj, Class cls) {
    void (*dtor)(id);
    
    // Call cls's dtor first, then superclasses's dtors.
    
    for ( ; cls; cls = cls->superclass) {
        if (!cls->hasCxxDtor()) return;
        dtor = (void(*)(id))
        lookupMethodInClassAndLoadCache(cls, SEL_cxx_destruct);
//        if (dtor != (void(*)(id))_objc_msgForward_impcache) {
//            (*dtor)(obj);
//        }
    }
}

void object_cxxDestruct(id obj) {
    if (!obj) return;
    if (obj->isTaggedPointer()) return;
    object_cxxDestructFromClass(obj, obj->ISA());
}

id
object_cxxConstructFromClass(id obj, Class cls) {
    assert(cls->hasCxxCtor());  // required for performance, not correctness
    
    id (*ctor)(id);
    Class supercls;
    
    supercls = cls->superclass;
    
    // Call superclasses' ctors first, if any.
    if (supercls  &&  supercls->hasCxxCtor()) {
        bool ok = object_cxxConstructFromClass(obj, supercls);
        if (!ok) return nil;  // some superclass's ctor failed - give up
    }
    
    // Find this class's ctor, if any.
    ctor = (id(*)(id))lookupMethodInClassAndLoadCache(cls, SEL_cxx_construct);
//    if (ctor == (id(*)(id))_objc_msgForward_impcache) return obj;  // no ctor - ok
    if ((*ctor)(obj)) return obj;  // ctor called and succeeded - ok
    
    // This class's ctor was called and failed.
    // Call superclasses's dtors to clean up.
    if (supercls) object_cxxDestructFromClass(obj, supercls);
    return nil;
}

static __attribute__((always_inline))
id
_class_createInstanceFromZone(Class cls, size_t extraBytes, void *zone,
                              bool cxxConstruct = true,
                              size_t *outAllocatedSize = nil) {
    if (!cls) return nil;
    
    assert(cls->isRealized());
    
    // Read class's info bits all at once for performance
    bool hasCxxCtor = cls->hasCxxCtor();
//    bool hasCxxDtor = cls->hasCxxDtor();
//    bool fast = cls->canAllocNonpointer();
    
    size_t size = cls->instanceSize(extraBytes);
    if (outAllocatedSize) *outAllocatedSize = size;
    
    id obj;
//    if (!zone  &&  fast) {
//        obj = (id)calloc(1, size);
//        if (!obj) return nil;
//        obj->initInstanceIsa(cls, hasCxxDtor);
//    }
//    else {
//        if (zone) {
//            obj = (id)malloc_zone_calloc ((malloc_zone_t *)zone, 1, size);
//        } else {
            obj = (id)malloc(size);
//        }
        if (!obj) return nil;
        
        // Use raw pointer isa on the assumption that they might be
        // doing something weird with the zone or RR.
        obj->initIsa(cls);
//    }
    
    if (cxxConstruct && hasCxxCtor) {
        obj = _objc_constructOrFree(obj, cls);
    }
    
    return obj;
}

static Protocol *getProtocol(const char *name) {
    // Try name as-is.
//    Protocol *result = (Protocol *)NXMapGet(protocols(), name);
    Protocol *result = Nil;
    if (result) return result;
    
    // Try Swift-mangled equivalent of the given name.
    if (char *swName = copySwiftV1MangledName(name, true/*isProtocol*/)) {
//        result = (Protocol *)NXMapGet(protocols(), swName);
        free(swName);
        return result;
    }
    
    return nil;
}

static protocol_t *remapProtocol(protocol_ref_t proto) {
    protocol_t *newproto = (protocol_t *)
    getProtocol(((protocol_t *)proto)->mangledName);
    return newproto ? newproto : (protocol_t *)proto;
}

static bool
protocol_conformsToProtocol_nolock(protocol_t *self, protocol_t *other) {
    if (!self  ||  !other) {
        return NO;
    }
    
    // protocols need not be fixed up
    
    if (0 == strcmp(self->mangledName, other->mangledName)) {
        return YES;
    }
    
    if (self->protocols) {
        uintptr_t i;
        for (i = 0; i < self->protocols->count; i++) {
            protocol_t *proto = remapProtocol(self->protocols->list[i]);
            if (0 == strcmp(other->mangledName, proto->mangledName)) {
                return YES;
            }
            if (protocol_conformsToProtocol_nolock(proto, other)) {
                return YES;
            }
        }
    }
    
    return NO;
}

extern "C" {
BOOL object_isClass(id obj) {
    if (!obj) return NO;
    return obj->isClass();
}

Class object_getClass(id obj) {
    if (obj) return obj->getIsa();
    else return Nil;
}
    
Class _Nullable
class_getSuperclass(Class _Nullable cls) {
    if (cls == Nil) {
        return Nil;
    }
    return cls->superclass;
}
    
BOOL
class_isMetaClass(Class _Nullable cls) {
    if (cls == Nil) {
        return NO;
    }
    return cls->isMetaClass();
}
    
const char * _Nonnull
class_getName(Class _Nullable cls) {
    if (!cls) {
        return "nil";
    }
    return cls->demangledName();
}

id
class_createInstance(Class cls, size_t extraBytes) {
    return _class_createInstanceFromZone(cls, extraBytes, nil);
}
    
__attribute__((aligned(16)))
id
objc_retain(id obj) {
    if (!obj) return obj;
    if (obj->isTaggedPointer()) return obj;
    return obj->retain();
}
    
id _objc_rootRetain(id a) {
    assert(a);
    return a->rootRetain();
}
    
void _objc_rootRelease(id a) {
    assert(a);
    a->rootRelease();
}
    
id _objc_rootAutorelease(id a) {
    assert(a);
    return a->rootAutorelease();
}

void _objc_rootDealloc(id a) {
    assert(a);
    a->rootDealloc();
}
    
BOOL _objc_rootTryRetain(id a) {
    return a->rootTryRetain();
}
    
id _objc_rootInit(id a) {
    return a;
}
    
uintptr_t _objc_rootRetainCount(id a) {
    return a->rootRetainCount();
}
    
__attribute__((aligned(16)))
void
objc_release(id obj) {
    if (!obj) return;
    if (obj->isTaggedPointer()) return;
    return obj->release();
}
    
__attribute__((aligned(16)))
id
objc_autorelease(id obj) {
    if (!obj) return obj;
    if (obj->isTaggedPointer()) return obj;
    return obj->autorelease();
}
    
Class objc_lookUpClass(const char *aClassName) {
    if (!aClassName) return Nil;
        
    // NO unconnected, NO class handler
    return look_up_class(aClassName, NO, NO);
}
    
id
objc_constructInstance(Class cls, void *bytes) {
    if (!cls || !bytes) return nil;
        
    id obj = (id)bytes;
        
    obj->initIsa(cls);
        
    if (cls->hasCxxCtor()) {
        return object_cxxConstructFromClass(obj, cls);
    } else {
        return obj;
    }
}
    
void *
objc_destructInstance(id obj) {
    if (obj) {
        // Read all of the flags at once for performance.
        bool cxx = obj->hasCxxDtor();
//        bool assoc = obj->hasAssociatedObjects();
            
        // This order is important.
        if (cxx) object_cxxDestruct(obj);
//        if (assoc) _object_remove_assocations(obj);
//        obj->clearDeallocating();
    }
        
    return obj;
}
    
BOOL class_conformsToProtocol(Class cls, Protocol *protocol) {
    protocol_t *proto = ((protocol_t*)protocol);
    if (!cls || !protocol) {
        return NO;
    }
    assert(cls->isRealized());
    for (const auto& protocol_refrence : cls->data()->protocols) {
        protocol_t *p = remapProtocol(protocol_refrence);
        if (p == proto || protocol_conformsToProtocol_nolock(p, proto)) {
            return YES;
        }
    }
    return NO;
}
    
id
object_dispose(id obj) {
    if (!obj) return nil;
        
    objc_destructInstance(obj);
    free(obj);
        
    return nil;
}
    
id
_objc_constructOrFree(id bytes, Class cls) {
    assert(cls->hasCxxCtor());  // for performance, not correctness
        
    id obj = object_cxxConstructFromClass(bytes, cls);
    if (!obj) free(bytes);
        
    return obj;
}
}
