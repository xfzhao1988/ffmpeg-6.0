/*
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

#ifndef AVUTIL_INTREADWRITE_H
#define AVUTIL_INTREADWRITE_H

#include <stdint.h>
#include "libavutil/avconfig.h"
#include "attributes.h"
#include "bswap.h"

/**
typedef union {
uint64_t u64;
uint32_t u32[2];
uint16_t u16[4];
uint8_t u8 [8];
double f64;
float f32[2];
} av_alias av_alias64;

ChatGPT
这段代码定义了一个 union（联合体），名为 av_alias64，其中包含了不同类型的数据成员。同时使用了 av_alias 属性，
表示这些成员可以视作可能别名的指针，这在处理底层数据结构时非常有用。

解释：
联合体定义：

typedef union {
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t  u8[8];
    double   f64;
    float    f32[2];
} av_alias av_alias64;

这个 union 包含了几种不同的数据类型，每一种类型占据相同的内存空间。
u64 使用了 64 位无符号整数 (uint64_t)，占据 8 个字节。
u32 是一个包含两个 32 位无符号整数的数组，每个占据 4 个字节，总共 8 个字节。
u16 是一个包含四个 16 位无符号整数的数组，每个占据 2 个字节，总共 8 个字节。
u8 是一个包含八个 8 位无符号整数的数组，每个占据 1 个字节，总共 8 个字节。
f64 是一个双精度浮点数 (double)，占据 8 个字节。
f32 是一个包含两个单精度浮点数 (float) 的数组，每个占据 4 个字节，总共 8 个字节。

av_alias 属性：
av_alias 属性是一个宏定义，用于告知编译器，这个联合体的各个成员可以被视作可能别名的指针。
这种声明方式可以影响编译器的优化和对内存的布局管理，特别是在需要进行底层内存操作时，这样的属性声明非常有用。

使用场景：
这种联合体的设计在处理二进制数据、网络通信、文件 I/O 或低级硬件控制等场景下很常见。
通过联合体，可以方便地在不同的数据表示之间进行转换，而 av_alias 属性的使用则确保了在某些情况下可以使用别名访问
不同类型的数据，从而更灵活地操作内存。

总之，这段代码展示了如何定义一个具有多种数据类型成员的联合体，并使用 av_alias 属性来强调这些成员可以视作
可能别名的指针，以提高代码的灵活性和效率。


举个例子说明av_alias64的使用

ChatGPT
av_alias64 这个联合体可以用来处理不同类型数据之间的转换和访问，特别是在需要进行内存布局控制或者数据类型转换时非常有用。
下面举一个简单的例子来说明其使用：


#include <stdio.h>
#include <stdint.h>

// 定义 av_alias 属性
#    define AV_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#if AV_GCC_VERSION_AT_LEAST(3,3) || defined(__clang__)
#define av_alias __attribute__((may_alias))
#else
#define av_alias
#endif

// 定义 av_alias64 联合体
typedef union {
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t  u8[8];
    double   f64;
    float    f32[2];
} av_alias av_alias64;

int main() {
    av_alias64 data;

    // 设置一个双精度浮点数
    data.f64 = 3.14159;

    // 访问并输出联合体中的不同数据表示
    printf("f64 value: %lf\n", data.f64);
    printf("u64 value: %lu\n", data.u64);
    printf("u32 values: %u %u\n", data.u32[0], data.u32[1]);
    printf("u16 values: %u %u %u %u\n", data.u16[0], data.u16[1], data.u16[2], data.u16[3]);
    printf("u8 values: %u %u %u %u %u %u %u %u\n", data.u8[0], data.u8[1], data.u8[2], data.u8[3],
           data.u8[4], data.u8[5], data.u8[6], data.u8[7]);

    return 0;
}

解释：
定义和初始化：

av_alias64 data; 定义了一个 av_alias64 类型的联合体变量 data。
设置双精度浮点数：

data.f64 = 3.14159; 将双精度浮点数 3.14159 存储到联合体中的 f64 成员。
访问不同表示：

使用 printf 输出不同成员的值：
data.f64 输出双精度浮点数值。
data.u64 输出 64 位无符号整数的值。
data.u32 输出两个 32 位无符号整数的值。
data.u16 输出四个 16 位无符号整数的值。
data.u8 输出八个 8 位无符号整数的值。
注意事项：
联合体的不同成员共享同一块内存空间，因此对一个成员的修改会影响其他成员的值。
av_alias 属性的使用确保了在需要时可以将不同类型的指针视为可能别名，这在需要进行内存布局控制和数据类型转换时特别有用。
这个例子展示了如何利用 av_alias64 联合体来方便地在不同的数据类型之间进行转换和访问，适用于底层编程和处理复杂数据结构的场景。
*/
typedef union {
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t  u8 [8];
    double   f64;
    float    f32[2];
} av_alias av_alias64;

typedef union {
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8 [4];
    float    f32;
} av_alias av_alias32;

typedef union {
    uint16_t u16;
    uint8_t  u8 [2];
} av_alias av_alias16;

