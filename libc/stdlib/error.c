//
//  error.c
//  OS
//
//  Created by Adam Kopeć on 1/16/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void error(const char* errormsg) {
    read_rtc();
    printf("\nError: %d:%d:%d %d/%d/%d\n%s\n", hour, minute, second, day, month, year, errormsg);
}
