//
//  math.cpp
//  Libc
//
//  Created by Adam Kopeć on 12/10/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <math.h>

float fabsf(float x) {
    return ((x < 0) ? -x : x);
}

double fabs(double x) {
    return ((x < 0) ? -x : x);
}

long double fabsl(long double x) {
    return ((x < 0) ? -x : x);
}

int
abs(int n) {
    return (n < 0 ? -n : n);
}

long
labs(long n) {
    return (n < 0 ? -n : n);
}

long long
llabs(long long n) {
    return (n < 0 ? -n : n);
}

double
round(double d) {
    return (double)((long int)(d));
}

long double
roundl(long double d) {
    return (long double)((long int)(d));
}

float
roundf(float f) {
    return (float)((long int)(f));
}

long int
lround(double d) {
    return (long int)(d);
}

long int
lroundf(float f) {
    return (long int)(f);
}

long int
lroundl(long double d) {
    return (long int)(d);
}



double
sqrt(double d) {
    __m128d in = _mm_load_sd(&d);
    in = __builtin_ia32_sqrtsd(in);
    return ({ double _d; _mm_store_sd( &_d, in ); /*return*/ _d; });
}

float
sqrtf(float f) {
    float res;
    __m128 in = _mm_load_ss(&f);
    _mm_store_ss(&res, _mm_mul_ss(in, _mm_rsqrt_ss(in)));
    return res;
}

float expf(float x) {
    return __builtin_expf(x);
}

double exp(double x) {
    return __builtin_exp(x);
}

long double expl(long double x) {
    return __builtin_expl(x);
}

float powf(float b, float e) {
    return expf(e * logf(b));
}

double pow(double b, double e) {
    return exp(e * log(b));
}

long double powl(long double b, long double e) {
    return expl(e * logl(b));
}

float logf(float x) {
    return __builtin_logf(x);
}

double log(double x) {
    return __builtin_log(x);
}

long double logl(long double x) {
    return __builtin_logl(x);
}

double
ceil(double d) {
    modf(d, &d);
    if (d > 0.0) d += 1.0;
    return d;
}

float
ceilf(float f) {
    modff(f, &f);
    if (f > 0.0) f += 1.0;
    return f;
}

long double
ceill(long double d) {
    modfl(d, &d);
    if (d > 0.0) d += 1.0;
    return d;
}

double
floor(double d) {
    modf(d, &d);
    if (d < 0.0) d -= 1.0;
    return d;
}

float
floorf(float f) {
    modff(f, &f);
    if (f < 0.0) f -= 1.0;
    return f;
}

long double
floorl(long double d) {
    modfl(d, &d);
    if (d < 0.0) d -= 1.0;
    return d;
}

float modff(float x, float *iptr) {
    if (fabsf(x) >= 8388608.0) {
        *iptr = x;
        return 0.0;
    }
    else if (fabs(x) < 1.0) {
        *iptr = 0.0;
        return x;
    }
    else {
        *iptr = (float) (int) x;
        return (x - *iptr);
    }
}

double modf(double x, double *iptr) {
    if (fabs(x) >= 4503599627370496.0) {
        *iptr = x;
        return 0.0;
    }
    else if (fabs(x) < 1.0) {
        *iptr = 0.0;
        return x;
    }
    else {
        *iptr = (double) (int64_t) x;
        return (x - *iptr);
    }
}

long double modfl(long double x, long double *iptr) {
    if (fabs(x) >= 4503599627370496.0) {
        *iptr = x;
        return 0.0;
    }
    else if (fabs(x) < 1.0) {
        *iptr = 0.0;
        return x;
    }
    else {
        *iptr = (long double) (int64_t) x;
        return (x - *iptr);
    }
}

float fmodf(float x, float div) {
    float n0;

    x /= div;
    x = modff(x, &n0);
    x *= div;

    return x;
}

double fmod(double x, double div) {
    double n0;

    x /= div;
    x = modf(x, &n0);
    x *= div;

    return x;
}

long double fmodl(long double x, long double div) {
    long double n0;

    x /= div;
    x = modfl(x, &n0);
    x *= div;

    return x;
}

double
rint(double d) {
    double d_orig;
    double r;
    if (isnan(d)) {
        return d;
    }
    if (d <= 0.0) {
        if (d == 0.0) {
            return d;
        }
        
        d_orig = d;
        d += 0.5;
        if (d >= 0.0) {
            return -0.0;
        }
        if (d == d_orig + 1.0) {
            return d_orig;
        }
        
        r = floor(d);
        
        if (r != d) {
            return r;
        }
        
        return floor(d * 0.5) * 2.0;
    } else {
        d_orig = d;
        d -= 0.5;
        if (d <= 0.0) {
            return 0.0;
        }
        if (d == d_orig - 1.0) {
            return d_orig;
        }
        r = ceil(d);
        if (r != d) {
            return r;
        }
        return ceil(d * 0.5) * 2.0;
    }
}

float
rintf(float f) {
    float f_orig;
    float r;
    if (isnan(f)) {
        return f;
    }
    if (f <= 0.0) {
        if (f == 0.0) {
            return f;
        }
        
        f_orig = f;
        f += 0.5;
        if (f >= 0.0) {
            return -0.0;
        }
        if (f == f_orig + 1.0) {
            return f_orig;
        }
        
        r = floorf(f);
        
        if (r != f) {
            return r;
        }
        
        return floorf(f * 0.5) * 2.0;
    } else {
        f_orig = f;
        f -= 0.5;
        if (f <= 0.0) {
            return 0.0;
        }
        if (f == f_orig - 1.0) {
            return f_orig;
        }
        r = ceilf(f);
        if (r != f) {
            return r;
        }
        return ceilf(f * 0.5) * 2.0;
    }
}

long double
rintl(long double d) {
    long double d_orig;
    long double r;
    if (isnan(d)) {
        return d;
    }
    if (d <= 0.0) {
        if (d == 0.0) {
            return d;
        }
        
        d_orig = d;
        d += 0.5;
        if (d >= 0.0) {
            return -0.0;
        }
        if (d == d_orig + 1.0) {
            return d_orig;
        }
        
        r = floorl(d);
        
        if (r != d) {
            return r;
        }
        
        return floorl(d * 0.5) * 2.0;
    } else {
        d_orig = d;
        d -= 0.5;
        if (d <= 0.0) {
            return 0.0;
        }
        if (d == d_orig - 1.0) {
            return d_orig;
        }
        r = ceill(d);
        if (r != d) {
            return r;
        }
        return ceill(d * 0.5) * 2.0;
    }
}