/*
 * Arch-specific headers can provide any combination of
 * AV_[RW][BLN](16|24|32|48|64) and AV_(COPY|SWAP|ZERO)(64|128) macros.
 * Preprocessor symbols must be defined, even if these are implemented
 * as inline functions.
 * Arch 特定的标头可以提供 AV_[RW][BLN](16|24|32|48|64) 和 AV_(COPY|SWAP|ZERO)(64|128)
 * 宏的任意组合。必须定义预处理器符号，即使这些符号是作为内联函数实现的。
 *
 * R/W means read/write, B/L/N means big/little/native endianness.
 * The following macros require aligned access, compared to their
 * unaligned variants: AV_(COPY|SWAP|ZERO)(64|128), AV_[RW]N[8-64]A.
 * Incorrect usage may range from abysmal performance to crash
 * depending on the platform.
 * R/W 表示读/写，B/L/N 表示大/小/本机字节序。与未对齐的变体相比，以下宏需要对齐
 * 访问：AV_(COPY|SWAP|ZERO)(64|128)、AV_[RW]N[8-64]A。根据平台的不同，不正确的
 * 使用可能会导致性能极差甚至崩溃。
 *
 * The unaligned variants are AV_[RW][BLN][8-64] and AV_COPY*U.
 * 未对齐的变体是 AV_[RW][BLN][8-64] 和 AV_COPY*U。
 *
 */

#ifdef HAVE_AV_CONFIG_H

#include "config.h"

#if   ARCH_ARM
#   include "arm/intreadwrite.h"
#elif ARCH_AVR32
#   include "avr32/intreadwrite.h"
#elif ARCH_MIPS
#   include "mips/intreadwrite.h"
#elif ARCH_PPC
#   include "ppc/intreadwrite.h"
#elif ARCH_TOMI
#   include "tomi/intreadwrite.h"
#elif ARCH_X86
#   include "x86/intreadwrite.h"
#endif

#endif /* HAVE_AV_CONFIG_H */

/**
 * 如下的XX有：16/24/32/48/64
*/
/*
 * Map AV_RNXX <-> AV_R[BL]XX for all variants provided by per-arch headers.
 * 针对每个架构标头提供的所有变体映射 AV_RNXX <-> AV_R[BL]XX。
 */
//done
#if AV_HAVE_BIGENDIAN

#   if    defined(AV_RN16) && !defined(AV_RB16)
#       define AV_RB16(p) AV_RN16(p)
#   elif !defined(AV_RN16) &&  defined(AV_RB16)
#       define AV_RN16(p) AV_RB16(p)
#   endif

#   if    defined(AV_WN16) && !defined(AV_WB16)
#       define AV_WB16(p, v) AV_WN16(p, v)
#   elif !defined(AV_WN16) &&  defined(AV_WB16)
#       define AV_WN16(p, v) AV_WB16(p, v)
#   endif

#   if    defined(AV_RN24) && !defined(AV_RB24)
#       define AV_RB24(p) AV_RN24(p)
#   elif !defined(AV_RN24) &&  defined(AV_RB24)
#       define AV_RN24(p) AV_RB24(p)
#   endif

#   if    defined(AV_WN24) && !defined(AV_WB24)
#       define AV_WB24(p, v) AV_WN24(p, v)
#   elif !defined(AV_WN24) &&  defined(AV_WB24)
#       define AV_WN24(p, v) AV_WB24(p, v)
#   endif

#   if    defined(AV_RN32) && !defined(AV_RB32)
#       define AV_RB32(p) AV_RN32(p)
#   elif !defined(AV_RN32) &&  defined(AV_RB32)
#       define AV_RN32(p) AV_RB32(p)
#   endif

#   if    defined(AV_WN32) && !defined(AV_WB32)
#       define AV_WB32(p, v) AV_WN32(p, v)
#   elif !defined(AV_WN32) &&  defined(AV_WB32)
#       define AV_WN32(p, v) AV_WB32(p, v)
#   endif

#   if    defined(AV_RN48) && !defined(AV_RB48)
#       define AV_RB48(p) AV_RN48(p)
#   elif !defined(AV_RN48) &&  defined(AV_RB48)
#       define AV_RN48(p) AV_RB48(p)
#   endif

#   if    defined(AV_WN48) && !defined(AV_WB48)
#       define AV_WB48(p, v) AV_WN48(p, v)
#   elif !defined(AV_WN48) &&  defined(AV_WB48)
#       define AV_WN48(p, v) AV_WB48(p, v)
#   endif

#   if    defined(AV_RN64) && !defined(AV_RB64)
#       define AV_RB64(p) AV_RN64(p)
#   elif !defined(AV_RN64) &&  defined(AV_RB64)
#       define AV_RN64(p) AV_RB64(p)
#   endif

#   if    defined(AV_WN64) && !defined(AV_WB64)
#       define AV_WB64(p, v) AV_WN64(p, v)
#   elif !defined(AV_WN64) &&  defined(AV_WB64)
#       define AV_WN64(p, v) AV_WB64(p, v)
#   endif

#else /* AV_HAVE_BIGENDIAN */

#   if    defined(AV_RN16) && !defined(AV_RL16)
#       define AV_RL16(p) AV_RN16(p)
#   elif !defined(AV_RN16) &&  defined(AV_RL16)
#       define AV_RN16(p) AV_RL16(p)
#   endif

#   if    defined(AV_WN16) && !defined(AV_WL16)
#       define AV_WL16(p, v) AV_WN16(p, v)
#   elif !defined(AV_WN16) &&  defined(AV_WL16)
#       define AV_WN16(p, v) AV_WL16(p, v)
#   endif

#   if    defined(AV_RN24) && !defined(AV_RL24)
#       define AV_RL24(p) AV_RN24(p)
#   elif !defined(AV_RN24) &&  defined(AV_RL24)
#       define AV_RN24(p) AV_RL24(p)
#   endif

