//
//  bootargs.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/27/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <platform/platform.h>
#include <platform/boot.h>
#include <i386/vm_types.h>

// Portions Copyright Apple, Inc

struct i24 {
    int32_t	i24 : 24;
    int32_t _pad : 8;
};

#define	NUM	0
#define	STR	1

static bool
isargsep(char c) {
    if (c == ' ' || c == '\0' || c == '\t')
        return (true);
    else
        return (false);
}

static int
argstrlcpy(char *from, char *to, unsigned maxlen) {
    unsigned int i = 0;
    while (!isargsep(*from) && i < maxlen) {
        i++;
        *to++ = *from++;
    }
    *to = 0;
    return(i);
}

static int argnumcpy(long long val, void *to, unsigned maxlen) {
    switch (maxlen) {
        case 0:
            /* No write-back, caller just wants to know if arg was found */
            break;
        case 1:
            *(int8_t *)to = val;
            break;
        case 2:
            *(int16_t *)to = val;
            break;
        case 3:
            /* Unlikely in practice */
            ((struct i24 *)to)->i24 = (int32_t) val;
            break;
        case 4:
            *(int32_t *)to = (int32_t) val;
            break;
        case 8:
            *(int64_t *)to = val;
            break;
        default:
            *(int32_t *)to = (int32_t) val;
            maxlen = 4;
            break;
    }
    return (int)maxlen;
}

static int getval(char *s, long long *val, bool skip_equal_sign);

bool
Parse_boot_argn(const char *arg_string, void *arg_ptr, int max_len) {
    char *args;
    char *cp, c;
    uintptr_t i;
    long long val;
    boolean_t arg_bool;
    boolean_t arg_found;
    
    args = Platform_state.bootArgs->CommandLine;
    if (*args == '\0')
        return false;
    arg_found = false;
    
    while(*args && isargsep(*args))
        args++;
    
    while (*args) {
        if (*args == '-')
            arg_bool = true;
        else
            arg_bool = false;
        
        cp = args;
        while (!isargsep(*cp) && *cp != '=')
            cp++;
        if (*cp != '=' && !arg_bool)
            goto gotit;
        
        c = *cp;
        
        i = cp-args;
        if (strncmp(args, arg_string, i) || (i!=strlen(arg_string)))
            goto gotit;
        if (arg_bool) {
            argnumcpy(1, arg_ptr, max_len);
            arg_found = true;
            break;
        } else {
            while (*cp && isargsep (*cp))
                cp++;
            if (*cp == '=' && c != '=') {
                args = cp+1;
                goto gotit;
            }
            if ('_' == *arg_string) { /* Force a string copy if the argument name begins with an underscore */
                int hacklen = 17 > max_len ? 17 : max_len;
                argstrlcpy(++cp, (char *)arg_ptr, hacklen - 1); /* Hack - terminate after 16 characters */
                arg_found = true;
                break;
            }
            switch (getval(cp, &val, false)) {
                case NUM:
                    argnumcpy(val, arg_ptr, max_len);
                    arg_found = true;
                    break;
                case STR:
                    if(max_len > 0) //max_len of 0 performs no copy at all
                        argstrlcpy(++cp, (char *)arg_ptr, max_len - 1);
                    arg_found = true;
                    break;
            }
            goto gotit;
        }
    gotit:
        /* Skip over current arg */
        while(!isargsep(*args)) args++;
        
        /* Skip leading white space (catch end of args) */
        while(*args && isargsep(*args)) args++;
    }
    return(arg_found);
}

static int
getval(char *s, long long *val, bool skip_equal_sign) {
    unsigned long long radix, intval;
    unsigned char c;
    int  sign = 1;
    bool has_value = false;
    if (*s == '=') {
        s++;
        has_value = true;
    }
    if (has_value || skip_equal_sign) {
        if (*s == '-')
            sign = -1, s++;
        intval = *s++-'0';
        radix = 10;
        if (intval == 0) {
            switch(*s) {
                case 'x':
                    radix = 16;
                    s++;
                    break;
                    
                case 'b':
                    radix = 2;
                    s++;
                    break;
                case '0': case '1': case '2': case '3':
                case '4': case '5': case '6': case '7':
                    intval = *s-'0';
                    s++;
                    radix = 8;
                    break;
                default:
                    if (!isargsep(*s))
                        return (STR);
            }
        } else if (intval >= radix) {
            return (STR);
        }
        for(;;) {
            c = *s++;
            if (isargsep(c))
                break;
            if ((radix <= 10) && ((c >= '0') && (c <= ('9' - (10 - radix))))) {
                c -= '0';
            } else if ((radix == 16) && ((c >= '0') && (c <= '9'))) {
                c -= '0';
            } else if ((radix == 16) && ((c >= 'a') && (c <= 'f'))) {
                c -= 'a' - 10;
            } else if ((radix == 16) && ((c >= 'A') && (c <= 'F'))) {
                c -= 'A' - 10;
            } else if (c == 'k' || c == 'K') {
                sign *= 1024;
                break;
            } else if (c == 'm' || c == 'M') {
                sign *= 1024 * 1024;
                break;
            } else if (c == 'g' || c == 'G') {
                sign *= 1024 * 1024 * 1024;
                break;
            } else {
                return (STR);
            }
            if (c >= radix)
                return (STR);
            intval *= radix;
            intval += c;
        }
        if (!isargsep(c) && !isargsep(*s))
            return STR;
        *val = intval * sign;
        return (NUM);
    }
    *val = 1;
    return (NUM);
}
