//
//  NSBlocks.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 1/31/18.
//

#import <stdbool.h>
#import <stdlib.h>
#import <stdio.h>
#import <string.h>
#undef memmove

void * _NSConcreteStackBlock[32];
void * _NSConcreteMallocBlock[32];
void * _NSConcreteAutoBlock[32];
void * _NSConcreteFinalizingBlock[32];
void * _NSConcreteGlobalBlock[32];
void * _NSConcreteWeakBlockVariable[32];

enum {
    BLOCK_REFCOUNT_MASK =     (0xffff),
    BLOCK_NEEDS_FREE =        (1 << 24),
    BLOCK_HAS_COPY_DISPOSE =  (1 << 25),
    BLOCK_HAS_CTOR =          (1 << 26),
    BLOCK_IS_GC =             (1 << 27),
    BLOCK_IS_GLOBAL =         (1 << 28),
    BLOCK_HAS_DESCRIPTOR =    (1 << 29),
};

enum {
    BLOCK_FIELD_IS_OBJECT   =  3,  // id, NSObject, __attribute__((NSObject)), block, ...
    BLOCK_FIELD_IS_BLOCK    =  7,  // a block variable
    BLOCK_FIELD_IS_BYREF    =  8,  // the on stack structure holding the __block variable
    BLOCK_FIELD_IS_WEAK     = 16,  // declared __weak, only used in byref copy helpers
    BLOCK_BYREF_CALLER      = 128, // called from __block (byref) copy/dispose support routines.
};

__unused static void *_Block_copy_class = _NSConcreteMallocBlock;
__unused static void *_Block_copy_finalizing_class = _NSConcreteMallocBlock;
__unused static int _Block_copy_flag = BLOCK_NEEDS_FREE;
static int _Byref_flag_initial_value = BLOCK_NEEDS_FREE | 2;

static bool isGC = false;

static int latching_incr_int(int *where) {
    while (1) {
        int old_value = *(volatile int *)where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return BLOCK_REFCOUNT_MASK;
        }
        if ((*where = old_value+1) == old_value/*OSAtomicCompareAndSwapInt(old_value, old_value+1, (volatile int *)where)*/) {
            return old_value+1;
        }
    }
}

static int latching_decr_int(int *where) {
    while (1) {
        int old_value = *(volatile int *)where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return BLOCK_REFCOUNT_MASK;
        }
        if ((old_value & BLOCK_REFCOUNT_MASK) == 0) {
            return 0;
        }
        if ((*where = old_value-1) == old_value/*OSAtomicCompareAndSwapInt(old_value, old_value-1, (volatile int *)where)*/) {
            return old_value-1;
        }
    }
}

struct Block_descriptor {
    unsigned long int reserved;
    unsigned long int size;
    void (*copy)(void *dst, void *src);
    void (*dispose)(void *);
};

struct Block_layout {
    void *isa;
    int flags;
    int reserved;
    void (*invoke)(void *, ...);
    struct Block_descriptor *descriptor;
};

struct Block_byref {
    void *isa;
    struct Block_byref *forwarding;
    int flags;
    int size;
    void (*byref_keep)(struct Block_byref *dst, struct Block_byref *src);
    void (*byref_destroy)(struct Block_byref *);
};

struct Block_byref_header {
    void *isa;
    struct Block_byref *forwarding;
    int flags;
    int size;
};

static void *_Block_alloc_default(const unsigned long size, const bool initialCountIsOne, const bool isObject) {
    return malloc(size);
}

static void _Block_assign_default(void *value, void **destptr) {
    *destptr = value;
}

static void _Block_setHasRefcount_default(const void *ptr, const bool hasRefcount) { }

__unused static void _Block_do_nothing(const void *aBlock) { }

static void _Block_retain_object_default(const void *ptr) {
    if (!ptr) return;
}

static void _Block_release_object_default(const void *ptr) {
    if (!ptr) return;
}

static void _Block_assign_weak_default(const void *ptr, void *dest) {
    *(long *)dest = (long)ptr;
}

static void _Block_memmove_default(void *dst, void *src, unsigned long size) {
    memmove(dst, src, (size_t)size);
}

static void *(*_Block_allocator)(const unsigned long, const bool isOne, const bool isObject) = _Block_alloc_default;
static void (*_Block_deallocator)(const void *) = (void (*)(const void *))free;
static void (*_Block_assign)(void *value, void **destptr) = _Block_assign_default;
static void (*_Block_setHasRefcount)(const void *ptr, const bool hasRefcount) = _Block_setHasRefcount_default;
static void (*_Block_retain_object)(const void *ptr) = _Block_retain_object_default;
static void (*_Block_release_object)(const void *ptr) = _Block_release_object_default;
static void (*_Block_assign_weak)(const void *dest, void *ptr) = _Block_assign_weak_default;
static void (*_Block_memmove)(void *dest, void *src, unsigned long size) = _Block_memmove_default;

