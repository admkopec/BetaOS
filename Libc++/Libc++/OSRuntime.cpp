//
//  OSRuntime.cpp
//  Libc++
//
//  Created by Adam Kopeć on 6/7/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "OSRuntime.hpp"
#include <stdint.h>
#include <string.h>

extern "C" { void * kalloc_(uint32_t size); void free_(void * data, uint32_t size); void bzero(void * addr, size_t length); void panic(const char* fmt, ...); void kprintf(const char* fmt, ...); }

const char * OSReturnStrings[4] = { "Success", "Timeout", "Failure", "Error" };

struct _mhead {
    size_t  mlen;
    char    dat[0] __attribute__((aligned(16)));
};

void* OSRuntime::OSMalloc(size_t size) {
    struct _mhead *mem;
    size_t  memsize = sizeof(*mem) + size;
    
    if (size == 0) {
        return (0);
    }
    
    mem = (struct _mhead *) kalloc_((uint32_t)memsize);
    if (!mem) {
        return (0);
    }
    
    mem->mlen = memsize;
    bzero(mem->dat, size);
    
    return mem->dat;
}

void OSRuntime::OSFree(void * addr) {
    struct _mhead * header;
    if (!addr) {
        return;
    }
    
    header = (struct _mhead*) addr;
    header--;
    
    memset(header, 0xbb, header->mlen);
    free_(header, (uint32_t)header->mlen);
}

void *
operator new(size_t size/*, size_t align*/) noexcept {
    void * result;
    
    return (void *) OSRuntime::OSMalloc(size);
    return result;
}

void
operator delete(void * addr) noexcept {
    OSRuntime::OSFree(addr);
    return;
}

void *
operator new[](unsigned long sz/*, size_t align*/) noexcept {
    if (sz == 0) {
        sz = 1;
    }
    return OSRuntime::OSMalloc(sz);
}

void
operator delete[](void * ptr) noexcept {
    if (ptr) {
        OSRuntime::OSFree(ptr);
    }
    
    return;
}

namespace std {
    void
    __throw_length_error(const char *msg) {
        panic("Size of array created by new[] has overflowed: %s", msg);
    }
    void
    __throw_logic_error(const char *msg) {
        panic("Logic error: %s", msg);
    }
    void
    __throw_bad_alloc() {
        panic("Bad alloc");
    }
    void
    __throw_system_error(int error) {
        panic("System error: %d", error);
    }
    void
    __throw_out_of_range_fmt(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        va_end(args);
        panic("Out of range");
    }
};

int
__cxa_guard_acquire(void *guard) {
    kprintf("__cxa_guard_acquire(%p)\n", guard);
    return 0;
}

void
__cxa_guard_release(void *guard) {
    kprintf("__cxa_guard_release(%p)\n", guard);
}


int
__cxa_atexit(void (*func) (void *), void *arg, void *dso_handle) {
    kprintf("__cxa_atexit(%p, %p, %p)\n", func, arg, dso_handle);
    // Exit never occurs so ignore this handler, return success
    return 0;
}

extern "C" {
    void * malloc(size_t size) {
        return OSRuntime::OSMalloc(size);
    }
    void free(void *ptr) {
        OSRuntime::OSFree(ptr);
    }
}
