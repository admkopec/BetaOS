//
//  printf.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" { extern void vsputc(int ch);
             extern bool experimental;
             extern bool use_screen_caching;
             extern void refresh_screen(void);
             extern int  kvsnprintf(char *buf, size_t size, const char *fmt, va_list args);
             extern void panic(const char* fmt, ...);
}

size_t
write(int fd, const void *buf, size_t nbyte) {
    //    printf("write(fd=%d, buf=%p nbyte=%lu\n", fd, buf, nbyte);
    
    if (fd == 1 || fd == 2) {
        printf((const char *)buf);
    } else {
        panic("write() with fd = %d\n", fd);
    }
    return nbyte;
}

int
vasprintf(char **strp, const char * format, va_list argp) {
    char buf[128];
    
    // len = number of characters in final string, not that fit in buffer
    // excludes terminating '\0'
    int len = kvsnprintf(buf, 128, format, argp);
    *strp = (char *)malloc(len + 1);
    if (*strp == NULL) {
        return -1;
    }
    
    if (len > 127) {
        return kvsnprintf(buf, len + 1, format, argp);
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
    kvsnprintf(buf, 2048, format, argp);
    va_end(argp);
    
    int len = (int)strlen(buf);
    char *result = (char *)malloc(len);
    memcpy(result, buf, len+1);
    *strp = result;
    
    return len;
}


int
snprintf(char * buf, __unused size_t size, __unused const char * format, ...) {
    // FIXME: use the size
//    printf("snprintf(%s)=", format);
//    va_list argp;
//    ksprintf(buf, format, argp);
//    va_end(argp);
//    print_string(buf);
    
    return (int)strlen(buf);
}

int printf(const char *s, ...) {
    va_list ap;
    int printedchars=0;

    char buf[16];
    size_t buflen, size;
    int i, j, neg;

    unsigned char c;
    int ival;
    unsigned int uival;

    va_start(ap, s);

    while ((c = *s++)) {
        size = 0;
        neg = 0;

        if (c == 0x00)
            break;
        else if (c == '%') {
            c = *s++;
            if (c >= '0' && c <= '9') {
                size = c - '0';
                c = *s++;
            }

            if (c == 'd') {
                ival = va_arg(ap, int);
                if (ival < 0) {
                    uival = 0 - ival;
                    neg++;
                } else
                uival = ival;
                itoa(buf, uival, 10);

                buflen = strlen(buf);
                if (buflen < size)
                for (i = (int) size, j = (int) buflen; i >= 0;
                     i--, j--)
                buf[i] = (j >= 0) ? buf[j] : '0';

                if (neg)
                printf("-%s", buf);
                else
                printf(buf);
            }
            else if (c == 'u') {
                uival = va_arg(ap, unsigned);
                itoa(buf, uival, 10);

                buflen = strlen(buf);
                if (buflen < size)
                for (i = (int) size, j = (int) buflen; i >= 0;
                     i--, j--)
                buf[i] =
                (j >=
                 0) ? buf[j] : '0';

                printf(buf);
            } else if (c == 'x' || c == 'X') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 16);

                buflen = strlen(buf);
                if (buflen < size)
                for (i = (int) size, j = (int) buflen; i >= 0;
                     i--, j--)
                buf[i] =
                (j >=
                 0) ? buf[j] : '0';
                if (c == 'X') {
                    printf("%s", buf);
                } else {
                    printf("0x%s", buf);
                }
            } else if (c == 'p') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 16);
                size = 8;

                buflen = strlen(buf);
                if (buflen < size)
                for (i = (int) size, j = (int) buflen; i >= 0;
                     i--, j--)
                buf[i] = (j >= 0) ? buf[j] : '0';

                printf("0x%s", buf);
            } else if (c == 's') {
                printf((char *) va_arg(ap, char *));
            }
        } else if (c > 0xC0) {
            if (c == 0xC2) {
                c = *s++;
                vsputc(c);
            } else if (c == 0xC3) {
                c = *s++;
                c |= 0xC0;
                vsputc(c);
            } else {
                continue;
            }
        } else
            vsputc(c);
        printedchars++;
    }
    if (experimental && use_screen_caching) {
        refresh_screen();
    }
    return printedchars;
}
