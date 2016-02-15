//
//  gets.c
//  BetaOS
//
//  Created by Adam Kopeć on 2/14/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>

char *gets(char *buf)
{
    char *p = buf;
    int ch;
    
    while (1) {
        ch = getchar();
        
        if (ch=='\b') {
            if (p>buf) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                p--;
            }
        }
        
        if (ch == '\r' || ch =='\n' || ch >= ' ' || ch=='\t') {
            putchar(ch);
            if (ch == '\r') putchar('\n');
            if (ch == '\n' || ch == '\r') break;
            *p++ = ch;
        }
    }
    
    *p = 0;
    return buf;
}