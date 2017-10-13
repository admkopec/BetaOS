//
//  putchar.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
//#include <kernel/tty.h>

extern void vsputc(int ch);
extern bool experimental;
extern bool use_screen_caching;
extern void refresh_screen(void);
extern void panic(const char*, ...);

int putchar(int ic)
{
    if ((ic<=0xFF&&ic>=0x20)||ic=='\n'||ic=='\t'||ic=='\b'||ic=='\r') {
        char c = (char) ic;
        //serial_putc(c);
        vsputc(c);
    }
    if (experimental && use_screen_caching) {
        refresh_screen();
    }
	return ic;
}

int
fputc(int ch, void *stream) {
    if (stream != stderr && stream != stdout) {
        panic("putc stream = %p", stream);
    } else {
        return putchar(ch);
    }
    return -1;
}

int
putc(int ch, void *stream) {
    if (stream != stderr && stream != stdout) {
        panic("putc stream = %p", stream);
    } else {
        return putchar(ch);
    }
    return -1;
}

int fputs(const char* ch, void *stream) {
    if (stream != stderr && stream != stdout) {
        panic("puts stream = %p", stream);
    } else {
        return puts(ch);
    }
    return -1;
}
