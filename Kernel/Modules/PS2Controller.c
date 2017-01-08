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

bool shifted=false;
bool capsed=false;
bool e0ed=false;

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
