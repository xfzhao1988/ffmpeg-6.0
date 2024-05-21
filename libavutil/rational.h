/*
 * rational numbers
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * @ingroup lavu_math_rational
 * Utilties for rational number calculation.
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef AVUTIL_RATIONAL_H
#define AVUTIL_RATIONAL_H

#include <stdint.h>
#include <limits.h>
#include "attributes.h"

/**
 * @defgroup lavu_math_rational AVRational
 * @ingroup lavu_math
 * Rational number calculation.
 * 有理数计算。
 *
 * While rational numbers can be expressed as floating-point numbers, the
 * conversion process is a lossy one, so are floating-point operations. On the
 * other hand, the nature of FFmpeg demands highly accurate calculation of
 * timestamps. This set of rational number utilities serves as a generic
 * interface for manipulating rational numbers as pairs of numerators and
 * denominators.
 * 虽然有理数可以表示为浮点数，但转换过程是有损的，浮点运算也是如此。 另一方面，
 * FFmpeg 的性质要求时间戳的计算高度准确。 这组有理数实用程序用作将有理数作为
 * 分子和分母对进行操作的通用接口。
 *
 * Many of the functions that operate on AVRational's have the suffix `_q`, in
 * reference to the mathematical symbol "ℚ" (Q) which denotes the set of all
 * rational numbers.
 * 许多在 AVRational 上运行的函数都有后缀 `_q`，参考数学符号“ℚ”(Q)，它表示所有有理数的集合。
 *
 * @{
 */

/**
 * Rational number (pair of numerator and denominator).
 * 有理数（分子和分母对）。
 */
typedef struct AVRational{
    int num; ///< Numerator分子
    int den; ///< Denominator分母
} AVRational;

/**
 * Create an AVRational.
 * 创建一个 AVRational。
 *
 * Useful for compilers that do not support compound literals.
 * 对于不支持复合文字的编译器很有用。
 *
 * @note The return value is not reduced.
 * 返回值不会减少。
 * @see av_reduce()
 */
static inline AVRational av_make_q(int num, int den)
{
    AVRational r = { num, den };
    return r;
}

/**
 * Compare two rationals.
 * 比较两个有理数。
 *
 * @param a First rational
 * 第一个有理数
 * @param b Second rational
 * 第二个有理数
 *
 * @return One of the following values:
 * 以下值之一：
 *         - 0 if `a == b`
 *         - 1 if `a > b`
 *         - -1 if `a < b`
 *         - `INT_MIN` if one of the values is of the form `0 / 0`
 *           如果其中一个值的形式为“0 / 0”
 */
//done
static inline int av_cmp_q(AVRational a, AVRational b){
    const int64_t tmp= a.num * (int64_t)b.den - b.num * (int64_t)a.den;

    /**
     * @brief
     * 有理数：a(a.num, a.den)
     * 有理数：b(b.num, b.dem)
     *
     * a与b的差：
     * a - b = a.num/a.den - b.num/b.den
     *       = (a.num * b.den - b.num * a.den)/(a.den * b.den)
     *       = tmp / (a.den * b.den)
     * a - b的差值结果的符号由tmp/a.den/b.den三者的符号位决定，其满足负负得正，正负得负，正正得正的规律，
     * 这恰好与异或运算的规则（两个数做异或运算，相同得0，不同得1）一致。因而a - b差值的符号可以使用
     * ((tmp ^ a.den ^ b.den)>>63)获得，穷举以下case，可以获得最终的符号位：
     *
     *                  case1(都为正数)      case2(有一个负数)       case3(有两个负数)        case4(有三个负数)
     * tmp(符号位)        0                         1                      1                        1
     * a.den(符号位)      0                         0                      1                        1
     * b.den(符号位)      0                         0                      0                        1
     * 异或结果           0                         1                      0                        1
     *
     * 因此(int)((tmp ^ a.den ^ b.den)>>63)的结果为0xFFFFFFFF或0x0，再与1做或运算，最终的结果为0x1或者0xFFFFFFFF,
     * 即1和-1
     */
    if(tmp) return (int)((tmp ^ a.den ^ b.den)>>63)|1;//返回值为-1/1
    /**
     * @brief 两个有理数的分母b.den和a.den都不为0时，穷举符合这种要求的case，
     * 得到如下可能的两个有理数情况:
     *
     *            case1        case2       case3       case4
     * 有理数x：    a/A          0/A         a/A         0/A
     *
     * 有理数y：    b/B          b/B         0/B         0/B
     * 上面的四个case，只有case1和case4同时满足tmp为0的要求，其返回值
     * 均为0，表示两个有理数相等。
     */
    else if(b.den && a.den) return 0;//返回值为0
    /**
     * @brief a.den和b.den至少有一个为0, 穷举符合这种要求的case，
     * 得到如下可能的两个有理数情况:
     *
     *            case1        case2       case3
     * 有理数x：    a/0          a/A         a/0
     *
     * 有理数y：    b/B          b/0         b/0
     * 上面的三个case，只有case3同时满足tmp为0的要求，故只有case3成立。其返回值
     * 会根据a.num和b.num的正负，分别返回1、-1和0：
     *
     * a正b负：返回1
     * a负b正：返回-1
     * a正b正：返回0
     * a负b负：返回0
     */
    else if(a.num && b.num) return (a.num>>31) - (b.num>>31);//返回值为-1/1/0
    /**
     * @brief b.den&&a.den=0(表示两个数的分母最少有一个为0)，b.den&&a.den=0(表示两个数的分子至少有一个为0)，
     * 穷举符合这种要求的case，得到如下可能的两个有理数情况:
     *            case1        case2       case3       case4       case5      ...
     * 有理数x：    0/A          a/0         0/0         a/A         0/0       ...
     *
     * 有理数y：    b/0          0/B         b/B         0/0         0/B       ...
     *
     * 上面这些case，除了case1/case2的其他case同时符合tmp为0的要求，故除了case1/case2的其他case均成立
     * 这些case中都包含有0/0的有理数，因而符合：如果其中一个有理数值的形式为“0 / 0”，则返回INT_MIN的预期
     *
     */
    else                    return INT_MIN;
}

