//
//  printf.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

extern "C" { extern void vsputc(int ch);
             extern bool experimental;
             extern bool use_screen_caching;
             extern void refresh_screen(void);
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
