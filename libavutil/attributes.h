/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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
 * Macro definitions for various function/variable attributes
 */

#ifndef AVUTIL_ATTRIBUTES_H
#define AVUTIL_ATTRIBUTES_H


#ifdef __GNUC__
/**
 * @brief 这段代码定义了一个宏 AV_GCC_VERSION_AT_LEAST(x,y)，用于检查编译器是否至少是指定版本的 GCC。
 *
 * 具体来说：
 * __GNUC__ 和 __GNUC_MINOR__ 是 GCC 编译器预定义的宏，分别表示当前编译器的主版本号和次版本号。
 *
 * __GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y)：这个表达式用于判断当前
 * GCC 编译器版本是否大于等于指定的版本 (x.y)。如果当前 GCC 编译器的主版本号大于 x，则直接返回真，
 * 如果主版本号等于 x，则比较次版本号是否大于等于 y。
 *
 * 例如，如果你希望检查编译器是否至少是 GCC 4.8 版本，你可以这样使用这个宏：
 * #if AV_GCC_VERSION_AT_LEAST(4,8)
 *   // 当前 GCC 版本至少是 4.8
 * #else
 *   // 当前 GCC 版本不满足要求
 * #endif
 *
 * 这个宏在编写跨平台代码时特别有用，可以根据 GCC 编译器的版本来选择不同的实现方式或者处理一些特定的问题。
 *
 */
#    define AV_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
/**
 * @brief 这段代码定义了一个宏 AV_GCC_VERSION_AT_MOST(x,y)，用于检查编译器是否最多是指定版本的 GCC。
 *
 * 具体来说：
 *
 * __GNUC__ 和 __GNUC_MINOR__ 是 GCC 编译器预定义的宏，分别表示当前编译器的主版本号和次版本号。
 *
 * __GNUC__ < (x) || __GNUC__ == (x) && __GNUC_MINOR__ <= (y)：这个表达式用于判断当前 GCC 编译器版本
 * 是否小于等于指定的版本 (x.y)。如果当前 GCC 编译器的主版本号小于 x，则直接返回真，如果主版本号等于 x，
 * 则比较次版本号是否小于等于 y。
 *
 * 例如，如果你希望检查编译器是否最多是 GCC 4.8 版本，你可以这样使用这个宏：
 * #if AV_GCC_VERSION_AT_MOST(4,8)
 *   // 当前 GCC 版本最多是 4.8
 * #else
 *   // 当前 GCC 版本不满足要求
 * #endif
 *
 * 这个宏在编写跨平台代码时特别有用，可以根据 GCC 编译器的版本来选择不同的实现方式或者处理一些特定的问题。
 *
 */
#    define AV_GCC_VERSION_AT_MOST(x,y)  (__GNUC__ < (x) || __GNUC__ == (x) && __GNUC_MINOR__ <= (y))
#else
#    define AV_GCC_VERSION_AT_LEAST(x,y) 0
#    define AV_GCC_VERSION_AT_MOST(x,y)  0
#endif

#ifdef __has_builtin
#    define AV_HAS_BUILTIN(x) __has_builtin(x)
#else
#    define AV_HAS_BUILTIN(x) 0
#endif

#ifndef av_always_inline
#if AV_GCC_VERSION_AT_LEAST(3,1)
/**
 * @brief 这是一个在C/C++中常用的宏定义，特别是在对性能要求较高的系统中使用。
 *
 * 这里是各部分的中文解释：
 *
 * #define：这是C和C++中的预处理指令，用于定义宏。
 *
 * av_always_inline：这是被定义的宏的名称。这似乎是一个自定义的名称，可能用于特定的项目或库
 * （例如FFmpeg经常使用前缀“av”来表示与音视频相关的函数）。
 *
 * __attribute__((always_inline))：这是GCC编译器的一个属性。它向编译器建议，无论编译器的
 * 优化设置如何，都应该总是将函数内联。内联函数可以通过消除函数调用的开销来提高执行速度。
 *
 * inline：这是C/C++中的一个关键字。它提示编译器应该尝试将函数的代码嵌入到每个调用函数的点，
 * 而不是管理函数调用和返回。这是另一种优化性能的方式，但这只是一个建议，编译器可能选择不内联函数。
 *
 * 通过组合这些，av_always_inline 被定义为任何用它声明的函数将被建议编译器内联，且由于 always_inline
 * 属性的强烈推荐，编译器被强烈建议这样做。这在性能关键的代码中非常有用，其中必须最小化函数调用的开销。
 *
 */
#    define av_always_inline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#    define av_always_inline __forceinline
#else
#    define av_always_inline inline
#endif
#endif

#ifndef av_extern_inline
#if defined(__ICL) && __ICL >= 1210 || defined(__GNUC_STDC_INLINE__)
#    define av_extern_inline extern inline
#else
#    define av_extern_inline inline
#endif
#endif

#if AV_GCC_VERSION_AT_LEAST(3,4)
#    define av_warn_unused_result __attribute__((warn_unused_result))
#else
#    define av_warn_unused_result
#endif