#   if    defined(AV_WN24) && !defined(AV_WL24)
#       define AV_WL24(p, v) AV_WN24(p, v)
#   elif !defined(AV_WN24) &&  defined(AV_WL24)
#       define AV_WN24(p, v) AV_WL24(p, v)
#   endif

#   if    defined(AV_RN32) && !defined(AV_RL32)
#       define AV_RL32(p) AV_RN32(p)
#   elif !defined(AV_RN32) &&  defined(AV_RL32)
#       define AV_RN32(p) AV_RL32(p)
#   endif

#   if    defined(AV_WN32) && !defined(AV_WL32)
#       define AV_WL32(p, v) AV_WN32(p, v)
#   elif !defined(AV_WN32) &&  defined(AV_WL32)
#       define AV_WN32(p, v) AV_WL32(p, v)
#   endif

#   if    defined(AV_RN48) && !defined(AV_RL48)
#       define AV_RL48(p) AV_RN48(p)
#   elif !defined(AV_RN48) &&  defined(AV_RL48)
#       define AV_RN48(p) AV_RL48(p)
#   endif

#   if    defined(AV_WN48) && !defined(AV_WL48)
#       define AV_WL48(p, v) AV_WN48(p, v)
#   elif !defined(AV_WN48) &&  defined(AV_WL48)
#       define AV_WN48(p, v) AV_WL48(p, v)
#   endif

#   if    defined(AV_RN64) && !defined(AV_RL64)
#       define AV_RL64(p) AV_RN64(p)
#   elif !defined(AV_RN64) &&  defined(AV_RL64)
#       define AV_RN64(p) AV_RL64(p)
#   endif

#   if    defined(AV_WN64) && !defined(AV_WL64)
#       define AV_WL64(p, v) AV_WN64(p, v)
#   elif !defined(AV_WN64) &&  defined(AV_WL64)
#       define AV_WN64(p, v) AV_WL64(p, v)
#   endif

#endif /* !AV_HAVE_BIGENDIAN */

/*
 * Define AV_[RW]N helper macros to simplify definitions not provided
 * by per-arch headers.
 * 定义 AV_[RW]N 辅助宏来简化每个架构标头未提供的定义。
 */
/**
 *
#if defined(__GNUC__) 是一个预处理指令，用于检查当前编译环境是否定义了 __GNUC__ 这个宏。

解释：
#if defined(...) 是 C/C++ 中的条件编译指令，用于在编译预处理阶段根据指定的条件来选择性地编译代码。
defined(__GNUC__) 判断是否定义了 __GNUC__ 宏，如果定义了，则条件成立。

__GNUC__ 宏：
__GNUC__ 是 GCC（GNU Compiler Collection）编译器定义的一个预定义宏。它是 GCC 编译器的版本号，用于标识代码是否在
GCC 编译环境中编译。

示例用法：

#if defined(__GNUC__)
    // 在 GCC 编译器下的特定代码
    // 可以使用 GCC 特定的特性或语法
    #pragma GCC diagnostic ignored "-Wunused-function"
    static void unused_function() {  ...  }
#endif

解释示例：
上面的示例代码在检测到当前编译环境是 GCC 时，使用 #pragma GCC diagnostic 指令来忽略未使用函数的警告。
这种方式允许在不同的编译器下编写特定于编译器的代码，以利用其特定的功能或优化。

注意事项：
__GNUC__ 宏通常在 GCC 编译器及其兼容的编译器（如 Clang）中定义。
如果需要跨平台编译，建议使用条件编译来处理不同编译器的特性差异。

总之，#if defined(__GNUC__) 是一个常见的条件编译指令，用于在代码中检查是否在 GCC 编译器环境下编译，并根据结果执行
相应的编译选项或代码段。
*/
#if defined(__GNUC__)

