//
//  error.c
//  BetaOS
//
//  Created by Adam Kopeć on 1/16/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void error(const char* errormsg) {
    gettime();
    kprintf("  %d:%02d:%02d %s %d/%d/%d\n  ERROR: %s\n", hour, minute, second, pmam, month, day, year, errormsg);
}
