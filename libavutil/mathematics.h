/*
 * copyright (c) 2005-2012 Michael Niedermayer <michaelni@gmx.at>
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
 * @addtogroup lavu_math
 * Mathematical utilities for working with timestamp and time base.
 * 用于处理时间戳和时基的数学实用程序。
 */

#ifndef AVUTIL_MATHEMATICS_H
#define AVUTIL_MATHEMATICS_H

#include <stdint.h>
#include <math.h>
#include "attributes.h"
#include "rational.h"
#include "intfloat.h"

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif
#ifndef M_LOG2_10
#define M_LOG2_10      3.32192809488736234787  /* log_2 10 */
#endif
/**
 * @brief 黄金比例是一种特殊的比例关系，通常用希腊字母 φ（phi）表示，其近似值约为1.61803398875。
 * 这种比例在自然界、艺术、建筑和设计中被广泛应用，被认为具有美学上的吸引力和和谐感。
 *
 * 黄金比例还与斐波那契数列有关，其中相邻两个数的比值越来越接近黄金比例，例如，1/1、2/1、3/2、5/3、8/5 等。
 *
 * 除了美学和数学上的应用，黄金比例还出现在生物学、天文学等自然科学领域中，被认为是自然界中普遍存在的一种规律。
 *
 */
#ifndef M_PHI
#define M_PHI          1.61803398874989484820   /* phi / golden ratio */
#endif
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif
#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif
/**
 * @brief
 * 这行代码使用了一个宏定义 NAN，它将一个 32 位整数 0x7fc00000 转换为单精度浮点数表示的 NaN（Not a Number）。
 * 在 IEEE 754 浮点数标准中，特定的位模式被用来表示 NaN，通常用于表示无效的数学运算结果。NaN 不等于任何数，
 * 包括它自己，在数学运算中的结果通常被定义为 NaN。
 *
 * 这个宏定义将整数 0x7fc00000 转换为单精度浮点数表示的 NaN，并将其命名为 NAN，方便在代码中使用。
 * 注意，在实际使用时，av_int2float 函数应该是一个将整数转换为单精度浮点数的函数。
 *
 */
#ifndef NAN
#define NAN            av_int2float(0x7fc00000)
#endif
/**
 * @brief
 * 这行代码定义了一个宏 INFINITY，它将一个特定的 32 位整数 0x7f800000 转换为单精度浮点数表示的正无穷大。
 * 在 IEEE 754 浮点数标准中，特定的位模式被用来表示正无穷大，通常表示某些数学运算的结果超出了浮点数的表示范围。
 *
 * 这个宏定义将整数 0x7f800000 转换为单精度浮点数表示的正无穷大，并将其命名为 INFINITY，方便在代码中使用。
 * 注意，在实际使用时，av_int2float 函数应该是一个将整数转换为单精度浮点数的函数。
 *
 */
#ifndef INFINITY
#define INFINITY       av_int2float(0x7f800000)
#endif

/**
 * @addtogroup lavu_math
 *
 * @{
 */

/**
 * Rounding methods.
 * 舍入方法。
 */