/**
 * Convert an AVRational to a `double`.
 * 将 AVRational 转换为“double”。
 * @param a AVRational to convert
 * 要转换的 AVRational
 * @return `a` in floating-point form
 * 浮点形式的“a”
 * @see av_d2q()
 */
//done
static inline double av_q2d(AVRational a){
    return a.num / (double) a.den;
}

/**
 * @brief av_reduce 函数通常与 FFmpeg 多媒体框架相关联。它用于简化分数，通过找到分子和分母的
 * 最大公约数（GCD,Greatest Common Divisor），然后将它们都除以该值，将它们简化为最简形式。以下是其参数的说明：
 *
 * dst_num：指向整数的指针，用于存储简化后的分子。
 * dst_den：指向整数的指针，用于存储简化后的分母。
 * num：要简化的分数的分子。
 * den：要简化的分数的分母。
 * max：分子和分母的最大允许值。
 *
 * 该函数通过将分数 (num / den) 简化，使得分子和分母都在 ±max 的范围内，并保持它们的比例不变。
 * 简化后的分子和分母分别存储在 dst_num 和 dst_den 指向的内存位置。
 *
 * 这个函数对于处理有理数特别有用，尤其是在多媒体应用中，其中像素宽高比、帧率和其他参数经常表示为分数。
 *
 */
*/
/**
 * Reduce a fraction.
 * 减少一小部分。
 *
 * This is useful for framerate calculations.
 * 这对于帧率计算很有用。
 *
 * @param[out] dst_num Destination numerator
 * 目标分子
 * @param[out] dst_den Destination denominator
 * 目标分母
 * @param[in]      num Source numerator
 * 原分子
 * @param[in]      den Source denominator
 * 原分母
 * @param[in]      max Maximum allowed values for `dst_num` & `dst_den`
 * `dst_num` 和 `dst_den` 的最大允许值
 * @return 1 if the operation is exact, 0 otherwise
 * 如果操作是精确的则为 1，否则为 0
 */
//done
int av_reduce(int *dst_num, int *dst_den, int64_t num, int64_t den, int64_t max);

/**
 * Multiply two rationals.
 * @param b First rational
 * @param c Second rational
 * @return b*c
 */
AVRational av_mul_q(AVRational b, AVRational c) av_const;

/**
 * Divide one rational by another.
 * @param b First rational
 * @param c Second rational
 * @return b/c
 */
AVRational av_div_q(AVRational b, AVRational c) av_const;

/**
 * Add two rationals.
 * @param b First rational
 * @param c Second rational
 * @return b+c
 */
AVRational av_add_q(AVRational b, AVRational c) av_const;

/**
 * Subtract one rational from another.
 * @param b First rational
 * @param c Second rational
 * @return b-c
 */
AVRational av_sub_q(AVRational b, AVRational c) av_const;

/**
 * Invert a rational.
 * @param q value
 * @return 1 / q
 */
static av_always_inline AVRational av_inv_q(AVRational q)
{
    AVRational r = { q.den, q.num };
    return r;
}

/**
 * Convert a double precision floating point number to a rational.
 *
 * In case of infinity, the returned value is expressed as `{1, 0}` or
 * `{-1, 0}` depending on the sign.
 *
 * @param d   `double` to convert
 * @param max Maximum allowed numerator and denominator
 * @return `d` in AVRational form
 * @see av_q2d()
 */
AVRational av_d2q(double d, int max) av_const;

/**
 * Find which of the two rationals is closer to another rational.
 *
 * @param q     Rational to be compared against
 * @param q1    Rational to be tested
 * @param q2    Rational to be tested
 * @return One of the following values:
 *         - 1 if `q1` is nearer to `q` than `q2`
 *         - -1 if `q2` is nearer to `q` than `q1`
 *         - 0 if they have the same distance
 */
int av_nearer_q(AVRational q, AVRational q1, AVRational q2);

/**
 * Find the value in a list of rationals nearest a given reference rational.
 *
 * @param q      Reference rational
 * @param q_list Array of rationals terminated by `{0, 0}`
 * @return Index of the nearest value found in the array
 */
int av_find_nearest_q_idx(AVRational q, const AVRational* q_list);

/**
 * Convert an AVRational to a IEEE 32-bit `float` expressed in fixed-point
 * format.
 *
 * @param q Rational to be converted
 * @return Equivalent floating-point value, expressed as an unsigned 32-bit
 *         integer.
 * @note The returned value is platform-indepedant.
 */
uint32_t av_q2intfloat(AVRational q);

/**
 * Return the best rational so that a and b are multiple of it.
 * If the resulting denominator is larger than max_den, return def.
 */
AVRational av_gcd_q(AVRational a, AVRational b, int max_den, AVRational def);

/**
 * @}
 */

#endif /* AVUTIL_RATIONAL_H */
