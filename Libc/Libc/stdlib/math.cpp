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

double
ceil(double d) {
    long result = (long)d;
    if ((double)result < d) {
        if (d >= 0) {
            result++;
        } else {
            result--;
        }
    }
    double resultd = (double)result;
    return resultd;
}

float
ceilf(float f) {
    long result = (long)f;
    if ((float)result < f) {
        if (f >= 0) {
            result++;
        } else {
            result--;
        }
    }
    float resultf = (float)result;
    return resultf;
}

long double
ceill(long double d) {
    long result = (long)d;
    if ((long double)result < d) {
        if (d >= 0) {
            result++;
        } else {
            result--;
        }
    }
    long double resultd = (long double)result;
    return resultd;
}

double
floor(double d) {
    long result = (long)d;
    if ((double)result > d) {
        if (d >= 0) {
            result--;
        } else {
            result++;
        }
    }
    double resultd = (double)result;
    return resultd;
}

float
floorf(float f) {
    long result = (long)f;
    if ((float)result > f) {
        if (f >= 0) {
            result--;
        } else {
            result++;
        }
    }
    float resultf = (float)result;
    return resultf;
}

long double
floorl(long double d) {
    long result = (long)d;
    if ((long double)result > d) {
        if (d >= 0) {
            result--;
        } else {
            result++;
        }
    }
    long double resultd = (long double)result;
    return resultd;
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

double
pow(double x, double y) {
    return 0;
}

float
powf(float a, float b) {
    return 0;
}