/**
 * 这段代码定义了一个枚举类型 AVRounding，用于表示不同的舍入方式。这些舍入方式通常用于处理浮点数或整数的
 * 舍入操作。具体的舍入方式包括：
 *
 * AV_ROUND_ZERO：向零舍入。
 * AV_ROUND_INF：向无穷大方向舍入。
 * AV_ROUND_DOWN：向负无穷大方向舍入。
 * AV_ROUND_UP：向正无穷大方向舍入。
 * AV_ROUND_NEAR_INF：四舍五入到最近的整数，当距离两个整数相等时，远离零的那个被选中。
 *
 * 此外，还有一个特殊的标记 AV_ROUND_PASS_MINMAX，用于告诉重新缩放函数（如 av_rescale_rnd）
 * 保持 INT64_MIN 和 INT64_MAX 不变，避免对 #AV_NOPTS_VALUE 进行特殊处理。这个标记是一个位掩码，
 * 必须与枚举类型的其他值进行按位或运算，以设置正常情况下的行为。
 *
 * 这些舍入方式可以在进行视频时间戳（PTS）的重新缩放等操作时非常有用。
*/
//done
enum AVRounding {
    AV_ROUND_ZERO     = 0, ///< Round toward zero.
                           ///< 向零舍入。
    AV_ROUND_INF      = 1, ///< Round away from zero.
                           ///< 向远离零的方向舍入
                           /**
                            * "Round away from zero"（向远离零的方向舍入）意味着将一个数字四舍五入为离它最近的整数，
                            * 但该整数的绝对值更大。对于正数，这意味着向正无穷大的方向舍入；对于负数，这意味着向负无
                            * 穷大的方向舍入。
                            * 例如：
                            *
                            * 2.3 将被舍入为 3。
                            * -2.3 将被舍入为 -3。
                            * 在您提供的 AVRounding 枚举中，"Round away from zero" 对应于 AV_ROUND_INF 值。
                           */
    AV_ROUND_DOWN     = 2, ///< Round toward -infinity.
                           ///< 向负无穷大舍入。
    AV_ROUND_UP       = 3, ///< Round toward +infinity.
                           ///< 向正无穷大舍入。
    AV_ROUND_NEAR_INF = 5, ///< Round to nearest and halfway cases away from zero.
                           ///< 舍入到距离零最近的整数，当距离两个整数相等时，远离零的那个被选中。
    /**
     * Flag telling rescaling functions to pass `INT64_MIN`/`MAX` through
     * unchanged, avoiding special cases for #AV_NOPTS_VALUE.
     * 标记告诉重新缩放函数将 `INT64_MIN`/`MAX` 不变地传递，避免 #AV_NOPTS_VALUE 的特殊情况。
     *
     * Unlike other values of the enumeration AVRounding, this value is a
     * bitmask that must be used in conjunction with another value of the
     * enumeration through a bitwise OR, in order to set behavior for normal
     * cases.
     * 与 AVRounding 枚举的其他值不同，该值是一个位掩码，必须通过按位 OR 与枚举的另一个值结合使用，
     * 以便设置正常情况下的行为。
     *
     * @code{.c}
     * av_rescale_rnd(3, 1, 2, AV_ROUND_UP | AV_ROUND_PASS_MINMAX);
     * // Rescaling 3:
     * //     Calculating 3 * 1 / 2
     * //     3 / 2 is rounded up to 2
     * //     => 2
     *
     * av_rescale_rnd(AV_NOPTS_VALUE, 1, 2, AV_ROUND_UP | AV_ROUND_PASS_MINMAX);
     * // Rescaling AV_NOPTS_VALUE:
     * //     AV_NOPTS_VALUE == INT64_MIN
     * //     AV_NOPTS_VALUE is passed through
     * //     => AV_NOPTS_VALUE
     * @endcode
     */
    AV_ROUND_PASS_MINMAX = 8192,
};

/**
 * Compute the greatest common divisor of two integer operands.
 * 计算两个整数操作数的最大公约数。
 *
 * @param a Operand
 * 操作数a
 * @param b Operand
 * 操作数b
 * @return GCD of a and b up to sign; if a >= 0 and b >= 0, return value is >= 0;
 * if a == 0 and b == 0, returns 0.
 * a 和 b 的最大公约数，保留符号；如果 a >= 0 且 b >= 0，则返回值 >= 0；如果 a == 0 且 b == 0，则返回 0。
 */
//done
int64_t av_const av_gcd(int64_t a, int64_t b);

/**
 * Rescale a 64-bit integer with rounding to nearest.
 * 重新缩放 64 位整数并四舍五入到最接近的值。
 *
 * The operation is mathematically equivalent to `a * b / c`, but writing that
 * directly can overflow.
 * 该操作在数学上相当于“a * b / c”，但直接写入可能会溢出。
 *
 * This function is equivalent to av_rescale_rnd() with #AV_ROUND_NEAR_INF.
 * 此函数相当于带有 #AV_ROUND_NEAR_INF 的 av_rescale_rnd()。
 *
 * @see av_rescale_rnd(), av_rescale_q(), av_rescale_q_rnd()
 */
//done
int64_t av_rescale(int64_t a, int64_t b, int64_t c) av_const;