#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define av_noinline __attribute__((noinline))
#elif defined(_MSC_VER)
#    define av_noinline __declspec(noinline)
#else
#    define av_noinline
#endif

#if AV_GCC_VERSION_AT_LEAST(3,1) || defined(__clang__)
#    define av_pure __attribute__((pure))
#else
#    define av_pure
#endif

/**
 * @brief 这里是这个宏定义的各个部分的解释：
 *
 * #define：这是 C 和 C++ 中的一个预处理指令，用于定义宏。
 *
 * av_const：这是被定义的宏的名称。看起来这是一个自定义的名称，可能是用在特定的项目或库中，
 * 比如像 FFmpeg 这样的音视频处理库。
 *
 * __attribute__((const))：这是 GCC 编译器的一个属性，它告诉编译器这个函数或变量具有“const”属性。
 * 这意味着函数满足以下条件：
 * 函数不会修改任何不通过指针参数访问的内存区域。
 * 函数对于相同的输入总是返回相同的结果。
 *
 * 这个属性非常有用，因为它允许编译器进行更积极的优化，例如删除重复的函数调用（如果函数多次使用
 * 相同的参数进行调用），并将结果替换为第一次调用的结果。这样可以显著提高程序的执行效率。
 *
 */
#if AV_GCC_VERSION_AT_LEAST(2,6) || defined(__clang__)
#    define av_const __attribute__((const))
#else
#    define av_const
#endif

#if AV_GCC_VERSION_AT_LEAST(4,3) || defined(__clang__)
#    define av_cold __attribute__((cold))
#else
#    define av_cold
#endif

#if AV_GCC_VERSION_AT_LEAST(4,1) && !defined(__llvm__)
#    define av_flatten __attribute__((flatten))
#else
#    define av_flatten
#endif

#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif

/**
 * Disable warnings about deprecated features
 * This is useful for sections of code kept for backward compatibility and
 * scheduled for removal.
 */
#ifndef AV_NOWARN_DEPRECATED
#if AV_GCC_VERSION_AT_LEAST(4,6) || defined(__clang__)
#    define AV_NOWARN_DEPRECATED(code) \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"") \
        code \
        _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#    define AV_NOWARN_DEPRECATED(code) \
        __pragma(warning(push)) \
        __pragma(warning(disable : 4996)) \
        code; \
        __pragma(warning(pop))
#else
#    define AV_NOWARN_DEPRECATED(code) code
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#    define av_unused __attribute__((unused))
#else
#    define av_unused
#endif

/**
 * Mark a variable as used and prevent the compiler from optimizing it
 * away.  This is useful for variables accessed only from inline
 * assembler without the compiler being aware.
 */
#if AV_GCC_VERSION_AT_LEAST(3,1) || defined(__clang__)
#    define av_used __attribute__((used))
#else
#    define av_used
#endif

#if AV_GCC_VERSION_AT_LEAST(3,3) || defined(__clang__)
#   define av_alias __attribute__((may_alias))
#else
#   define av_alias
#endif

#if (defined(__GNUC__) || defined(__clang__)) && !defined(__INTEL_COMPILER)
#    define av_uninit(x) x=x
#else
#    define av_uninit(x) x
#endif

/**
 * @brief 这段代码定义了一个宏 av_printf_format，它使用了 GCC 的 __attribute__ 语法，
 * 用于指示编译器对某个函数进行 printf 格式字符串的检查。
 *
 * 具体来说：
 *
 * __format__(__printf__, fmtpos, attrpos)：这个属性指示编译器将检查函数的参数，以确保函数的第 fmtpos
 * 个参数是格式字符串，且它的格式符与 attrpos 参数所表示的格式串参数中的格式符相匹配。这样可以帮助编译器
 * 在编译时捕获潜在的格式字符串不匹配或格式化输出中潜在的问题。
 *
 * fmtpos：是一个整数值，表示格式字符串在函数参数列表中的位置。通常情况下，
 * 格式字符串是作为函数参数的第一个参数传递的，所以 fmtpos 的值通常为 1。
 *
 * attrpos：也是一个整数值，表示格式字符串中的格式符参数列表的开始位置。这个参数用于告诉编译器在格式化
 * 字符串中从哪个位置开始查找格式符。
 *
 * 这个宏通常用于声明类似于 printf 的函数，以确保格式字符串与参数的类型匹配。例如：
 * void my_printf(const char *fmt, ...) av_printf_format(1, 2);
 *
 * 这样声明的 my_printf 函数，编译器将会检查第一个参数是否是格式字符串，以及后续的参数与格式字符串中的格式符是否匹配。
 *
 */
#if defined(__GNUC__) || defined(__clang__)
#    define av_builtin_constant_p __builtin_constant_p
#    define av_printf_format(fmtpos, attrpos) __attribute__((__format__(__printf__, fmtpos, attrpos)))
#else
#    define av_builtin_constant_p(x) 0
#    define av_printf_format(fmtpos, attrpos)
#endif

#if AV_GCC_VERSION_AT_LEAST(2,5) || defined(__clang__)
#    define av_noreturn __attribute__((noreturn))
#else
#    define av_noreturn
#endif

#endif /* AVUTIL_ATTRIBUTES_H */