/**
union unaligned_64 { uint64_t l; } attribute((packed)) av_alias;

ChatGPT
这段代码定义了一个名为 unaligned_64 的联合体 union，其中包含一个 uint64_t 类型的成员 l。同时使用
了 __attribute__((packed)) 和 av_alias 属性来影响联合体的布局和别名处理。

解释：
联合体定义：

union unaligned_64 {
    uint64_t l;
} __attribute__((packed)) av_alias;

union unaligned_64 { uint64_t l; } 定义了一个联合体，其中只有一个成员 l，类型为 uint64_t，即占用 8 字节的无符号整数。

__attribute__((packed)) 属性：
__attribute__((packed)) 是 GCC 和 Clang 中的一个属性，用于告知编译器取消结构体或联合体的内存对齐，
即不要为了对齐要求而填充空白字节。
在这里，它被用来确保 union unaligned_64 的 l 成员没有额外的填充字节，直接紧密地按照其实际大小分配内存空间。

av_alias 属性：
av_alias 是一个宏或标识符，可能代表了 __attribute__((may_alias)) 或类似的属性，用于指示编译器允许将指定类型
的指针视为可能别名的指针。
这里的 av_alias 被应用在联合体上，可能表明 unaligned_64 联合体的成员 l 可以被视为可能别名的指针，
从而影响指针操作的优化和行为。

使用场景：
这样的联合体定义在需要处理底层硬件通信或需要直接访问特定内存布局的数据时非常有用。
packed 属性确保了在结构体或联合体中没有额外的字节填充，节省了内存空间并且保持了数据的紧凑性。
av_alias 属性允许在需要时将 unaligned_64 联合体的成员 l 视为可能别名的指针，这在某些低级编程场景中可能非常实用。

总之，这段代码展示了如何定义一个紧凑且具有别名属性的联合体，在需要处理精确的内存布局和数据访问时特别有用。
*/
/**
这段代码片段是在使用 GCC 编译器时，定义了一些结构体和宏来处理字节对齐和读写操作。让我为你解释一下每个部分的作用和含义：

定义 union unaligned_64, union unaligned_32, union unaligned_16:
这些是联合体（union），用于在不进行字节对齐的情况下访问特定大小的整数类型（uint64_t, uint32_t, uint16_t）。
关键字 __attribute__((packed)) 告诉编译器不要对这些联合体进行字节对齐优化，即使用最小的存储空间。

宏定义 AV_RN(s, p):
这个宏用于读取指针 p 处的 s 位（或字节）的数据，并返回其值。它使用了先前定义的 union unaligned_##s 来访问非对齐的数据。
示例使用：AV_RN(64, ptr) 将读取 ptr 指向的 64 位数据，AV_RN(32, ptr) 将读取 32 位数据，依此类推。

宏定义 AV_WN(s, p, v):
这个宏用于将值 v 写入到指针 p 处的 s 位（或字节）的位置。同样使用了先前定义的 union unaligned_##s 来进行非对齐的写操作。

示例使用：AV_WN(64, ptr, value) 将值 value 写入 ptr 指向的 64 位数据位置，AV_WN(32, ptr, value) 将写入 32 位数据，依此类推。

示例使用：
uint8_t buffer[8];
uint64_t value = 0x123456789abcdef0;
AV_WN(64, buffer, value);  // 写入 64 位值到 buffer
uint64_t read_value = AV_RN(64, buffer);  // 从 buffer 中读取 64 位值

解释：
这些宏和联合体的目的是为了处理在特定环境中需要直接操作内存中非对齐数据的情况。在一些嵌入式系统或者特定的数据处理场景中，
需要直接控制数据的存储方式和字节对齐，这种方式能够绕过编译器的默认优化，确保数据的正确读写。
*/
union unaligned_64 { uint64_t l; } __attribute__((packed)) av_alias;
union unaligned_32 { uint32_t l; } __attribute__((packed)) av_alias;
union unaligned_16 { uint16_t l; } __attribute__((packed)) av_alias;

#   define AV_RN(s, p) (((const union unaligned_##s *) (p))->l)
#   define AV_WN(s, p, v) ((((union unaligned_##s *) (p))->l) = (v))
/**
这段代码是在使用微软编译器（MSVC）时，为特定的处理器架构（ARM、X64、ARM64）和启用了快速非对齐访问（AV_HAVE_FAST_UNALIGNED）
的情况下，定义了用于非对齐数据读写操作的宏。下面是对这段代码的具体解释：

代码解析：

#elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_X64) || defined(_M_ARM64)) && AV_HAVE_FAST_UNALIGNED

#   define AV_RN(s, p) (*((const __unaligned uint##s##_t*)(p)))
#   define AV_WN(s, p, v) (*((__unaligned uint##s##_t*)(p)) = (v))

条件编译：
#elif defined(_MSC_VER): 检查是否使用 Microsoft 编译器（MSVC）。
&& (defined(_M_ARM) || defined(_M_X64) || defined(_M_ARM64)): 检查是否为 ARM、X64 或 ARM64 处理器架构。
&& AV_HAVE_FAST_UNALIGNED: 检查是否启用了快速非对齐访问。

宏定义 AV_RN(s, p)：
#define AV_RN(s, p) (*((const __unaligned uint##s##_t*)(p)))
这个宏用于读取指针 p 处的 s 位数据，并返回其值。

__unaligned 是 Microsoft 编译器特有的关键字，用于指示指针 p 所指向的数据可能是非对齐的。
uint##s##_t 是一种宏拼接技术，s 可以是 16、32 或 64，这样可以生成 uint16_t、uint32_t 或 uint64_t。

宏定义 AV_WN(s, p, v)：
#define AV_WN(s, p, v) (*((__unaligned uint##s##_t*)(p)) = (v))
这个宏用于将值 v 写入到指针 p 处的 s 位位置。
使用 __unaligned 关键字指示数据可能是非对齐的。

示例使用：
#include <stdint.h>
#include <stdio.h>

// 假设 AV_HAVE_FAST_UNALIGNED 已经定义
#define AV_HAVE_FAST_UNALIGNED 1

// 使用条件编译来定义 AV_RN 和 AV_WN 宏
#if defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_X64) || defined(_M_ARM64)) && AV_HAVE_FAST_UNALIGNED
#   define AV_RN(s, p) (*((const __unaligned uint##s##_t*)(p)))
#   define AV_WN(s, p, v) (*((__unaligned uint##s##_t*)(p)) = (v))
#endif

int main() {
    uint8_t buffer[8];
    uint64_t value = 0x123456789abcdef0;

    // 写入 64 位值到 buffer
    AV_WN(64, buffer, value);
    // 从 buffer 中读取 64 位值
    uint64_t read_value = AV_RN(64, buffer);

    printf("Written value: 0x%" PRIx64 "\n", value);
    printf("Read value: 0x%" PRIx64 "\n", read_value);

    return 0;
}

解释：

条件编译：
这部分代码仅在使用 Microsoft 编译器（MSVC）并且目标架构是 ARM、X64 或 ARM64 且启用了快速非对齐访问时才会生效。
如果这些条件都满足，则定义 AV_RN 和 AV_WN 宏。

宏定义：
AV_RN(s, p): 读取指针 p 处的 s 位（16、32、64）数据。
AV_WN(s, p, v): 将值 v 写入到指针 p 处的 s 位（16、32、64）位置。
使用 __unaligned 关键字确保编译器生成适当的代码来处理非对齐的数据访问。

示例使用：
示例代码中定义了一个 8 字节的缓冲区 buffer，并使用 AV_WN(64, buffer, value) 将 64 位的 value 写入 buffer 中。
然后使用 AV_RN(64, buffer) 从 buffer 中读取 64 位的值，并将其打印出来。
这展示了如何在非对齐内存访问的情况下使用这些宏进行读写操作。

总结：
这种条件编译和宏定义方式确保在特定的编译器和处理器架构下，能够正确处理非对齐的数据访问。
__unaligned 关键字告诉编译器要生成适当的代码来处理非对齐的内存访问，从而避免潜在的问题。
这种方式提高了代码的可移植性和可靠性，特别是在需要处理非对齐数据访问的嵌入式系统或特定的数据处理场景中。
*/
#elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_X64) || defined(_M_ARM64)) && AV_HAVE_FAST_UNALIGNED

