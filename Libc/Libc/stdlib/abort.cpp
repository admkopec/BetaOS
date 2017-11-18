//
//  abort.c
//  BetaOS
//
//  Created by Adam Kopeć on 1/16/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdlib.h>

extern "C" void panic(const char *, ...);

void
abort() {
    panic("Abort called");
    for (; ;) { }
}