void _Block_release(const void *arg) {
    struct Block_layout *aBlock = (struct Block_layout *)arg;
    int32_t newCount;
    if (!aBlock) return;
    newCount = latching_decr_int(&aBlock->flags) & BLOCK_REFCOUNT_MASK;
    if (newCount > 0) return;
    // Hit zero
    if (aBlock->flags & BLOCK_IS_GC) {
        // Tell GC we no longer have our own refcounts.  GC will decr its refcount
        // and unless someone has done a CFRetain or marked it uncollectable it will
        // now be subject to GC reclamation.
        _Block_setHasRefcount(aBlock, false);
    }
    else if (aBlock->flags & BLOCK_NEEDS_FREE) {
        if (aBlock->flags & BLOCK_HAS_COPY_DISPOSE)(*aBlock->descriptor->dispose)(aBlock);
        _Block_deallocator(aBlock);
    }
    else if (aBlock->flags & BLOCK_IS_GLOBAL) {
        ;
    }
    else {
        printf("Block_release called upon a stack Block: %p, ignored\n", aBlock);
    }
}

static void _Block_destroy(const void *arg) {
    struct Block_layout *aBlock;
    if (!arg) return;
    aBlock = (struct Block_layout *)arg;
    if (aBlock->flags & BLOCK_IS_GC) {
        return;
    }
    _Block_release(aBlock);
}

unsigned long int _Block_size(void *arg) {
    return ((struct Block_layout *)arg)->descriptor->size;
}

static void *_Block_copy_internal(const void *arg, const int flags) {
    struct Block_layout *aBlock;
    const bool wantsOne = ((1 << 16) & flags) == (1 << 16);
    
    if (!arg) return NULL;
    
    // The following would be better done as a switch statement
    aBlock = (struct Block_layout *)arg;
    if (aBlock->flags & BLOCK_NEEDS_FREE) {
        // latches on high
        latching_incr_int(&aBlock->flags);
        return aBlock;
    } else if (aBlock->flags & BLOCK_IS_GC) {
        // GC refcounting is expensive so do most refcounting here.
        if (wantsOne && ((latching_incr_int(&aBlock->flags) & BLOCK_REFCOUNT_MASK) == 1)) {
            // Tell collector to hang on this - it will bump the GC refcount version
            _Block_setHasRefcount(aBlock, true);
        }
        return aBlock;
    } else if (aBlock->flags & BLOCK_IS_GLOBAL) {
        return aBlock;
    }
    
    // Its a stack block.  Make a copy.
    if (!isGC) {
        struct Block_layout *result = malloc(aBlock->descriptor->size);
        if (!result) return (void *)0;
        memmove(result, aBlock, aBlock->descriptor->size); // bitcopy first
        // reset refcount
        result->flags &= ~(BLOCK_REFCOUNT_MASK);    // XXX not needed
        result->flags |= BLOCK_NEEDS_FREE | 1;
        result->isa = _NSConcreteMallocBlock;
        if (result->flags & BLOCK_HAS_COPY_DISPOSE) {
            //printf("calling block copy helper %p(%p, %p)...\n", aBlock->descriptor->copy, result, aBlock);
            (*aBlock->descriptor->copy)(result, aBlock); // do fixup
        }
        return result;
    } else {
        // Under GC want allocation with refcount 1 so we ask for "true" if wantsOne
        // This allows the copy helper routines to make non-refcounted block copies under GC
        unsigned long int flags = aBlock->flags;
        bool hasCTOR = (flags & BLOCK_HAS_CTOR) != 0;
        struct Block_layout *result = _Block_allocator(aBlock->descriptor->size, wantsOne, hasCTOR);
        if (!result) return (void *)0;
        memmove(result, aBlock, aBlock->descriptor->size); // bitcopy first
        // reset refcount
        // if we copy a malloc block to a GC block then we need to clear NEEDS_FREE.
        flags &= ~(BLOCK_NEEDS_FREE|BLOCK_REFCOUNT_MASK);   // XXX not needed
        if (wantsOne)
            flags |= BLOCK_IS_GC | 1;
        else
            flags |= BLOCK_IS_GC;
        result->flags = (int)flags;
        if (flags & BLOCK_HAS_COPY_DISPOSE) {
            //printf("calling block copy helper...\n");
            (*aBlock->descriptor->copy)(result, aBlock); // do fixup
        }
        if (hasCTOR) {
            result->isa = _NSConcreteFinalizingBlock;
        } else {
            result->isa = _NSConcreteAutoBlock;
        }
        return result;
    }
}

void *_Block_copy(const void *arg) {
    return _Block_copy_internal(arg, (1 << 16));
}

