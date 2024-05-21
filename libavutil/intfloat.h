/*
 * Copyright (c) 2011 Mans Rullgard
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

#ifndef AVUTIL_INTFLOAT_H
#define AVUTIL_INTFLOAT_H

#include <stdint.h>
#include "attributes.h"

/**
 * @brief 这段代码定义了一个联合体（union），名为 av_intfloat32，它包含两个成员：一个 32 位无符号
 * 整数 i 和一个单精度浮点数 f。联合体是一种特殊的数据结构，它允许在相同的内存位置存储不同的数据类型，
 * 但一次只能使用一个成员。
 *
 * 在这个联合体中，当你给 i 赋值时，你可以使用这个整数来表示一个单精度浮点数的二进制形式，
 * 从而实现整数与浮点数之间的转换。这种技术在一些低级编程任务中很有用，但要注意联合体的使用需要谨慎，
 * 确保不会发生未定义的行为。
 *
 */
union av_intfloat32 {
    uint32_t i;
    float    f;
};

/**
 * @brief 这段代码定义了一个联合体 av_intfloat64，包含两个成员：一个 64 位无符号整数 i 和一个双精度浮点数 f。
 * 与之前的示例类似，这个联合体允许在相同的内存位置存储一个整数或一个双精度浮点数，但一次只能使用一个成员。
 *
 * 使用这样的联合体可以实现整数与双精度浮点数之间的转换，方法是将整数赋值给 i，然后读取 f 中的双精度浮点数，
 * 或者将双精度浮点数赋值给 f，然后读取 i 中的整数。
 *
 * 这种技术可以在某些低级编程场景中很有用，但需要谨慎使用，确保转换过程正确且不会导致未定义的行为。
 *
 */
union av_intfloat64 {
    uint64_t i;
    double   f;
};

/**
 * Reinterpret a 32-bit integer as a float.
 * 将 32 位整数重新解释为浮点数。
 */
//done
static av_always_inline float av_int2float(uint32_t i)
{
    union av_intfloat32 v;
    v.i = i;
    return v.f;
}

/**
 * Reinterpret a float as a 32-bit integer.
 * 将浮点数重新解释为 32 位整数。
 */
static av_always_inline uint32_t av_float2int(float f)
{
    union av_intfloat32 v;
    v.f = f;
    return v.i;
}

/**
 * Reinterpret a 64-bit integer as a double.
 * 将 64 位整数重新解释为双精度数。
 */
static av_always_inline double av_int2double(uint64_t i)
{
    union av_intfloat64 v;
    v.i = i;
    return v.f;
}

/**
 * Reinterpret a double as a 64-bit integer.
 * 将双精度数重新解释为 64 位整数。
 */
static av_always_inline uint64_t av_double2int(double f)
{
    union av_intfloat64 v;
    v.f = f;
    return v.i;
}

#endif /* AVUTIL_INTFLOAT_H */
