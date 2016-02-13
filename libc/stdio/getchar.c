//
//  getchar.c
//  OS
//
//  Created by Adam Kopeć on 2/9/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <PS2Controller.h>
#include <kernel/interrupts.h>

/*int getchar(void) {
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
}*/

bool shifted;

int getchar() {
    int c;
    if (inb(0x64)&(1 << 0)) {
        c=inb(0x60);
        if (c==0xAA||c==0xB6) {
            shifted=false;
        }
        if (c==0x2A||c==0x36||shifted==true) {
            shifted=true;
            do {
                c=inb(0x60);
                if (c==0xAA||c==0xB6) {
                    shifted=false;
                    c=inb(0x60);
                    return keymap[c][0];
                }
                if (c!=0&&c<=0x53)
                    return keymap[c][1];
            } while (1);
        }
        if (c==0xE0) {
            do {
                c=inb(0x60);
                if (c!=0&&c<=0x53)
                    return keymap[c][2];
            } while (1);
        }
        if (c!=0&&c<=0x53)
            return keymap[c][0];
    }
}