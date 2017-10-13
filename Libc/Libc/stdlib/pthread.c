//
//  pthread.c
//  Libc
//
//  Created by Adam Kopeć on 9/3/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
extern void panic(const char*, ...);

#ifdef ULTDEBUG
#define ULTDBG(x...) printf(x)
#else
#define ULTDBG(x...)
#endif

/*
 * pthread - minimal implementation of functions used to satisfy a single
 * threaded version.
 */

typedef struct pthread_mutex pthread_mutex_t;
typedef struct pthread_mutexaddr pthread_mutexattr_t;
typedef struct pthread_cond pthread_cond_t;
typedef struct pthread_rw_lock pthread_rwlock_t;
typedef int pthread_key_t;
typedef int pthread_t;
typedef long dispatch_once_t;

void dispatch_once_f(dispatch_once_t *predicate, void *context, void (*function)(void *)) {
    ULTDBG("dispatch_once_t(%p,%p,%p)\n", predicate, context, function);
    if(*predicate == 0) {
        *predicate = ~0L;
        function(context);
    }
}

int
pthread_mutex_init(__unused pthread_mutex_t *restrict mutex, __unused const pthread_mutexattr_t *restrict attr) {
    ULTDBG("pthread_mutex_init(%p,%p)\n", mutex, attr);
//        memset(mutex, 0, sizeof(pthread_mutex_t));
    return 0;
}

int
pthread_mutex_destroy(__unused pthread_mutex_t *mutex) {
    ULTDBG("pthread_mutex_destroy(%p)\n", mutex);
    return 0;
}


int
pthread_mutex_lock(__unused pthread_mutex_t *mutex) {
    ULTDBG("pthread_mutex_lock(%p)\n", mutex);
    return 0;
}

int
pthread_mutex_trylock (__unused pthread_mutex_t *mutex) {
    ULTDBG("pthread_mutex_trylock(%p)\n", mutex);
    return 0;
}


int
pthread_mutex_unlock(__unused pthread_mutex_t *mutex) {
    ULTDBG("pthread_mutex_unlock(%p)\n", mutex);
    return 0;
}


int
pthread_mutexattr_init(__unused pthread_mutexattr_t *attr) {
    ULTDBG("pthread_mutexattr_init(%p)\n", attr);
    //    memset(attr, 0, sizeof(pthread_mutexattr_t));
    return 0;
}


int
pthread_mutexattr_settype(__unused pthread_mutexattr_t *attr, __unused int kind) {
    ULTDBG("pthread_mutexattr_settype(%p, %d)\n", attr, kind);
    return 0;
}


int
pthread_mutexattr_destroy(__unused pthread_mutexattr_t *attr) {
    ULTDBG("pthread_mutexattr_destroy(%p)\n", attr);
    return 0;
}


int
pthread_cond_init(__unused pthread_cond_t *restrict cond, __unused const /*pthread_condattr_t*/void *restrict cond_attr) {
    ULTDBG("pthread_cond_init(%p,%p)\n", cond, cond_attr);
    //    memset(cond, 0, sizeof(pthread_cond_t));
    return 0;
}


int
pthread_cond_destroy(__unused pthread_cond_t *restrict cond) {
    ULTDBG("pthread_cond_destroy(%p)\n", cond);
    return 0;
}


int
pthread_cond_signal(__unused pthread_cond_t *restrict cond) {
    ULTDBG("pthread_cond_signal(%p)\n", cond);
    return 0;
}


int
pthread_cond_broadcast(__unused pthread_cond_t *restrict cond) {
    ULTDBG("pthread_cond_broadcast(%p)\n", cond);
    return 0;
}


int
pthread_cond_wait(__unused pthread_cond_t *restrict cond, __unused pthread_mutex_t *restrict mutex) {
    ULTDBG("pthread_cond_wait(%p, %p)\n", cond, mutex);
    return 0;
}


int
pthread_rwlock_init(__unused pthread_rwlock_t *restrict rwlock, __unused const /*pthread_rwlockattr_t*/void *restrict attr) {
    ULTDBG("pthread_rwlock_init(%p, %p)\n", rwlock, attr);
    return 0;
}


int
pthread_rwlock_destroy(__unused pthread_rwlock_t *rwlock) {
    ULTDBG("pthread_rwlock_destroy(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_tryrdlock(__unused pthread_rwlock_t *rwlock) {
    ULTDBG("pthread_rwlock_tryrdlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_rdlock(__unused pthread_rwlock_t *rwlock) {
    ULTDBG("pthread_rwlock_rdlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_trywrlock(__unused pthread_rwlock_t *rwlock) {
    ULTDBG("pthread_tryrwlock_wrlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_wrlock(__unused pthread_rwlock_t *rwlock) {
    ULTDBG("pthread_rwlock_wrlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_unlock(__unused pthread_rwlock_t *rwlock) {
    ULTDBG("pthread_rwlock_unlock(%p)\n", rwlock);
    return 0;
}


pthread_t
pthread_self(void) {
    ULTDBG("pthread_self()\n");
    return 1;
}


// Used as a weak symbol to detect libpthread
int
__pthread_key_create (__unused pthread_key_t *key, __unused  void (*destructor) (void *)) {
    ULTDBG("pthread_key_create(%p,%p)\n", key, destructor);
    panic("UNIMPLEMENTED: %s", __func__);
    return 0;
}

int
pthread_key_create (pthread_key_t *key, void (*destructor) (void *)) {
    return __pthread_key_create(key, destructor);
}


void *
pthread_getspecific (__unused pthread_key_t __key) {
    panic("UNIMPLEMENTED: %s", __func__);
    return NULL;
}


// Store POINTER in the thread-specific data slot identified by KEY.
int
pthread_setspecific (__unused pthread_key_t __key, __unused const void *__pointer) {
    panic("UNIMPLEMENTED: %s", __func__);
    return 0;
}

void pthread_equal() { }

// swift_once_f() used to implemented swift_once() in the stdlib
void
swift_once_f(uintptr_t *predicate, void (*function)(void *), void *context) {
    ULTDBG("swift_oncef(%p, %p, %p) [%lu]\n", predicate, function, context, *predicate);
    
    if (!*predicate) {
        *predicate = 1;
        function(context);
    }
}