#   define AV_RN(s, p) (*((const __unaligned uint##s##_t*)(p)))
#   define AV_WN(s, p, v) (*((__unaligned uint##s##_t*)(p)) = (v))

#elif AV_HAVE_FAST_UNALIGNED

#   define AV_RN(s, p) (((const av_alias##s*)(p))->u##s)
#   define AV_WN(s, p, v) (((av_alias##s*)(p))->u##s = (v))

#else

#ifndef AV_RB16
#   define AV_RB16(x)                           \
    ((((const uint8_t*)(x))[0] << 8) |          \
      ((const uint8_t*)(x))[1])
#endif
#ifndef AV_WB16
#   define AV_WB16(p, val) do {                 \
        uint16_t d = (val);                     \
        ((uint8_t*)(p))[1] = (d);               \
        ((uint8_t*)(p))[0] = (d)>>8;            \
    } while(0)
#endif

#ifndef AV_RL16
#   define AV_RL16(x)                           \
    ((((const uint8_t*)(x))[1] << 8) |          \
      ((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL16
#   define AV_WL16(p, val) do {                 \
        uint16_t d = (val);                     \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
    } while(0)
#endif

#ifndef AV_RB32
#   define AV_RB32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
               (((const uint8_t*)(x))[1] << 16) |    \
               (((const uint8_t*)(x))[2] <<  8) |    \
                ((const uint8_t*)(x))[3])
#endif
#ifndef AV_WB32
#   define AV_WB32(p, val) do {                 \
        uint32_t d = (val);                     \
        ((uint8_t*)(p))[3] = (d);               \
        ((uint8_t*)(p))[2] = (d)>>8;            \
        ((uint8_t*)(p))[1] = (d)>>16;           \
        ((uint8_t*)(p))[0] = (d)>>24;           \
    } while(0)
#endif

#ifndef AV_RL32
#   define AV_RL32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[3] << 24) |    \
               (((const uint8_t*)(x))[2] << 16) |    \
               (((const uint8_t*)(x))[1] <<  8) |    \
                ((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL32
#   define AV_WL32(p, val) do {                 \
        uint32_t d = (val);                     \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
    } while(0)
#endif

#ifndef AV_RB64
#   define AV_RB64(x)                                   \
    (((uint64_t)((const uint8_t*)(x))[0] << 56) |       \
     ((uint64_t)((const uint8_t*)(x))[1] << 48) |       \
     ((uint64_t)((const uint8_t*)(x))[2] << 40) |       \
     ((uint64_t)((const uint8_t*)(x))[3] << 32) |       \
     ((uint64_t)((const uint8_t*)(x))[4] << 24) |       \
     ((uint64_t)((const uint8_t*)(x))[5] << 16) |       \
     ((uint64_t)((const uint8_t*)(x))[6] <<  8) |       \
      (uint64_t)((const uint8_t*)(x))[7])
#endif
#ifndef AV_WB64
#   define AV_WB64(p, val) do {                 \
        uint64_t d = (val);                     \
        ((uint8_t*)(p))[7] = (d);               \
        ((uint8_t*)(p))[6] = (d)>>8;            \
        ((uint8_t*)(p))[5] = (d)>>16;           \
        ((uint8_t*)(p))[4] = (d)>>24;           \
        ((uint8_t*)(p))[3] = (d)>>32;           \
        ((uint8_t*)(p))[2] = (d)>>40;           \
        ((uint8_t*)(p))[1] = (d)>>48;           \
        ((uint8_t*)(p))[0] = (d)>>56;           \
    } while(0)
#endif

