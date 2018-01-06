//
//  unimplemented.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 11/28/17.
//

#define UNIMPLEMENTED(x) void x() { ULTDBG(__func__); }

#ifdef ULTDEBUG
#define ULTDBG(x...) printf(x)
#else
#define ULTDBG(x...)
#endif

int _DefaultRuneLocale;

UNIMPLEMENTED(getline)
UNIMPLEMENTED(getsectiondata)
UNIMPLEMENTED(__error)
UNIMPLEMENTED(malloc_zone_from_ptr)
UNIMPLEMENTED(snprintf_l)
UNIMPLEMENTED(vdprintf)
UNIMPLEMENTED(fprintf)
UNIMPLEMENTED(dlsym)
UNIMPLEMENTED(dladdr)
UNIMPLEMENTED(close)
UNIMPLEMENTED(read)
UNIMPLEMENTED(asl_log)
UNIMPLEMENTED(__swbuf)
UNIMPLEMENTED(_tlv_bootstrap)
UNIMPLEMENTED(malloc_default_zone)
UNIMPLEMENTED(getenv)

/*
 * Math functions
 */

UNIMPLEMENTED(arc4random)
UNIMPLEMENTED(arc4random_uniform)
UNIMPLEMENTED(cos)
UNIMPLEMENTED(cosf)
UNIMPLEMENTED(exp)
UNIMPLEMENTED(exp2)
UNIMPLEMENTED(exp2f)
UNIMPLEMENTED(expf)
UNIMPLEMENTED(fma)
UNIMPLEMENTED(fmaf)
UNIMPLEMENTED(fmal)
UNIMPLEMENTED(fmod)
UNIMPLEMENTED(fmodf)
UNIMPLEMENTED(fmodl)
UNIMPLEMENTED(log)
UNIMPLEMENTED(log10)
UNIMPLEMENTED(log10f)
UNIMPLEMENTED(log2)
UNIMPLEMENTED(log2f)
UNIMPLEMENTED(logf)
UNIMPLEMENTED(nearbyint)
UNIMPLEMENTED(nearbyintf)
UNIMPLEMENTED(sin)
UNIMPLEMENTED(sinf)
UNIMPLEMENTED(lgamma_r)
UNIMPLEMENTED(lgammaf_r)
UNIMPLEMENTED(lgammal_r)

/*
 * Misc
 */

UNIMPLEMENTED(strtod_l)
UNIMPLEMENTED(strtof_l)
UNIMPLEMENTED(strtold_l)
UNIMPLEMENTED(remainder)
UNIMPLEMENTED(remainderf)
UNIMPLEMENTED(remainderl)
UNIMPLEMENTED(__memcpy_chk)
UNIMPLEMENTED(__memmove_chk)
UNIMPLEMENTED(__strlcpy_chk)

UNIMPLEMENTED(sysconf)
UNIMPLEMENTED(trunc)
UNIMPLEMENTED(truncf)
UNIMPLEMENTED(truncl)

UNIMPLEMENTED(fcntl)
UNIMPLEMENTED(ioctl)

UNIMPLEMENTED(open)
UNIMPLEMENTED(openat)
UNIMPLEMENTED(sem_open)

UNIMPLEMENTED(__fpclassifyd)
UNIMPLEMENTED(__fpclassifyf)
