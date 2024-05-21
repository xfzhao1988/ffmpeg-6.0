/*
 * Copyright (c) 2015 James Almer
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_X86_INTMATH_H
#define AVUTIL_X86_INTMATH_H

#include <stdint.h>
#include <stdlib.h>
#if HAVE_FAST_CLZ
/**
 * @brief 这行代码使用了条件编译指令 #if defined(_MSC_VER)，用于检查是否正在使用 Microsoft Visual C++
 * 编译器（MSVC）。在这种情况下，如果正在使用 MSVC 编译器，则条件成立，代码块内的内容将被编译。
 *
 * 这种方式的条件编译通常用于根据不同的编译器或编译环境来编写特定的代码，以保证代码在不同平台或编译器下的兼容性。
 *
 */
#if defined(_MSC_VER)
#include <intrin.h>
 /**
 * @brief 这行代码使用了条件编译指令 #elif defined(__INTEL_COMPILER)，用于检查是否正在使用
 * Intel 编译器。在这种情况下，如果正在使用 Intel 编译器，则条件成立，代码块内的内容将被编译。
 *
 * 这种方式的条件编译通常用于根据不同的编译器或编译环境来编写特定的代码，以保证代码在不同平台或编译器下的兼容性。
 *
 */
#elif defined(__INTEL_COMPILER)
#include <immintrin.h>
#endif
#endif
#include "config.h"

#if HAVE_FAST_CLZ
#if (defined(__INTEL_COMPILER) && (__INTEL_COMPILER>=1216)) || defined(_MSC_VER)
#   if defined(__INTEL_COMPILER)
#       define ff_log2(x) (_bit_scan_reverse((x)|1))
#   else
#       define ff_log2 ff_log2_x86
static av_always_inline av_const int ff_log2_x86(unsigned int v)
{
    unsigned long n;
    _BitScanReverse(&n, v|1);
    return n;
}
#   endif
#   define ff_log2_16bit av_log2

#if defined(__INTEL_COMPILER) || (defined(_MSC_VER) && (_MSC_VER >= 1700) && \
                                  (defined(__BMI__) || !defined(__clang__)))
#   define ff_ctz(v) _tzcnt_u32(v)

#   if ARCH_X86_64
/**
 * @brief
 * 这行代码定义了一个宏 ff_ctzll(v)，该宏用于计算无符号 64 位整数 v 从低位开始的连续零的个数。
 * 在这个定义中，宏 ff_ctzll(v) 被展开为调用 _tzcnt_u64(v) 函数，该函数在支持
 * BMI（Bit Manipulation Instructions）指令集的处理器上计算 v 的末尾零的个数。
 *
 * 这个宏的作用是获取一个整数在二进制表示中从低位开始的零的个数，也就是获取这个整数的二进制表示中
 * 最低位的连续零的个数。这在一些位操作的场景中非常有用，比如获取一个数的最低有效位（LSB）的位置等。
 *
 */
#       define ff_ctzll(v) _tzcnt_u64(v)//done
#   else
#       define ff_ctzll ff_ctzll_x86//done
static av_always_inline av_const int ff_ctzll_x86(long long v)
{
    return ((uint32_t)v == 0) ? _tzcnt_u32((uint32_t)(v >> 32)) + 32 : _tzcnt_u32((uint32_t)v);
}
#   endif
#endif /* _MSC_VER */

#endif /* __INTEL_COMPILER */

#endif /* HAVE_FAST_CLZ */

#if defined(__GNUC__)

/* Our generic version of av_popcount is faster than GCC's built-in on
 * CPUs that don't support the popcnt instruction.
 */
#if defined(__POPCNT__)
    #define av_popcount   __builtin_popcount
#if ARCH_X86_64
    #define av_popcount64 __builtin_popcountll
#endif

#endif /* __POPCNT__ */

#if defined(__BMI2__)

#if AV_GCC_VERSION_AT_LEAST(5,1)
#define av_mod_uintp2 __builtin_ia32_bzhi_si
#elif HAVE_INLINE_ASM
/* GCC releases before 5.1.0 have a broken bzhi builtin, so for those we
 * implement it using inline assembly
 */
#define av_mod_uintp2 av_mod_uintp2_bmi2
static av_always_inline av_const unsigned av_mod_uintp2_bmi2(unsigned a, unsigned p)
{
    if (av_builtin_constant_p(p))
        return a & ((1 << p) - 1);
    else {
        unsigned x;
        __asm__ ("bzhi %2, %1, %0 \n\t" : "=r"(x) : "rm"(a), "r"(p));
        return x;
    }
}
#endif /* AV_GCC_VERSION_AT_LEAST */

#endif /* __BMI2__ */

#if defined(__SSE2__) && !defined(__INTEL_COMPILER)

#define av_clipd av_clipd_sse2
static av_always_inline av_const double av_clipd_sse2(double a, double amin, double amax)
{
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    __asm__ ("maxsd %1, %0 \n\t"
             "minsd %2, %0 \n\t"
             : "+&x"(a) : "xm"(amin), "xm"(amax));
    return a;
}

#endif /* __SSE2__ */

#if defined(__SSE__) && !defined(__INTEL_COMPILER)

#define av_clipf av_clipf_sse
static av_always_inline av_const float av_clipf_sse(float a, float amin, float amax)
{
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    __asm__ ("maxss %1, %0 \n\t"
             "minss %2, %0 \n\t"
             : "+&x"(a) : "xm"(amin), "xm"(amax));
    return a;
}

#endif /* __SSE__ */

#if defined(__AVX__) && !defined(__INTEL_COMPILER)

#undef av_clipd
#define av_clipd av_clipd_avx
static av_always_inline av_const double av_clipd_avx(double a, double amin, double amax)
{
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    __asm__ ("vmaxsd %1, %0, %0 \n\t"
             "vminsd %2, %0, %0 \n\t"
             : "+&x"(a) : "xm"(amin), "xm"(amax));
    return a;
}

#undef av_clipf
#define av_clipf av_clipf_avx
static av_always_inline av_const float av_clipf_avx(float a, float amin, float amax)
{
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    __asm__ ("vmaxss %1, %0, %0 \n\t"
             "vminss %2, %0, %0 \n\t"
             : "+&x"(a) : "xm"(amin), "xm"(amax));
    return a;
}

#endif /* __AVX__ */

#endif /* __GNUC__ */

#endif /* AVUTIL_X86_INTMATH_H */
