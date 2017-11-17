//
//  abort.c
//  BetaOS
//
//  Created by Adam Kopeć on 1/16/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdlib.h>
#include <kernel/misc_protos.h>

void
abort() {
    panic("Abort called");
    for (; ;) { }
}
