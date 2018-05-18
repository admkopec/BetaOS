//
//  putchar.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015-2018 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern void vsputc(int ch);
extern void serial_putc(int c);
extern bool early;
extern void panic(const char*, ...);

int putchar(int ic) {
    if ((ic<=0xFF&&ic>=0x20)||ic=='\n'||ic=='\t'||ic=='\b'||ic=='\r') {
        char c = (char) ic;
        if (early) {
            serial_putc(c);
        } else {
            vsputc(c);
        }
    }
	return ic;
}

int
fputc(int ch, FILE *stream) {
    if (stream != stderr && stream != stdout) {
        panic("putc stream = %p", stream);
    } else {
        return putchar(ch);
    }
    return -1;
}

int
putc(int ch, FILE *stream) {
    if (stream != stderr && stream != stdout) {
        panic("putc stream = %p", stream);
    } else {
        return putchar(ch);
    }
    return -1;
}

int fputs(const char* ch, FILE *stream) {
    if (stream != stderr && stream != stdout) {
        panic("puts stream = %p", stream);
    } else {
        return puts(ch);
    }
    return -1;
}