static void _Block_byref_assign_copy(void *dest, const void *arg, const int flags) {
    struct Block_byref **destp = (struct Block_byref **)dest;
    struct Block_byref *src = (struct Block_byref *)arg;
    
    if (src->forwarding->flags & BLOCK_IS_GC) {
        ;   // don't need to do any more work
    }
    else if ((src->forwarding->flags & BLOCK_REFCOUNT_MASK) == 0) {
        // src points to stack
        bool isWeak = ((flags & (BLOCK_FIELD_IS_BYREF|BLOCK_FIELD_IS_WEAK)) == (BLOCK_FIELD_IS_BYREF|BLOCK_FIELD_IS_WEAK));
        // if its weak ask for an object (only matters under GC)
        struct Block_byref *copy = (struct Block_byref *)_Block_allocator(src->size, false, isWeak);
        copy->flags = src->flags | _Byref_flag_initial_value; // non-GC one for caller, one for stack
        copy->forwarding = copy; // patch heap copy to point to itself (skip write-barrier)
        src->forwarding = copy;  // patch stack to point to heap copy
        copy->size = src->size;
        if (isWeak) {
            copy->isa = &_NSConcreteWeakBlockVariable;  // mark isa field so it gets weak scanning
        }
        if (src->flags & BLOCK_HAS_COPY_DISPOSE) {
            // Trust copy helper to copy everything of interest
            // If more than one field shows up in a byref block this is wrong XXX
            copy->byref_keep = src->byref_keep;
            copy->byref_destroy = src->byref_destroy;
            (*src->byref_keep)(copy, src);
        }
        else {
            // just bits.  Blast 'em using _Block_memmove in case they're __strong
            _Block_memmove((void *)&copy->byref_keep, (void *)&src->byref_keep, src->size - sizeof(struct Block_byref_header));
        }
    }
    // already copied to heap
    else if ((src->forwarding->flags & BLOCK_NEEDS_FREE) == BLOCK_NEEDS_FREE) {
        latching_incr_int(&src->forwarding->flags);
    }
    // assign byref data block pointer into new Block
    _Block_assign(src->forwarding, (void **)destp);
}
static void _Block_byref_release(const void *arg) {
    struct Block_byref *shared_struct = (struct Block_byref *)arg;
    int refcount;
    
    // dereference the forwarding pointer since the compiler isn't doing this anymore (ever?)
    shared_struct = shared_struct->forwarding;
    
    // To support C++ destructors under GC we arrange for there to be a finalizer for this
    // by using an isa that directs the code to a finalizer that calls the byref_destroy method.
    if ((shared_struct->flags & BLOCK_NEEDS_FREE) == 0) {
        return; // stack or GC or global
    }
    refcount = shared_struct->flags & BLOCK_REFCOUNT_MASK;
    if (refcount <= 0) {
        printf("_Block_byref_release: Block byref data structure at %p underflowed\n", arg);
    }
    else if ((latching_decr_int(&shared_struct->flags) & BLOCK_REFCOUNT_MASK) == 0) {
        if (shared_struct->flags & BLOCK_HAS_COPY_DISPOSE) {
            (*shared_struct->byref_destroy)(shared_struct);
        }
        _Block_deallocator((struct Block_layout *)shared_struct);
    }
}


void _Block_object_assign(void *destAddr, const void *object, const int flags) {
    if ((flags & BLOCK_BYREF_CALLER) == BLOCK_BYREF_CALLER) {
        if ((flags & BLOCK_FIELD_IS_WEAK) == BLOCK_FIELD_IS_WEAK) {
            _Block_assign_weak(object, destAddr);
        } else {
            // do *not* retain or *copy* __block variables whatever they are
            _Block_assign((void *)object, destAddr);
        }
    } else if ((flags & BLOCK_FIELD_IS_BYREF) == BLOCK_FIELD_IS_BYREF)  {
        // copying a __block reference from the stack Block to the heap
        // flags will indicate if it holds a __weak reference and needs a special isa
        _Block_byref_assign_copy(destAddr, object, flags);
    } else if ((flags & BLOCK_FIELD_IS_BLOCK) == BLOCK_FIELD_IS_BLOCK) {
        // copying a Block declared variable from the stack Block to the heap
        _Block_assign(_Block_copy_internal(object, flags), destAddr);
    } else if ((flags & BLOCK_FIELD_IS_OBJECT) == BLOCK_FIELD_IS_OBJECT) {
        _Block_retain_object(object);
        _Block_assign((void *)object, destAddr);
    }
}

void _Block_object_dispose(const void *object, const int flags) {
    if (flags & BLOCK_FIELD_IS_BYREF)  {
        // get rid of the __block data structure held in a Block
        _Block_byref_release(object);
    } else if ((flags & (BLOCK_FIELD_IS_BLOCK|BLOCK_BYREF_CALLER)) == BLOCK_FIELD_IS_BLOCK) {
        // get rid of a referenced Block held by this Block
        // (ignore __block Block variables, compiler doesn't need to call us)
        _Block_destroy(object);
    } else if ((flags & (BLOCK_FIELD_IS_WEAK|BLOCK_FIELD_IS_BLOCK|BLOCK_BYREF_CALLER)) == BLOCK_FIELD_IS_OBJECT) {
        // get rid of a referenced object held by this Block
        // (ignore __block object variables, compiler doesn't need to call us)
        _Block_release_object(object);
    }
}
