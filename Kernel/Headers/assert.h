//
//  assert.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/29/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef assert_h
#define assert_h

void Assert(const char* file,
            int         line,
            const char* expression) __attribute__((noinline));

#define assert(ex)  (__builtin_expect(!!((long)(ex)), 1L) ? (void)0 : Assert(__FILE__, __LINE__, # ex))

#endif /* assert_h */
