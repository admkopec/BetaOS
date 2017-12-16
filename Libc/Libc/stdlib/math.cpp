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
    if (n >= 0) {
        return n;
    } else {
        return -n;
    }
}

long
labs(long n) {
    if (n >= 0) {
        return n;
    } else {
        return -n;
    }
}

float
sqrtf(float f) {
    float res;
    __m128 in = _mm_load_ss(&f);
    _mm_store_ss(&res, _mm_mul_ss(in, _mm_rsqrt_ss(in)));
    return res;
}

float ceilf(float f) {
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