/**
 * Rescale a 64-bit integer with specified rounding.
 * 使用指定的舍入重新缩放 64 位整数。
 *
 * The operation is mathematically equivalent to `a * b / c`, but writing that
 * directly can overflow, and does not support different rounding methods.
 * If the result is not representable then INT64_MIN is returned.
 * 该运算在数学上等价于“a * b / c”，但直接写入可能会溢出，并且不支持不同的舍入方法。
 * 如果结果不可表示，则返回 INT64_MIN。
 *
 * @see av_rescale(), av_rescale_q(), av_rescale_q_rnd()
 */
int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const;

/**
 * Rescale a 64-bit integer by 2 rational numbers.
 *
 * The operation is mathematically equivalent to `a * bq / cq`.
 *
 * This function is equivalent to av_rescale_q_rnd() with #AV_ROUND_NEAR_INF.
 *
 * @see av_rescale(), av_rescale_rnd(), av_rescale_q_rnd()
 */
int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const;

/**
 * Rescale a 64-bit integer by 2 rational numbers with specified rounding.
 *
 * The operation is mathematically equivalent to `a * bq / cq`.
 *
 * @see av_rescale(), av_rescale_rnd(), av_rescale_q()
 */
int64_t av_rescale_q_rnd(int64_t a, AVRational bq, AVRational cq,
                         enum AVRounding rnd) av_const;

/**
 * Compare two timestamps each in its own time base.
 *
 * @return One of the following values:
 *         - -1 if `ts_a` is before `ts_b`
 *         - 1 if `ts_a` is after `ts_b`
 *         - 0 if they represent the same position
 *
 * @warning
 * The result of the function is undefined if one of the timestamps is outside
 * the `int64_t` range when represented in the other's timebase.
 */
int av_compare_ts(int64_t ts_a, AVRational tb_a, int64_t ts_b, AVRational tb_b);

/**
 * Compare the remainders of two integer operands divided by a common divisor.
 *
 * In other words, compare the least significant `log2(mod)` bits of integers
 * `a` and `b`.
 *
 * @code{.c}
 * av_compare_mod(0x11, 0x02, 0x10) < 0 // since 0x11 % 0x10  (0x1) < 0x02 % 0x10  (0x2)
 * av_compare_mod(0x11, 0x02, 0x20) > 0 // since 0x11 % 0x20 (0x11) > 0x02 % 0x20 (0x02)
 * @endcode
 *
 * @param a Operand
 * @param b Operand
 * @param mod Divisor; must be a power of 2
 * @return
 *         - a negative value if `a % mod < b % mod`
 *         - a positive value if `a % mod > b % mod`
 *         - zero             if `a % mod == b % mod`
 */
int64_t av_compare_mod(uint64_t a, uint64_t b, uint64_t mod);

/**
 * Rescale a timestamp while preserving known durations.
 *
 * This function is designed to be called per audio packet to scale the input
 * timestamp to a different time base. Compared to a simple av_rescale_q()
 * call, this function is robust against possible inconsistent frame durations.
 *
 * The `last` parameter is a state variable that must be preserved for all
 * subsequent calls for the same stream. For the first call, `*last` should be
 * initialized to #AV_NOPTS_VALUE.
 *
 * @param[in]     in_tb    Input time base
 * @param[in]     in_ts    Input timestamp
 * @param[in]     fs_tb    Duration time base; typically this is finer-grained
 *                         (greater) than `in_tb` and `out_tb`
 * @param[in]     duration Duration till the next call to this function (i.e.
 *                         duration of the current packet/frame)
 * @param[in,out] last     Pointer to a timestamp expressed in terms of
 *                         `fs_tb`, acting as a state variable
 * @param[in]     out_tb   Output timebase
 * @return        Timestamp expressed in terms of `out_tb`
 *
 * @note In the context of this function, "duration" is in term of samples, not
 *       seconds.
 */
int64_t av_rescale_delta(AVRational in_tb, int64_t in_ts,  AVRational fs_tb, int duration, int64_t *last, AVRational out_tb);

/**
 * Add a value to a timestamp.
 *
 * This function guarantees that when the same value is repeatly added that
 * no accumulation of rounding errors occurs.
 *
 * @param[in] ts     Input timestamp
 * @param[in] ts_tb  Input timestamp time base
 * @param[in] inc    Value to be added
 * @param[in] inc_tb Time base of `inc`
 */
int64_t av_add_stable(AVRational ts_tb, int64_t ts, AVRational inc_tb, int64_t inc);


/**
 * @}
 */

#endif /* AVUTIL_MATHEMATICS_H */
