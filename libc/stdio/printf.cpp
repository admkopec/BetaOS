//
//  printf.cpp
//  OS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int printf(const char *s, ...) {
    va_list ap;
    int printedchars=0;

    char buf[16];
    int i, j, size, buflen, neg;

    unsigned char c;
    int ival;
    unsigned int uival;

    va_start(ap, s);

    while ((c = *s++)) {
        size = 0;
        neg = 0;

        if (c == 0)
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
                for (i = size, j = buflen; i >= 0;
                     i--, j--)
                buf[i] =
                (j >=
                 0) ? buf[j] : '0';

                if (neg)
                printf("-%s", buf);
                else
                printf(buf);
            }
            else if (c == 'u') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 10);

                buflen = strlen(buf);
                if (buflen < size)
                for (i = size, j = buflen; i >= 0;
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
                for (i = size, j = buflen; i >= 0;
                     i--, j--)
                buf[i] =
                (j >=
                 0) ? buf[j] : '0';

                printf("0x%s", buf);
            } else if (c == 'p') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 16);
                size = 8;

                buflen = strlen(buf);
                if (buflen < size)
                for (i = size, j = buflen; i >= 0;
                     i--, j--)
                buf[i] =
                (j >=
                 0) ? buf[j] : '0';

                printf("0x%s", buf);
            } else if (c == 's') {
                printf((char *) va_arg(ap, int));
            }
        } else
        putchar(c);
        printedchars++;
    }
    return printedchars;
}
