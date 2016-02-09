//
//  getchar.c
//  OS
//
//  Created by Adam Kopeć on 2/9/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>

int getchar(void) {
    int c;
    do {
        if(inb(0x60)!=c) {
            c=inb(0x60);
            if(c>0)
                break;
        }
    } while(1);
    //return keymap[c];
    return c;
}