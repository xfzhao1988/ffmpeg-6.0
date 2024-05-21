/*
 * Copyright (c) 2012 Nicolas George
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
 * @ingroup lavu_avbprint
 * AVBPrint public header
 */

#ifndef AVUTIL_BPRINT_H
#define AVUTIL_BPRINT_H

#include <stdarg.h>

#include "attributes.h"
#include "avstring.h"

/**
 * @defgroup lavu_avbprint AVBPrint
 * @ingroup lavu_data
 *
 * A buffer to print data progressively
 * 渐进式打印数据的缓冲区
 * @{
 */

/**
 * Define a structure with extra padding to a fixed size
 * This helps ensuring binary compatibility with future versions.
 * 定义一个具有固定大小的额外填充的结构这有助于确保与未来版本的二进制兼容性。
 */
//done
#define FF_PAD_STRUCTURE(name, size, ...) \
struct ff_pad_helper_##name { __VA_ARGS__ }; \
typedef struct name { \
    __VA_ARGS__ \
    char reserved_padding[size - sizeof(struct ff_pad_helper_##name)]; \
} name;

/**
 * Buffer to print data progressively
 * 渐进式打印数据的缓冲区
 *
 * The string buffer grows as necessary and is always 0-terminated.
 * 字符串缓冲区根据需要增长，并且始终以 0 结尾。
 * The content of the string is never accessed, and thus is
 * encoding-agnostic and can even hold binary data.
 * 字符串的内容永远不会被访问，因此与编码无关，甚至可以保存二进制数据。
 *
 * Small buffers are kept in the structure itself, and thus require no
 * memory allocation at all (unless the contents of the buffer is needed
 * after the structure goes out of scope). This is almost as lightweight as
 * declaring a local `char buf[512]`.
 * 小缓冲区保留在结构本身中，因此根本不需要内存分配（除非结构超出范围后需要缓冲区的内容）。
 * 这几乎与声明本地“char buf[512]”一样轻量。
 *
 * The length of the string can go beyond the allocated size: the buffer is
 * then truncated, but the functions still keep account of the actual total
 * length.
 * 字符串的长度可以超出分配的大小：然后缓冲区会被截断，但函数仍然会考虑实际的总长度。
 *
 * In other words, AVBPrint.len can be greater than AVBPrint.size and records
 * the total length of what would have been to the buffer if there had been
 * enough memory.
 * 换句话说，AVBPrint.len 可以大于 AVBPrint.size，并记录缓冲区的总长度（如果有足够的内存）。
 *
 * Append operations do not need to be tested for failure: if a memory
 * allocation fails, data stop being appended to the buffer, but the length
 * is still updated. This situation can be tested with
 * av_bprint_is_complete().
 * 追加操作不需要测试失败：如果内存分配失败，数据将停止追加到缓冲区，但长度仍然会更新。
 * 这种情况可以用av_bprint_is_complete()来测试。
 *
 * The AVBPrint.size_max field determines several possible behaviours:
 * AVBPrint.size_max 字段确定几种可能的行为：
 * - `size_max = -1` (= `UINT_MAX`) or any large value will let the buffer be
 *   reallocated as necessary, with an amortized linear cost.
 *   `size_max = -1` (= `UINT_MAX`) 或任何较大的值都将使缓冲区根据需要重新分配，
 *    并具有摊销的线性成本。
 *
 * - `size_max = 0` prevents writing anything to the buffer: only the total
 *   length is computed. The write operations can then possibly be repeated in
 *   a buffer with exactly the necessary size
 *   (using `size_init = size_max = len + 1`).
 *   `size_max = 0` 阻止向缓冲区写入任何内容：仅计算总长度。 然后，可以在具有所需大
 *   小的缓冲区中重复写入操作（使用“size_init = size_max = len + 1”）。
 *
 * - `size_max = 1` is automatically replaced by the exact size available in the
 *   structure itself, thus ensuring no dynamic memory allocation. The
 *   internal buffer is large enough to hold a reasonable paragraph of text,
 *   such as the current paragraph.
 *   `size_max = 1` 会自动替换为结构本身可用的确切大小，从而确保没有动态内存分配。
 *   内部缓冲区足够大，可以容纳合理的文本段落，例如当前段落。
 */
//done
FF_PAD_STRUCTURE(AVBPrint, 1024,
    char *str;         /**< string so far */
    unsigned len;      /**< length so far */
    unsigned size;     /**< allocated memory */
    unsigned size_max; /**< maximum allocated memory */
    char reserved_internal_buffer[1];
)

/**
 * @name Max size special values 最大尺寸特殊值
 * Convenience macros for special values for av_bprint_init() size_max
 * parameter.
 * av_bprint_init() size_max 参数的特殊值的便捷宏。
 * @{
 */

/**
 * Buffer will be reallocated as necessary, with an amortized linear cost.
 * 缓冲区将根据需要重新分配，并具有摊销的线性成本。
 */
#define AV_BPRINT_SIZE_UNLIMITED  ((unsigned)-1)//done
/**
 * Use the exact size available in the AVBPrint structure itself.
 * 使用 AVBPrint 结构本身可用的确切大小。
 *
 * Thus ensuring no dynamic memory allocation. The internal buffer is large
 * enough to hold a reasonable paragraph of text, such as the current paragraph.
 * 从而确保没有动态内存分配。 内部缓冲区足够大，可以容纳合理的文本段落，例如当前段落。
 */
#define AV_BPRINT_SIZE_AUTOMATIC  1//done
/**
 * Do not write anything to the buffer, only calculate the total length.
 * 不向缓冲区写入任何内容，仅计算总长度。
 *
 * The write operations can then possibly be repeated in a buffer with
 * exactly the necessary size (using `size_init = size_max = AVBPrint.len + 1`).
 * 然后，可以在具有所需大小的缓冲区中重复写入操作（使用“size_init = size_max = AVBPrint.len + 1”）。
 */
#define AV_BPRINT_SIZE_COUNT_ONLY 0//done
/** @} */

/**
 * Init a print buffer.
 * 初始化打印缓冲区。
 *
 * @param buf        buffer to init 初始化缓冲区
 * @param size_init  initial size (including the final 0)
 *                   初始大小（包括最后的0）
 * @param size_max   maximum size; 最大size
 *                   - `0` means do not write anything, just count the length
 *                     ‘0’表示不写任何东西，只计算长度
 *
 *                   - `1` is replaced by the maximum value for automatic storage
 *                       any large value means that the internal buffer will be
 *                       reallocated as needed up to that limit
 *                     `1` 被自动存储的最大值替换，任何大值意味着内部缓冲区将根据需要重新分配，直至达到该限制
 *
 *                   - `-1` is converted to `UINT_MAX`, the largest limit possible.
 *                   Check also `AV_BPRINT_SIZE_*` macros.
 *                     `-1` 转换为 `UINT_MAX`，即可能的最大限制。 另请检查“AV_BPRINT_SIZE_*”宏。
 */
//done
void av_bprint_init(AVBPrint *buf, unsigned size_init, unsigned size_max);

/**
 * Init a print buffer using a pre-existing buffer.
 * 使用预先存在的缓冲区初始化打印缓冲区。
 *
 * The buffer will not be reallocated.
 * 缓冲区不会被重新分配。
 *
 * @param buf     buffer structure to init
 *                要初始化的缓冲区结构
 * @param buffer  byte buffer to use for the string data
 *                用于字符串数据的字节缓冲区
 * @param size    size of buffer
 *                缓冲区大小
 */
//done
void av_bprint_init_for_buffer(AVBPrint *buf, char *buffer, unsigned size);

/**
 * Append a formatted string to a print buffer.
 * 将格式化字符串附加到打印缓冲区。
 */
//done
void av_bprintf(AVBPrint *buf, const char *fmt, ...) av_printf_format(2, 3);

/**
 * Append a formatted string to a print buffer.
 * 将格式化字符串附加到打印缓冲区。
 */
//done
void av_vbprintf(AVBPrint *buf, const char *fmt, va_list vl_arg);

/**
 * Append char c n times to a print buffer.
 * 将 char c 追加到打印缓冲区 n 次。
 */
//done
void av_bprint_chars(AVBPrint *buf, char c, unsigned n);

/**
 * Append data to a print buffer.
 * 将数据附加到打印缓冲区。
 *
 * param buf  bprint buffer to use
 *            要使用的 bprint 缓冲区
 * param data pointer to data
 *            指向数据的指针
 * param size size of data
 *            数据大小
 */
//done
void av_bprint_append_data(AVBPrint *buf, const char *data, unsigned size);

struct tm;
/**
 * Append a formatted date and time to a print buffer.
 * 将格式化的日期和时间附加到打印缓冲区。
 *
 * param buf  bprint buffer to use
 *            要使用的bprint缓冲区
 * param fmt  date and time format string, see strftime()
 *            日期和时间格式字符串，请参阅strftime（）
 * param tm   broken-down time structure to translate
 *            分解时间结构进行转换
 *
 * @note due to poor design of the standard strftime function, it may
 * produce poor results if the format string expands to a very long text and
 * the bprint buffer is near the limit stated by the size_max option.
 * 由于标准strftime函数设计不好，如果格式字符串扩展为很长的文本，并且bprint缓冲区
 * 接近size_max选项规定的限制，则可能会产生较差的结果。
 */
//done
void av_bprint_strftime(AVBPrint *buf, const char *fmt, const struct tm *tm);

/**
 * Allocate bytes in the buffer for external use.
 * 在缓冲区中分配字节以供外部使用。
 *
 * @param[in]  buf          buffer structure
 *                          缓冲结构
 * @param[in]  size         required size
 *                          所需大小
 * @param[out] mem          pointer to the memory area
 *                          指向内存区域的指针
 * @param[out] actual_size  size of the memory area after allocation;
 *                          can be larger or smaller than size
 *                          分配后的内存区域的大小；可以大于或小于尺寸
 */
//done
void av_bprint_get_buffer(AVBPrint *buf, unsigned size,
                          unsigned char **mem, unsigned *actual_size);

/**
 * Reset the string to "" but keep internal allocated data.
 * 将字符串重置为“”，但保留内部分配的数据。
 */
//done
void av_bprint_clear(AVBPrint *buf);

/**
 * Test if the print buffer is complete (not truncated).
 * 测试打印缓冲区是否完整（未截断）。
 *
 * It may have been truncated due to a memory allocation failure
 * or the size_max limit (compare size and size_max if necessary).
 * 它可能由于内存分配失败或 size_max 限制而被截断（如有必要，请比较 size 和 size_max）。
 */
//done
static inline int av_bprint_is_complete(const AVBPrint *buf)
{
    return buf->len < buf->size;
}

/**
 * Finalize a print buffer.
 * 完成打印缓冲区。
 *
 * The print buffer can no longer be used afterwards,
 * but the len and size fields are still valid.
 * 此后打印缓冲区不能再使用，但 len 和 size 字段仍然有效。
 *
 * @arg[out] ret_str  if not NULL, used to return a permanent copy of the
 *                    buffer contents, or NULL if memory allocation fails;
 *                    if NULL, the buffer is discarded and freed
 * 如果不为 NULL，则用于返回缓冲区内容的永久副本，如果内存分配失败则返回 NULL；
 * 如果为 NULL，则丢弃并释放缓冲区
 * @return  0 for success or error code (probably AVERROR(ENOMEM))
 * 0 表示成功或错误代码（可能是 AVERROR(ENOMEM)）
 */
//done
int av_bprint_finalize(AVBPrint *buf, char **ret_str);

/**
 * Escape the content in src and append it to dstbuf.
 * 对 src 中的内容进行转义并将其附加到 dstbuf。
 *
 * @param dstbuf        already inited destination bprint buffer
 *                      已初始化目标 bprint 缓冲区
 * @param src           string containing the text to escape
 *                      包含要转义的文本的字符串
 * @param special_chars string containing the special characters which
 *                      need to be escaped, can be NULL
 *                      包含需要转义的特殊字符的字符串，可以为NULL
 * @param mode          escape mode to employ, see AV_ESCAPE_MODE_* macros.
 *                      Any unknown value for mode will be considered equivalent to
 *                      AV_ESCAPE_MODE_BACKSLASH, but this behaviour can change without
 *                      notice.
 *                      要使用的转义模式，请参阅 AV_ESCAPE_MODE_* 宏。模式的任何未知值都将被视为等同于
 *                      AV_ESCAPE_MODE_BACKSLASH，但此行为可能会更改，恕不另行通知。
 * @param flags         flags which control how to escape, see AV_ESCAPE_FLAG_* macros
 *                      控制如何转义的标志，请参阅 AV_ESCAPE_FLAG_* 宏
 */
//done
void av_bprint_escape(AVBPrint *dstbuf, const char *src, const char *special_chars,
                      enum AVEscapeMode mode, int flags);

/** @} */

#endif /* AVUTIL_BPRINT_H */
