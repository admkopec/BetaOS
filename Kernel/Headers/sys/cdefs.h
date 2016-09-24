//
//  cdefs.h
//  BetaOS
//
//  Created by Adam Kopeć on 5/10/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef cdefs_h
#define cdefs_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <kernel/kalloc.h>
#include <assert.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif  /* MAX */

#define RB_ENTRY(type)                              \
struct {                                            \
struct type *rbe_left;		/* left element */		\
struct type *rbe_right;		/* right element */		\
struct type *rbe_parent;	/* parent element */    \
}

#define	__probable(x)	__builtin_expect(!!((long)(x)), 1L)
#define	__improbable(x)	__builtin_expect(!!((long)(x)), 0L)

#define __unused        __attribute__((unused))

#endif /* cdefs_h */
