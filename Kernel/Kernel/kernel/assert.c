//
//  assert.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/1/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <assert.h>
#include <kernel/misc_protos.h>

void __assert_rtn(const char * msg, const char * file, int line, const char * expression) {
    // Implement properly!
    panic("(%s:%d) Assertion failed %s %s", file, line, expression, msg);
    for (; ;) { }
}
