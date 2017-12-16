//
//  object.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 11/19/17.
//

#import "objc-private.h"
#import "runtime-private.h"
#import <objc/objc.h>

extern "C" {
    extern id _Nullable
    objc_msgSend(id _Nullable self, SEL _Nonnull op, ...);
}

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
    return true;
//    return ISA()->isMetaClass();
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



extern "C" {
BOOL object_isClass(id obj) {
    if (!obj) return NO;
    return obj->isClass();
}

Class object_getClass(id obj) {
    if (obj) return obj->getIsa();
    else return Nil;
}

__attribute__((aligned(16)))
id
objc_retain(id obj) {
    if (!obj) return obj;
    if (obj->isTaggedPointer()) return obj;
    return obj->retain();
}
    
    
__attribute__((aligned(16)))
void
objc_release(id obj) {
    if (!obj) return;
    if (obj->isTaggedPointer()) return;
    return obj->release();
}
}