#ifndef AV_RL64
#   define AV_RL64(x)                                   \
    (((uint64_t)((const uint8_t*)(x))[7] << 56) |       \
     ((uint64_t)((const uint8_t*)(x))[6] << 48) |       \
     ((uint64_t)((const uint8_t*)(x))[5] << 40) |       \
     ((uint64_t)((const uint8_t*)(x))[4] << 32) |       \
     ((uint64_t)((const uint8_t*)(x))[3] << 24) |       \
     ((uint64_t)((const uint8_t*)(x))[2] << 16) |       \
     ((uint64_t)((const uint8_t*)(x))[1] <<  8) |       \
      (uint64_t)((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL64
#   define AV_WL64(p, val) do {                 \
        uint64_t d = (val);                     \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
        ((uint8_t*)(p))[4] = (d)>>32;           \
        ((uint8_t*)(p))[5] = (d)>>40;           \
        ((uint8_t*)(p))[6] = (d)>>48;           \
        ((uint8_t*)(p))[7] = (d)>>56;           \
    } while(0)
#endif

#if AV_HAVE_BIGENDIAN
#   define AV_RN(s, p)    AV_RB##s(p)
#   define AV_WN(s, p, v) AV_WB##s(p, v)
#else
#   define AV_RN(s, p)    AV_RL##s(p)
#   define AV_WN(s, p, v) AV_WL##s(p, v)
#endif

#endif /* HAVE_FAST_UNALIGNED */

#ifndef AV_RN16
#   define AV_RN16(p) AV_RN(16, p)
#endif

#ifndef AV_RN32
#   define AV_RN32(p) AV_RN(32, p)
#endif

#ifndef AV_RN64
#   define AV_RN64(p) AV_RN(64, p)
#endif

#ifndef AV_WN16
#   define AV_WN16(p, v) AV_WN(16, p, v)
#endif

#ifndef AV_WN32
#   define AV_WN32(p, v) AV_WN(32, p, v)
#endif

#ifndef AV_WN64
#   define AV_WN64(p, v) AV_WN(64, p, v)
#endif

#if AV_HAVE_BIGENDIAN
#   define AV_RB(s, p)    AV_RN##s(p)
#   define AV_WB(s, p, v) AV_WN##s(p, v)
#   define AV_RL(s, p)    av_bswap##s(AV_RN##s(p))
#   define AV_WL(s, p, v) AV_WN##s(p, av_bswap##s(v))
#else
#   define AV_RB(s, p)    av_bswap##s(AV_RN##s(p))
#   define AV_WB(s, p, v) AV_WN##s(p, av_bswap##s(v))
#   define AV_RL(s, p)    AV_RN##s(p)
#   define AV_WL(s, p, v) AV_WN##s(p, v)
#endif

#define AV_RB8(x)     (((const uint8_t*)(x))[0])
#define AV_WB8(p, d)  do { ((uint8_t*)(p))[0] = (d); } while(0)

#define AV_RL8(x)     AV_RB8(x)
#define AV_WL8(p, d)  AV_WB8(p, d)

#ifndef AV_RB16
#   define AV_RB16(p)    AV_RB(16, p)
#endif
#ifndef AV_WB16
#   define AV_WB16(p, v) AV_WB(16, p, v)
#endif

#ifndef AV_RL16
#   define AV_RL16(p)    AV_RL(16, p)
#endif
#ifndef AV_WL16
#   define AV_WL16(p, v) AV_WL(16, p, v)
#endif

#ifndef AV_RB32
#   define AV_RB32(p)    AV_RB(32, p)
#endif
#ifndef AV_WB32
#   define AV_WB32(p, v) AV_WB(32, p, v)
#endif

#ifndef AV_RL32
#   define AV_RL32(p)    AV_RL(32, p)
#endif
#ifndef AV_WL32
#   define AV_WL32(p, v) AV_WL(32, p, v)
#endif

#ifndef AV_RB64
#   define AV_RB64(p)    AV_RB(64, p)
#endif
#ifndef AV_WB64
#   define AV_WB64(p, v) AV_WB(64, p, v)
#endif

#ifndef AV_RL64
#   define AV_RL64(p)    AV_RL(64, p)
#endif
#ifndef AV_WL64
#   define AV_WL64(p, v) AV_WL(64, p, v)
#endif

#ifndef AV_RB24
#   define AV_RB24(x)                           \
    ((((const uint8_t*)(x))[0] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[2])
#endif
#ifndef AV_WB24
#   define AV_WB24(p, d) do {                   \
        ((uint8_t*)(p))[2] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[0] = (d)>>16;           \
    } while(0)
#endif

#ifndef AV_RL24
#   define AV_RL24(x)                           \
    ((((const uint8_t*)(x))[2] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL24
#   define AV_WL24(p, d) do {                   \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
    } while(0)
#endif

#ifndef AV_RB48
#   define AV_RB48(x)                                     \
    (((uint64_t)((const uint8_t*)(x))[0] << 40) |         \
     ((uint64_t)((const uint8_t*)(x))[1] << 32) |         \
     ((uint64_t)((const uint8_t*)(x))[2] << 24) |         \
     ((uint64_t)((const uint8_t*)(x))[3] << 16) |         \
     ((uint64_t)((const uint8_t*)(x))[4] <<  8) |         \
      (uint64_t)((const uint8_t*)(x))[5])
#endif
#ifndef AV_WB48
#   define AV_WB48(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[5] = (d);               \
        ((uint8_t*)(p))[4] = (d)>>8;            \
        ((uint8_t*)(p))[3] = (d)>>16;           \
        ((uint8_t*)(p))[2] = (d)>>24;           \
        ((uint8_t*)(p))[1] = (d)>>32;           \
        ((uint8_t*)(p))[0] = (d)>>40;           \
    } while(0)
#endif

#ifndef AV_RL48
#   define AV_RL48(x)                                     \
    (((uint64_t)((const uint8_t*)(x))[5] << 40) |         \
     ((uint64_t)((const uint8_t*)(x))[4] << 32) |         \
     ((uint64_t)((const uint8_t*)(x))[3] << 24) |         \
     ((uint64_t)((const uint8_t*)(x))[2] << 16) |         \
     ((uint64_t)((const uint8_t*)(x))[1] <<  8) |         \
      (uint64_t)((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL48
#   define AV_WL48(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
        ((uint8_t*)(p))[4] = (d)>>32;           \
        ((uint8_t*)(p))[5] = (d)>>40;           \
    } while(0)
#endif

/*
 * The AV_[RW]NA macros access naturally aligned data
 * in a type-safe way.
 * AV_[RW]NA 宏以类型安全的方式访问自然对齐的数据。
 */
/**
ffmpeg的intreadwrite.h文件中AV_RN32A与AV_RN32的却别是什么？分别在什么场景下使用。

ChatGPT
在 FFmpeg 的 intreadwrite.h 文件中，AV_RN32A 和 AV_RN32 是用于读取 32 位整数的宏，
但它们有不同的使用场景和实现细节。具体的区别和使用场景如下：

AV_RN32 和 AV_RN32A 的区别：

AV_RN32：
AV_RN32 是一个通用宏，用于从内存中的特定位置读取一个 32 位的整数。
它处理的是标准的对齐或非对齐的内存访问，取决于具体实现。
通常用法：读取一个 32 位整数，适用于不确定内存对齐的场景。

AV_RN32A：
AV_RN32A 是一个专门用于对齐访问的宏，假设访问的内存地址是对齐的。
在对齐的内存访问场景下，它可以使用更高效的加载指令，从而提高性能。
通常用法：在可以保证内存地址是对齐的情况下使用，适用于对性能要求较高的场景。

实现细节：
这些宏的具体实现可能会根据编译器、架构和平台的不同而有所变化。下面是一些可能的实现方式：

// AV_RN32 - 读取32位整数，处理非对齐和对齐访问
#define AV_RN32(p) \
    ((((const uint8_t*)(p))[0] << 24) | \
     (((const uint8_t*)(p))[1] << 16) | \
     (((const uint8_t*)(p))[2] << 8)  | \
      ((const uint8_t*)(p))[3])

// AV_RN32A - 读取对齐的32位整数，更高效的实现
#define AV_RN32A(p) (*((const uint32_t*)(p)))

在这种实现中：
AV_RN32 是一个逐字节处理的宏，适用于任何内存地址，即使这些地址不是 32 位对齐的。
AV_RN32A 是一个直接使用 uint32_t 指针进行访问的宏，假设内存地址是对齐的，从而可以更高效地读取数据。

使用场景：

AV_RN32 的使用场景：
适用于需要处理可能非对齐内存的场景。
适合于一般的数据解析和处理，特别是在不能保证内存对齐的情况下。
例如，在解析网络数据包或处理从文件中读取的原始数据时，可能无法确保数据地址是对齐的，此时使用 AV_RN32 更为安全。

AV_RN32A 的使用场景：
适用于可以确保内存地址是对齐的场景。
适合于高性能需求的应用，例如处理器缓存行对齐的数据访问。
例如，在图像处理、视频解码等性能关键的应用中，如果能够确保数据对齐，使用 AV_RN32A 可以获得更高的性能。

总结：
AV_RN32：通用的 32 位整数读取宏，适用于任何内存地址（对齐或非对齐），安全但可能效率较低。
AV_RN32A：专用于对齐内存地址的 32 位整数读取宏，高效但仅适用于对齐内存的场景。

根据具体的应用需求和内存访问模式选择适当的宏，可以在保证正确性的同时，优化性能。
*/

#define AV_RNA(s, p)    (((const av_alias##s*)(p))->u##s)
#define AV_WNA(s, p, v) (((av_alias##s*)(p))->u##s = (v))

#ifndef AV_RN16A
#   define AV_RN16A(p) AV_RNA(16, p)
#endif

#ifndef AV_RN32A
#   define AV_RN32A(p) AV_RNA(32, p)
#endif

#ifndef AV_RN64A
#   define AV_RN64A(p) AV_RNA(64, p)
#endif

#ifndef AV_WN16A
#   define AV_WN16A(p, v) AV_WNA(16, p, v)
#endif

#ifndef AV_WN32A
#   define AV_WN32A(p, v) AV_WNA(32, p, v)
#endif

#ifndef AV_WN64A
#   define AV_WN64A(p, v) AV_WNA(64, p, v)
#endif

#if AV_HAVE_BIGENDIAN
#   define AV_RLA(s, p)    av_bswap##s(AV_RN##s##A(p))
#   define AV_WLA(s, p, v) AV_WN##s##A(p, av_bswap##s(v))
#else
#   define AV_RLA(s, p)    AV_RN##s##A(p)
#   define AV_WLA(s, p, v) AV_WN##s##A(p, v)
#endif

#ifndef AV_RL64A
#   define AV_RL64A(p) AV_RLA(64, p)
#endif
#ifndef AV_WL64A
#   define AV_WL64A(p, v) AV_WLA(64, p, v)
#endif

/*
 * The AV_COPYxxU macros are suitable for copying data to/from unaligned
 * memory locations.
 * AV_COPYxxU 宏适用于从未对齐的内存位置复制数据或将数据复制到未对齐的内存位置。
 */

/**
ffmpeg的intreadwrite.h文件中AV_COPY和AV_COPYU的区别是什么？分别在什么场景下使用？

ChatGPT
在 FFmpeg 的 intreadwrite.h 文件中，AV_COPY 和 AV_COPYU 是用于复制数据的宏，但它们在处理
对齐和非对齐内存方面有所不同。让我们详细探讨它们的区别和适用场景。

AV_COPY 和 AV_COPYU 的区别：

AV_COPY：
AV_COPY 用于对齐的数据复制。
假设源地址和目标地址都是对齐的，因此可以使用更高效的内存复制操作。

AV_COPYU：
AV_COPYU 用于非对齐的数据复制。

处理可能存在的内存对齐问题，确保即使在非对齐的内存地址上也能正确地复制数据。

实现细节（示例）：

以下是可能的宏定义方式：

// AV_COPYU - 复制非对齐的数据
#define AV_COPYU(d, s, n) \
    do { \
        for (size_t i = 0; i < (n); i++) { \
            ((uint8_t*)(d))[i] = ((const uint8_t*)(s))[i]; \
        } \
    } while (0)

// AV_COPY - 复制对齐的数据
#define AV_COPY(d, s, n) \
    memcpy((d), (s), (n))

在这种实现中：
AV_COPYU 逐字节地复制数据，适用于非对齐的内存地址。
AV_COPY 使用标准库函数 memcpy 进行复制，假设数据是对齐的，从而可以更高效地完成复制。

使用场景：

AV_COPYU 的使用场景：
适用于需要处理非对齐内存地址的数据复制场景。
适用于从网络数据包、文件等读取的数据，无法保证内存地址对齐的情况下。

例如，在解析网络数据包、处理文件 I/O 操作时，可能无法保证数据的对齐，此时使用 AV_COPYU 可以确保数据的正确性。

AV_COPY 的使用场景：
适用于可以确保内存地址是对齐的数据复制场景。
适用于高性能需求的应用，例如处理器缓存行对齐的数据复制。
例如，在图像处理、视频解码等性能关键的应用中，如果能够确保数据对齐，使用 AV_COPY 可以获得更高的性能。

总结：
AV_COPYU：用于非对齐的数据复制，逐字节处理，确保数据的正确性，但效率可能较低。
AV_COPY：用于对齐的数据复制，使用 memcpy 等高效函数，效率较高，但仅适用于对齐内存地址的场景。

根据具体的应用需求和内存访问模式选择适当的宏，可以在保证正确性的同时，优化性能。
*/
#define AV_COPYU(n, d, s) AV_WN##n(d, AV_RN##n(s));

#ifndef AV_COPY16U
#   define AV_COPY16U(d, s) AV_COPYU(16, d, s)
#endif

#ifndef AV_COPY32U
#   define AV_COPY32U(d, s) AV_COPYU(32, d, s)
#endif

#ifndef AV_COPY64U
#   define AV_COPY64U(d, s) AV_COPYU(64, d, s)
#endif

#ifndef AV_COPY128U
#   define AV_COPY128U(d, s)                                    \
    do {                                                        \
        AV_COPY64U(d, s);                                       \
        AV_COPY64U((char *)(d) + 8, (const char *)(s) + 8);     \
    } while(0)
#endif

/* Parameters for AV_COPY*, AV_SWAP*, AV_ZERO* must be
 * naturally aligned. They may be implemented using MMX,
 * so emms_c() must be called before using any float code
 * afterwards.
 * AV_COPY*、AV_SWAP*、AV_ZERO* 的参数必须自然对齐。它们可能使用 MMX 实现，因此在使用任何浮点
 * 代码之前必须调用 emms_c()。
 */

#define AV_COPY(n, d, s) \
    (((av_alias##n*)(d))->u##n = ((const av_alias##n*)(s))->u##n)

#ifndef AV_COPY16
#   define AV_COPY16(d, s) AV_COPY(16, d, s)
#endif

#ifndef AV_COPY32
#   define AV_COPY32(d, s) AV_COPY(32, d, s)
#endif

#ifndef AV_COPY64
#   define AV_COPY64(d, s) AV_COPY(64, d, s)
#endif

#ifndef AV_COPY128
#   define AV_COPY128(d, s)                    \
    do {                                       \
        AV_COPY64(d, s);                       \
        AV_COPY64((char*)(d)+8, (char*)(s)+8); \
    } while(0)
#endif

#define AV_SWAP(n, a, b) FFSWAP(av_alias##n, *(av_alias##n*)(a), *(av_alias##n*)(b))

#ifndef AV_SWAP64
#   define AV_SWAP64(a, b) AV_SWAP(64, a, b)
#endif

#define AV_ZERO(n, d) (((av_alias##n*)(d))->u##n = 0)

#ifndef AV_ZERO16
#   define AV_ZERO16(d) AV_ZERO(16, d)
#endif

#ifndef AV_ZERO32
#   define AV_ZERO32(d) AV_ZERO(32, d)
#endif

#ifndef AV_ZERO64
#   define AV_ZERO64(d) AV_ZERO(64, d)
#endif

#ifndef AV_ZERO128
#   define AV_ZERO128(d)         \
    do {                         \
        AV_ZERO64(d);            \
        AV_ZERO64((char*)(d)+8); \
    } while(0)
#endif

#endif /* AVUTIL_INTREADWRITE_H */
