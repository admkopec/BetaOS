//
//  strlen.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <string.h>
//#include <i386/cpuid.h>
//#define _HBit(n)        (1ULL << ((n)+32))
//#define CPUID_FEATURE_SSE4_2    _HBit(20) /* Streaming SIMD extensions 4.2 */
//extern "C" { uint64_t cpuid_features(); }

size_t strlen(const char* string)
{
    size_t result = 0;
//    if (cpuid_features() & CPUID_FEATURE_SSE4_2) {
//        __asm__(" mov $-16, %0;                      "
//            " pxor %%xmm0, %%xmm0;               "
//            ".strlen_4_2_start:                  "
//            " add $16, %0;                       "
//            " pcmpistri $0x08, (%0,%1), %%xmm0;  "
//            " jnz .strlen_4_2_start;             "
//            " add %2, %0;                        "
//            :"=a"(result):"d"((size_t)string),"c"((size_t)string));
//
//        return result;
//    }
    while ( string[result] )
        result++;
    return result;
}
