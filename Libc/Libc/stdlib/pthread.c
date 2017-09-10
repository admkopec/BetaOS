//
//  pthread.c
//  Libc
//
//  Created by Adam Kopeć on 9/3/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
extern void panic(const char*, ...);

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
    printf("dispatch_once_t(%p,%p,%p)\n", predicate, context, function);
    if(*predicate == 0) {
        *predicate = ~0L;
        function(context);
    }
}

int
pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr) {
    printf("pthread_mutex_init(%p,%p)\n", mutex, attr);
    //    memset(mutex, 0, sizeof(pthread_mutex_t));
    return 0;
}

int
pthread_mutex_destroy(pthread_mutex_t *mutex) {
    printf("pthread_mutex_destroy(%p)\n", mutex);
    return 0;
}


int
pthread_mutex_lock(pthread_mutex_t *mutex) {
    printf("pthread_mutex_lock(%p)\n", mutex);
    return 0;
}

int
pthread_mutex_trylock (pthread_mutex_t *mutex) {
    printf("pthread_mutex_trylock(%p)\n", mutex);
    return 0;
}


int
pthread_mutex_unlock(pthread_mutex_t *mutex) {
    printf("pthread_mutex_unlock(%p)\n", mutex);
    return 0;
}


int
pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    printf("pthread_mutexattr_init(%p)\n", attr);
    //    memset(attr, 0, sizeof(pthread_mutexattr_t));
    return 0;
}


int
pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind) {
    printf("pthread_mutexattr_settype(%p, %d)\n", attr, kind);
    return 0;
}


int
pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    printf("pthread_mutexattr_destroy(%p)\n", attr);
    return 0;
}


int
pthread_cond_init(pthread_cond_t *restrict cond, const /*pthread_condattr_t*/void *restrict cond_attr) {
    printf("pthread_cond_init(%p,%p)\n", cond, cond_attr);
    //    memset(cond, 0, sizeof(pthread_cond_t));
    return 0;
}


int
pthread_cond_destroy(pthread_cond_t *restrict cond) {
    printf("pthread_cond_destroy(%p)\n", cond);
    return 0;
}


int
pthread_cond_signal(pthread_cond_t *restrict cond) {
    printf("pthread_cond_signal(%p)\n", cond);
    return 0;
}


int
pthread_cond_broadcast(pthread_cond_t *restrict cond) {
    printf("pthread_cond_broadcast(%p)\n", cond);
    return 0;
}


int
pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex) {
    printf("pthread_cond_wait(%p, %p)\n", cond, mutex);
    return 0;
}


int
pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const /*pthread_rwlockattr_t*/void *restrict attr) {
    printf("pthread_rwlock_init(%p, %p)\n", rwlock, attr);
    return 0;
}


int
pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
    printf("pthread_rwlock_destroy(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
    printf("pthread_rwlock_tryrdlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
    printf("pthread_rwlock_rdlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
    printf("pthread_tryrwlock_wrlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
    printf("pthread_rwlock_wrlock(%p)\n", rwlock);
    return 0;
}


int
pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
    printf("pthread_rwlock_unlock(%p)\n", rwlock);
    return 0;
}


pthread_t
pthread_self(void) {
    printf("pthread_self()\n");
    return 1;
}


// Used as a weak symbol to detect libpthread
int
__pthread_key_create (pthread_key_t *key,  void (*destructor) (void *)) {
    printf("pthread_key_create(%p,%p)\n", key, destructor);
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
    printf("swift_oncef(%p, %p, %p) [%lu]\n", predicate, function, context,
           *predicate);
    
    if (!*predicate) {
        *predicate = 1;
        function(context);
    }
}

