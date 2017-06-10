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

extern "C" { void * kalloc_(uint32_t size); void free_(void * data, uint32_t size); void bzero(void * addr, size_t length); void panic(const char* fmt, ...); }

struct _mhead {
    size_t  mlen;
    char    dat[0];
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
operator new(size_t size) noexcept {
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
operator new[](unsigned long sz) noexcept {
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
    __throw_length_error(const char *msg __unused) {
        panic("Size of array created by new[] has overflowed");
    }
};
