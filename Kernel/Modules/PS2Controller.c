//
//  PS2Controller.c
//  BetaOS
//
//  Created by Adam Kopeć on 2/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "PS2Controller.h"
#include "PS2Keymap.h"
#include <stdbool.h>
#include <i386/pio.h>
#include <stdio.h>

bool shifted=false;
bool capsed=false;
bool e0ed=false;

void updateLEDs() {
    uint8_t state = ((capsed << 2) | (0 << 1) | 0);
    uint8_t i;
    int j = 0;
    do {
        i = inb(0x64);
        j++;
    } while ((i & 0x02) && j < 1000);
    
    if (j == 1000) {
        return;
    }
    
    outb(0x60, 0xED);
    
    i = 0;
    j = 0;
    do {
        i = inb(0x60);
        j++;
    } while ((i != 0xFA) && j < 1000);
    
    if (j == 1000) {
        return;
    }

    i = 0;
    do {
        i = inb(0x64);
    } while (i & 0x02);
    
    outb(0x60, state);
    
    i = 0;
    do {
        i = inb(0x60);
    } while (i != 0xFA);
}

int pollchar() {
    int c;
    if (inb(0x64)&(1 << 0)) {
        c=inb(0x60);
        if (c==0xE0) {
            e0ed=true;
        } else if (c > 0x53) {
            /* Unsupported */
        } else if (e0ed) {
            e0ed = false;
            return keymap[c][2];
        }
        if (c==0x3A) {
            if (!capsed) {
                capsed=true;
            } else {
                capsed=false;
            }
            updateLEDs();
        }
        if (capsed) {
            if (c==0x2A||c==0x36||shifted) {
                shifted=true;
                c=inb(0x60);
                if (c!=0xAA&&c!=0xB6) {
                    return keymap[c][0];
                } else {
                    shifted=false;
                }

            }
            if ((c>=0x10&&c<=0x19)||(c>=0x1E&&c<=0x26)||(c>=0x2C&&c<=0x32)) {
                return keymap[c][1];
            } else {
                return keymap[c][0];
            }
        }
        if (c==0x2A||c==0x36||shifted==true) {
            shifted=true;
            c=inb(0x60);
            if (c==0xAA||c==0xB6) {
                shifted=false;
                return keymap[c][0];
            }
            return keymap[c][1];
        }
        return keymap[c][0];
    }
    return false;
}
