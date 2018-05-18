//
//  itoa.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef ULONG_MAX
#define    ULONG_MAX    ((unsigned long)(~0L))        /* 0xFFFFFFFF */
#endif

#ifndef LONG_MAX
#define    LONG_MAX    ((long)(ULONG_MAX >> 1))    /* 0x7FFFFFFF */
#endif

#ifndef LONG_MIN
#define    LONG_MIN    ((long)(~LONG_MAX))        /* 0x80000000 */
#endif

static inline int
isupper(char c) {
    return (c >= 'A' && c <= 'Z');
}

static inline int
isalpha(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}


static inline int
isspace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}

static inline int
isdigit(char c) {
    return (c >= '0' && c <= '9');
}

extern "C" {
void itoa(char *buf, unsigned long int n, int base) {
    unsigned long int tmp = n;
    int i = 0, j;

    do {
        tmp = n % base;
        buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
    } while (n /= base);
    buf[i--] = 0;

    for (j = 0; j < i; j++, i--) {
        tmp = buf[j];
        buf[j] = buf[i];
        buf[i] = tmp;
    }
}
}

long
strtol(const char * nptr, char ** endptr, int base) {
    const char *s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;
    
    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
        if ((base == 0 || base == 16) &&
            c == '0' && (*s == 'x' || *s == 'X')) {
            c = s[1];
            s += 2;
            base = 16;
        } else if ((base == 0 || base == 2) &&
                   c == '0' && (*s == 'b' || *s == 'B')) {
            c = s[1];
            s += 2;
            base = 2;
        }
    if (base == 0)
        base = c == '0' ? 8 : 10;
        
    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
        cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
        cutlim = (int)(cutoff % (unsigned long)base);
        cutoff /= (unsigned long)base;
        for (acc = 0, any = 0;; c = *s++) {
            if (isdigit(c))
                c -= '0';
                else if (isalpha(c))
                    c -= isupper(c) ? 'A' - 10 : 'a' - 10;
                    else
                        break;
            if (c >= base)
                break;
            if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
                any = -1;
                else {
                    any = 1;
                    acc *= base;
                    acc += c;
                }
        }
    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
    } else if (neg)
        acc = -acc;
        if (endptr != 0)
            *endptr = (char *)(any ? s - 1 : nptr);
            return (acc);
}

/*
 * Convert a string to an unsigned long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
unsigned long
strtoul(const char * nptr, char ** endptr, int base) {
    const char *s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;
    
    /*
     * See strtol for comments as to the logic used.
     */
    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
        if ((base == 0 || base == 16) &&
            c == '0' && (*s == 'x' || *s == 'X')) {
            c = s[1];
            s += 2;
            base = 16;
        } else if ((base == 0 || base == 2) &&
                   c == '0' && (*s == 'b' || *s == 'B')) {
            c = s[1];
            s += 2;
            base = 2;
        }
    if (base == 0)
        base = c == '0' ? 8 : 10;
        cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
        cutlim = (int)((unsigned long)ULONG_MAX % (unsigned long)base);
        for (acc = 0, any = 0;; c = *s++) {
            if (isdigit(c))
                c -= '0';
                else if (isalpha(c))
                    c -= isupper(c) ? 'A' - 10 : 'a' - 10;
                    else
                        break;
            if (c >= base)
                break;
            if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
                any = -1;
                else {
                    any = 1;
                    acc *= base;
                    acc += c;
                }
        }
    if (any < 0) {
        acc = ULONG_MAX;
    } else if (neg)
        acc = -acc;
        if (endptr != 0)
            *endptr = (char *)(any ? s - 1 : nptr);
            return (acc);
}
