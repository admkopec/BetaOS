//
//  printf.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
int  __doprnt(const char *fmt, va_list argp, void (*putc)(int), int radix, int is_log);
struct string_buf {
    char *data;
    size_t count;
    size_t max_len;
};
}

int
vasprintf(char **strp, const char * format, va_list argp) {
    char buf[128];
    
    // len = number of characters in final string, not that fit in buffer
    // excludes terminating '\0'
    int len = vsnprintf(buf, 128, format, argp);
    *strp = (char *)malloc(len + 1);
    if (*strp == NULL) {
        return -1;
    }
    
    if (len > 127) {
        return vsnprintf(buf, len + 1, format, argp);
    } else {
        memcpy(*strp, buf, len + 1);
    }
    
    return len;
}

int
asprintf(char **strp, const char * format, ...) {
    char buf[2048];
    // FIXME: use the size
    
    va_list argp;
    va_start(argp, format);
    vsnprintf(buf, 2048, format, argp);
    va_end(argp);
    
    int len = (int)strlen(buf);
    char *result = (char *)malloc(len);
    memcpy(result, buf, len+1);
    *strp = result;
    
    return len;
}

void* buf_p = NULL;

static void
b_print_char(int ch) {
    string_buf *buf = (string_buf *)buf_p;
    if (buf->count+1 < buf->max_len) {
        *(buf->data + buf->count) = (char)ch;
        buf->count++;
        *(buf->data + buf->count) = '\0';
    }
}

int
snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, size, fmt, args);
    va_end(args);
    
    return len;
}

int
vsprintf(char *buf, const char *fmt, va_list args) {
    return vsnprintf(buf, 2048, fmt, args);
}

int
vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    struct string_buf string_buf = { .data = buf, .count = 0, .max_len = size };
    if (size < 1) {
        return 0;
    }
    *buf = '\0';
    
    buf_p = &string_buf;
    
    __doprnt(fmt, args, b_print_char, 16, false);
    return (int)size;
}
