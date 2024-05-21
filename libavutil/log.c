/*
 * log functions
 * Copyright (c) 2003 Michel Bardiaux
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
 * logging functions
 */

#include "config.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_IO_H
#include <io.h>
#endif
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bprint.h"
#include "common.h"
#include "internal.h"
#include "log.h"
#include "thread.h"
//done
static AVMutex mutex = AV_MUTEX_INITIALIZER;

//done
#define LINE_SZ 1024

#if HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>
/* this is the log level at which valgrind will output a full backtrace */
#define BACKTRACE_LOGLEVEL AV_LOG_ERROR
#endif

static int av_log_level = AV_LOG_INFO;//done
static int flags;//done

#define NB_LEVELS 8//done
#if defined(_WIN32) && HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
#include <windows.h>
static const uint8_t color[16 + AV_CLASS_CATEGORY_NB] = {
    [AV_LOG_PANIC  /8] = 12,
    [AV_LOG_FATAL  /8] = 12,
    [AV_LOG_ERROR  /8] = 12,
    [AV_LOG_WARNING/8] = 14,
    [AV_LOG_INFO   /8] =  7,
    [AV_LOG_VERBOSE/8] = 10,
    [AV_LOG_DEBUG  /8] = 10,
    [AV_LOG_TRACE  /8] = 8,
    [16+AV_CLASS_CATEGORY_NA              ] =  7,
    [16+AV_CLASS_CATEGORY_INPUT           ] = 13,
    [16+AV_CLASS_CATEGORY_OUTPUT          ] =  5,
    [16+AV_CLASS_CATEGORY_MUXER           ] = 13,
    [16+AV_CLASS_CATEGORY_DEMUXER         ] =  5,
    [16+AV_CLASS_CATEGORY_ENCODER         ] = 11,
    [16+AV_CLASS_CATEGORY_DECODER         ] =  3,
    [16+AV_CLASS_CATEGORY_FILTER          ] = 10,
    [16+AV_CLASS_CATEGORY_BITSTREAM_FILTER] =  9,
    [16+AV_CLASS_CATEGORY_SWSCALER        ] =  7,
    [16+AV_CLASS_CATEGORY_SWRESAMPLER     ] =  7,
    [16+AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT ] = 13,
    [16+AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT  ] = 5,
    [16+AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT ] = 13,
    [16+AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT  ] = 5,
    [16+AV_CLASS_CATEGORY_DEVICE_OUTPUT       ] = 13,
    [16+AV_CLASS_CATEGORY_DEVICE_INPUT        ] = 5,
};

static int16_t background, attr_orig;
static HANDLE con;
#else
/*
 * 这段代码定义了一个名为 color 的数组，用于存储不同日志级别和 AV 类别的颜色信息。
 * 这些颜色信息将在输出日志消息时用于设置终端文本的颜色，以便根据日志级别和 AV 类别在终端中显示不同颜色的日志消息。
 * 让我逐个解释这个数组的结构和含义：
 *
 * color 是一个静态的 uint32_t 类型数组，长度为 16 + AV_CLASS_CATEGORY_NB。它包含了所有可能的日志级别和 AV 类别的颜色信息。
 * 每个数组元素对应于一个特定的日志级别或 AV 类别。数组的索引值用于确定日志级别或 AV 类别，从而找到相应的颜色信息。
 * 颜色信息以 RGB 格式表示，使用 24 位无符号整数来表示红色、绿色和蓝色分量。具体地，颜色值通过位运算和位移操作从三个不同的源中提取。
 * 对于日志级别，颜色信息位于数组的前半部分。每个日志级别被除以 8，然后根据其所在数组索引的奇偶性和余数来确定红色、绿色和蓝色分量的值。
 * 对于 AV 类别，颜色信息位于数组的后半部分。AV 类别的值从 16 + AV_CLASS_CATEGORY_NA 开始，依次递增。同样，根据其所在数组索引
 * 的奇偶性和余数来确定红色、绿色和蓝色分量的值。
 * 这样设计的目的是使得不同的日志级别和 AV 类别在终端中以不同的颜色显示，以便用户更容易地识别和区分不同类型的日志消息。
 */
//done
static const uint32_t color[16 + AV_CLASS_CATEGORY_NB] = {
    [AV_LOG_PANIC  /8] =  52 << 16 | 196 << 8 | 0x41,
    [AV_LOG_FATAL  /8] = 208 <<  8 | 0x41,//这个表示前景色是208，背景色是0x41
    [AV_LOG_ERROR  /8] = 196 <<  8 | 0x11,
    [AV_LOG_WARNING/8] = 226 <<  8 | 0x03,
    [AV_LOG_INFO   /8] = 253 <<  8 | 0x09,
    [AV_LOG_VERBOSE/8] =  40 <<  8 | 0x02,
    [AV_LOG_DEBUG  /8] =  34 <<  8 | 0x02,
    [AV_LOG_TRACE  /8] =  34 <<  8 | 0x07,
    [16+AV_CLASS_CATEGORY_NA              ] = 250 << 8 | 0x09,
    [16+AV_CLASS_CATEGORY_INPUT           ] = 219 << 8 | 0x15,
    [16+AV_CLASS_CATEGORY_OUTPUT          ] = 201 << 8 | 0x05,
    [16+AV_CLASS_CATEGORY_MUXER           ] = 213 << 8 | 0x15,
    [16+AV_CLASS_CATEGORY_DEMUXER         ] = 207 << 8 | 0x05,
    [16+AV_CLASS_CATEGORY_ENCODER         ] =  51 << 8 | 0x16,
    [16+AV_CLASS_CATEGORY_DECODER         ] =  39 << 8 | 0x06,
    [16+AV_CLASS_CATEGORY_FILTER          ] = 155 << 8 | 0x12,
    [16+AV_CLASS_CATEGORY_BITSTREAM_FILTER] = 192 << 8 | 0x14,
    [16+AV_CLASS_CATEGORY_SWSCALER        ] = 153 << 8 | 0x14,
    [16+AV_CLASS_CATEGORY_SWRESAMPLER     ] = 147 << 8 | 0x14,
    [16+AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT ] = 213 << 8 | 0x15,
    [16+AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT  ] = 207 << 8 | 0x05,
    [16+AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT ] = 213 << 8 | 0x15,
    [16+AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT  ] = 207 << 8 | 0x05,
    [16+AV_CLASS_CATEGORY_DEVICE_OUTPUT       ] = 213 << 8 | 0x15,
    [16+AV_CLASS_CATEGORY_DEVICE_INPUT        ] = 207 << 8 | 0x05,
};

#endif
static int use_color = -1;//done

#if defined(_WIN32) && HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
static void win_console_puts(const char *str)
{
    const uint8_t *q = str;
    uint16_t line[LINE_SZ];

    while (*q) {
        uint16_t *buf = line;
        DWORD nb_chars = 0;
        DWORD written;

        while (*q && nb_chars < LINE_SZ - 1) {
            uint32_t ch;
            uint16_t tmp;

            GET_UTF8(ch, *q ? *q++ : 0, ch = 0xfffd; goto continue_on_invalid;)
continue_on_invalid:
            PUT_UTF16(ch, tmp, *buf++ = tmp; nb_chars++;)
        }

        WriteConsoleW(con, line, nb_chars, &written, NULL);
    }
}
#endif

/*
 * 这个函数 check_color_terminal 用于检查终端是否支持彩色输出，并根据检查结果设置是否使用彩色输出。
 * 让我来解释一下它的实现：
 *
 * 首先，通过调用 getenv("TERM") 获取环境变量 "TERM" 的值，以确定终端类型。
 * 接着，根据编译环境和操作系统的不同，使用不同的方法来判断终端是否支持彩色输出。在 Windows 系统上，
 * 通过调用 GetStdHandle 获取标准错误输出的句柄，并检查是否支持控制台模式。如果支持，
 * 获取控制台的屏幕缓冲区信息，保存原始属性和背景色信息。在类 Unix 系统上，通过调用 isatty
 * 函数检查标准错误输出是否连接到终端。如果是，表示终端支持彩色输出。
 *
 * 如果环境变量 "AV_LOG_FORCE_NOCOLOR" 被设置，则强制禁用彩色输出；如果环境变量 "AV_LOG_FORCE_COLOR" 被设置，
 * 则强制启用彩色输出。
 *
 * 如果既没有强制设置彩色输出也没有禁用彩色输出，根据终端类型和操作系统来决定是否使用彩色输出。
 * 如果终端连接到终端并且支持彩色输出，则启用彩色输出。
 *
 * 最后，如果环境变量 "AV_LOG_FORCE_256COLOR" 被设置，或者终端类型包含 "256color" 字符串，
 * 则将彩色输出设置为 256 色模式。
 *
 * 综上所述，这个函数通过检查终端类型和环境变量的设置，来动态地确定是否启用彩色输出，并设置相应的彩色输出模式。
 */
//done
static void check_color_terminal(void)
{
    char *term = getenv("TERM");

#if defined(_WIN32) && HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
    CONSOLE_SCREEN_BUFFER_INFO con_info;
    DWORD dummy;
    con = GetStdHandle(STD_ERROR_HANDLE);
    if (con != INVALID_HANDLE_VALUE && !GetConsoleMode(con, &dummy))
        con = INVALID_HANDLE_VALUE;
    if (con != INVALID_HANDLE_VALUE) {
        GetConsoleScreenBufferInfo(con, &con_info);
        attr_orig  = con_info.wAttributes;
        background = attr_orig & 0xF0;
    }
#endif

    if (getenv("AV_LOG_FORCE_NOCOLOR")) {
        use_color = 0;
    } else if (getenv("AV_LOG_FORCE_COLOR")) {
        use_color = 1;
    } else {
#if defined(_WIN32) && HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
        use_color = (con != INVALID_HANDLE_VALUE);
#elif HAVE_ISATTY
        use_color = (term && isatty(2));
#else
        use_color = 0;
#endif
    }

    if (getenv("AV_LOG_FORCE_256COLOR") || term && strstr(term, "256color"))
        use_color *= 256;
}

//done
static void ansi_fputs(int level, int tint, const char *str, int local_use_color)
{
    if (local_use_color == 1) {
        /*
        * 这段代码使用 fprintf 函数将带有 ANSI 转义码的文本输出到标准错误流 stderr。
        * ANSI 转义码用于在终端中控制文本的格式和颜色。
        *
        * 让我们逐个解释代码中的部分：
        *
        * \033[ 是 ANSI 转义序列的开始部分，表示后续的内容是一个 ANSI 转义码。
        *
        * %"PRIu32";3%"PRIu32"m 是格式化字符串，用于插入颜色控制信息。PRIu32 是一个宏，
        * 用于指定无符号整数类型 uint32_t 的格式说明符。这样可以确保在不同平台上的编译器正确
        * 识别和处理 uint32_t 类型的值。
        *
        * (color[level] >> 4) & 15 和 color[level] & 15 是用于提取颜色信息的表达式。
        * color[level] 是一个 uint32_t 类型的值，其中包含了前景色和背景色的信息。通过位运算，
        * 可以从中提取前景色和背景色的值，然后插入到格式化字符串中。
        *
        * str 是要输出的文本字符串。
        *
        * \033[0m 是 ANSI 转义码，用于恢复默认文本格式。在输出完带颜色的文本后，
        * 通过这个转义码将终端文本格式恢复为默认状态。
        *
        * 这段代码的作用是将字符串 str 输出到标准错误流 stderr，并根据 color[level] 中的颜色信息设置文本的颜色。
        */
        /*
         * "\033[%"PRIu32";3%"PRIu32"m%s\033[0m"更详细的解释:
         *
         * 这个字符串是一个 ANSI 转义序列，用于控制终端文本的颜色和格式。让我一步步解释它：
         *
         * \033[：这是 ANSI 转义序列的起始部分，表示后续的内容是一个 ANSI 转义码。
         *
         * %"PRIu32";：这部分是一个格式化字符串，用于插入颜色信息中前景色的值。PRIu32 是一个宏，
         * 用于指定无符号整数类型 uint32_t 的格式说明符。因为颜色信息是一个 uint32_t 类型的值，
         * 所以使用这个宏来确保在不同平台上的编译器正确识别和处理这个值。%"PRIu32" 的作用是将 PRIu32
         * 替换为正确的格式说明符，以便格式化输出前景色的值。
         *
         * 3%"PRIu32"m：这部分是另一个格式化字符串，用于插入颜色信息中背景色的值。同样，%"PRIu32" 的作用
         * 是将 PRIu32 替换为正确的格式说明符，以便格式化输出背景色的值。在 ANSI 转义码中，数字 3 表示背景色。
         *
         * %s：这是一个格式化字符串，用于插入要输出的文本字符串。
         *
         * \033[0m：这是另一个 ANSI 转义码，用于将终端文本格式恢复为默认状态。在输出完带颜色的文本后，
         * 通过这个转义码将终端文本格式恢复为默认状态，以免影响后续输出。
         *
         * 综上所述，这个 ANSI 转义序列用于设置输出文本的前景色和背景色，然后输出指定的文本字符串，
         * 并在输出后将终端文本格式恢复为默认状态。
         */

        /*
         * ANSI 转义码值与含义:
         * ANSI 转义码是一种用于在终端显示中控制文本格式和颜色的特殊序列。
         * 它们以 \033[ 开头，后跟一个或多个控制序列，并以字母 m 结尾。以下是一些常见的 ANSI 转义码及其含义：
         *
         * \033[0m：重置所有文本属性到默认值，即取消设置的所有格式和颜色。
         * \033[1m：设置粗体（高亮）文本。
         * \033[2m：设置一半亮度（暗）文本。
         * \033[3m：设置斜体文本（某些终端可能不支持）。
         * \033[4m：设置下划线文本。
         * \033[5m：设置闪烁文本（某些终端可能不支持）。
         * \033[7m：设置反显文本（将前景色与背景色交换）。
         * \033[8m：隐藏（不可见）文本（某些终端可能不支持）。
         * \033[30m - \033[37m：设置前景色，数字代表颜色代码。例如，30 表示黑色，31 表示红色，32 表示绿色，以此类推。
         * \033[40m - \033[47m：设置背景色，数字代表颜色代码。例如，40 表示黑色背景，41 表示红色背景，42 表示绿色背景，以此类推。
         *
         * 这些是一些常见的 ANSI 转义码，可以用于控制终端文本的格式和颜色。不同终端可能支持的 ANSI 转义码不同，因此在使用时应注意兼容性。
         */
        fprintf(stderr,
                "\033[%"PRIu32";3%"PRIu32"m%s\033[0m",
                (color[level] >> 4) & 15,
                color[level] & 15,
                str);
    } else if (tint && use_color == 256) {
        /*
         * 这段代码使用 ANSI 转义码来设置终端文本的背景色和前景色，并输出带有特定颜色的文本字符串。
         * 让我逐步解释这段代码：
         *
         * \033[48;5;%"PRIu32"m：这部分是设置终端文本背景色的 ANSI 转义码。其中，48 表示设置背景色，
         * 5 表示使用 256 色模式，%"PRIu32" 用于插入颜色值的格式化字符串。颜色值通过位运算从
         * color[level] 中提取，并移位以获取红色分量的值。
         *
         * \033[38;5;%dm：这部分是设置终端文本前景色的 ANSI 转义码。其中，38 表示设置前景色，
         * 5 表示使用 256 色模式，%d 用于插入颜色值的格式化字符串。颜色值直接使用变量 tint 中的值。
         *
         * %s：这是一个格式化字符串，用于插入要输出的文本字符串。
         *
         * \033[0m：这是另一个 ANSI 转义码，用于将终端文本格式恢复为默认状态。在输出完带颜色的文本后，
         * 通过这个转义码将终端文本格式恢复为默认状态，以免影响后续输出。
         *
         * 综上所述，这段代码会根据 color[level] 中提取的颜色值和变量 tint 中的颜色值，
         * 设置终端文本的背景色和前景色，并输出带有特定颜色的文本字符串。
         */
        fprintf(stderr,
                "\033[48;5;%"PRIu32"m\033[38;5;%dm%s\033[0m",
                (color[level] >> 16) & 0xff,
                tint,
                str);
    } else if (local_use_color == 256) {
        /*
         * 这段代码使用 ANSI 转义码来设置终端文本的背景色和前景色，并输出带有特定颜色的文本字符串。
         * 让我解释一下这段代码：
         * \033[48;5;%"PRIu32"m：这部分是设置终端文本背景色的 ANSI 转义码。其中，48 表示设置背景色，
         * 5 表示使用 256 色模式，%"PRIu32" 用于插入颜色值的格式化字符串。颜色值通过位运算从
         * color[level] 中提取，然后通过适当的位移操作获取红色分量的值。
         *
         * \033[38;5;%"PRIu32"m：这部分是设置终端文本前景色的 ANSI 转义码。其中，38 表示设置前景色，
         * 5 表示使用 256 色模式，%"PRIu32" 用于插入颜色值的格式化字符串。同样，
         * 颜色值通过位运算从 color[level] 中提取，然后通过适当的位移操作获取绿色分量的值。
         *
         * %s：这是一个格式化字符串，用于插入要输出的文本字符串。
         *
         * \033[0m：这是另一个 ANSI 转义码，用于将终端文本格式恢复为默认状态。在输出完带颜色的文本后，
         * 通过这个转义码将终端文本格式恢复为默认状态，以免影响后续输出。
         *
         * 综上所述，这段代码会根据 color[level] 中提取的颜色值的不同分量，分别设置终端文本的背景色和前景色，
         * 并输出带有特定颜色的文本字符串。
         */
        fprintf(stderr,
                "\033[48;5;%"PRIu32"m\033[38;5;%"PRIu32"m%s\033[0m",
                (color[level] >> 16) & 0xff,
                (color[level] >> 8) & 0xff,
                str);
    } else
        fputs(str, stderr);
}

//done
static void colored_fputs(int level, int tint, const char *str)
{
    int local_use_color;
    if (!*str)
        return;

    if (use_color < 0)
        check_color_terminal();

    if (level == AV_LOG_INFO/8) local_use_color = 0;
    else                        local_use_color = use_color;

#if defined(_WIN32) && HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
    if (con != INVALID_HANDLE_VALUE) {
        if (local_use_color)
            SetConsoleTextAttribute(con, background | color[level]);
        win_console_puts(str);
        if (local_use_color)
            SetConsoleTextAttribute(con, attr_orig);
    } else {
        ansi_fputs(level, tint, str, local_use_color);
    }
#else
    ansi_fputs(level, tint, str, local_use_color);
#endif

}
//done
const char *av_default_item_name(void *ptr)
{
    return (*(AVClass **) ptr)->class_name;
}
//done
AVClassCategory av_default_get_category(void *ptr)
{
    return (*(AVClass **) ptr)->category;
}

/*
 * 这段代码定义了一个名为 sanitize 的静态函数，它接受一个指向 uint8_t 类型的指针 line，
 * 用于对一行数据进行清理处理，将不可打印的字符替换为问号（'?'）。让我们逐行解释这个函数的实现：
 * static void sanitize(uint8_t *line): 这一行定义了一个静态函数 sanitize，它的参数是一个指向
 * uint8_t 类型的指针 line。
 *
 * while(*line) { ... }: 这是一个 while 循环，它会不断迭代直到遇到一个空字符（'\0'），即字符串的结束标志。
 * 在每次循环中，会执行以下操作：
 *
 * if(*line < 0x08 || (*line > 0x0D && *line < 0x20)): 这一行是一个条件语句，用于检查当前指针 line
 * 所指向的字符是否为不可打印的字符。具体来说，它检查当前字符是否小于 0x08（即 ASCII 控制字符范围）或者
 * 大于 0x0D 并且小于 0x20（即不可打印字符的其他范围）。
 * *line='?';: 如果当前字符是不可打印的字符，则将其替换为问号（'?'）。
 * line++;: 将指针 line 向前移动到下一个字符，以便在下一次循环中处理下一个字符。
 * 总的来说，这个函数的作用是遍历一行数据，将其中的不可打印字符替换为问号。这样可以保证在打印或处理
 * 数据时不会因为不可打印字符而导致意外行为。
 */
//done
static void sanitize(uint8_t *line){
    while(*line){
        if(*line < 0x08 || (*line > 0x0D && *line < 0x20))
            *line='?';
        line++;
    }
}
//done
static int get_category(void *ptr){
    AVClass *avc = *(AVClass **) ptr;
    if(    !avc
        || (avc->version&0xFF)<100
        ||  avc->version < (51 << 16 | 59 << 8)
        ||  avc->category >= AV_CLASS_CATEGORY_NB) return AV_CLASS_CATEGORY_NA + 16;

    if(avc->get_category)
        return avc->get_category(ptr) + 16;

    return avc->category + 16;
}
//done
static const char *get_level_str(int level)
{
    switch (level) {
    case AV_LOG_QUIET:
        return "quiet";
    case AV_LOG_DEBUG:
        return "debug";
    case AV_LOG_TRACE:
        return "trace";
    case AV_LOG_VERBOSE:
        return "verbose";
    case AV_LOG_INFO:
        return "info";
    case AV_LOG_WARNING:
        return "warning";
    case AV_LOG_ERROR:
        return "error";
    case AV_LOG_FATAL:
        return "fatal";
    case AV_LOG_PANIC:
        return "panic";
    default:
        return "";
    }
}

//done
static void format_line(void *avcl, int level, const char *fmt, va_list vl,
                        AVBPrint part[4], int *print_prefix, int type[2])
{
    AVClass* avc = avcl ? *(AVClass **) avcl : NULL;
    av_bprint_init(part+0, 0, AV_BPRINT_SIZE_AUTOMATIC);
    av_bprint_init(part+1, 0, AV_BPRINT_SIZE_AUTOMATIC);
    av_bprint_init(part+2, 0, AV_BPRINT_SIZE_AUTOMATIC);
    av_bprint_init(part+3, 0, 65536);

    if(type) type[0] = type[1] = AV_CLASS_CATEGORY_NA + 16;
    if (*print_prefix && avc) {
        if (avc->parent_log_context_offset) {
            AVClass** parent = *(AVClass ***) (((uint8_t *) avcl) +
                                   avc->parent_log_context_offset);
            if (parent && *parent) {
                av_bprintf(part+0, "[%s @ %p] ",
                         (*parent)->item_name(parent), parent);
                if(type) type[0] = get_category(parent);
            }
        }
        av_bprintf(part+1, "[%s @ %p] ",
                 avc->item_name(avcl), avcl);
        if(type) type[1] = get_category(avcl);
    }

    if (*print_prefix && (level > AV_LOG_QUIET) && (flags & AV_LOG_PRINT_LEVEL))
        av_bprintf(part+2, "[%s] ", get_level_str(level));

    av_vbprintf(part+3, fmt, vl);

    if(*part[0].str || *part[1].str || *part[2].str || *part[3].str) {
        char lastc = part[3].len && part[3].len <= part[3].size ? part[3].str[part[3].len - 1] : 0;
        *print_prefix = lastc == '\n' || lastc == '\r';
    }
}

//done
void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix)
{
    av_log_format_line2(ptr, level, fmt, vl, line, line_size, print_prefix);
}
//done
int av_log_format_line2(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix)
{
    AVBPrint part[4];
    int ret;

    format_line(ptr, level, fmt, vl, part, print_prefix, NULL);
    ret = snprintf(line, line_size, "%s%s%s%s", part[0].str, part[1].str, part[2].str, part[3].str);
    av_bprint_finalize(part+3, NULL);
    return ret;
}

//done
void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    static int print_prefix = 1;
    static int count;
    static char prev[LINE_SZ];
    AVBPrint part[4];
    char line[LINE_SZ];
    static int is_atty;
    int type[2];
    unsigned tint = 0;

    if (level >= 0) {
        tint = level & 0xff00;
        level &= 0xff;
    }

    if (level > av_log_level)
        return;
    ff_mutex_lock(&mutex);

    format_line(ptr, level, fmt, vl, part, &print_prefix, type);
    snprintf(line, sizeof(line), "%s%s%s%s", part[0].str, part[1].str, part[2].str, part[3].str);

#if HAVE_ISATTY
    if (!is_atty)
        is_atty = isatty(2) ? 1 : -1;
#endif

    if (print_prefix && (flags & AV_LOG_SKIP_REPEATED) && !strcmp(line, prev) &&
        *line && line[strlen(line) - 1] != '\r'){
        count++;
        if (is_atty == 1)
            fprintf(stderr, "    Last message repeated %d times\r", count);
        goto end;
    }
    if (count > 0) {
        fprintf(stderr, "    Last message repeated %d times\n", count);
        count = 0;
    }
    strcpy(prev, line);
    sanitize(part[0].str);
    colored_fputs(type[0], 0, part[0].str);
    sanitize(part[1].str);
    colored_fputs(type[1], 0, part[1].str);
    sanitize(part[2].str);
    colored_fputs(av_clip(level >> 3, 0, NB_LEVELS - 1), tint >> 8, part[2].str);
    sanitize(part[3].str);
    colored_fputs(av_clip(level >> 3, 0, NB_LEVELS - 1), tint >> 8, part[3].str);

#if CONFIG_VALGRIND_BACKTRACE
    if (level <= BACKTRACE_LOGLEVEL)
        VALGRIND_PRINTF_BACKTRACE("%s", "");
#endif
end:
    av_bprint_finalize(part+3, NULL);
    ff_mutex_unlock(&mutex);
}

//done
static void (*av_log_callback)(void*, int, const char*, va_list) =
    av_log_default_callback;

//done
void av_log(void* avcl, int level, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    av_vlog(avcl, level, fmt, vl);
    va_end(vl);
}
//done
void av_log_once(void* avcl, int initial_level, int subsequent_level, int *state, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    av_vlog(avcl, *state ? subsequent_level : initial_level, fmt, vl);
    va_end(vl);
    *state = 1;
}

//done
void av_vlog(void* avcl, int level, const char *fmt, va_list vl)
{
    AVClass* avc = avcl ? *(AVClass **) avcl : NULL;
    void (*log_callback)(void*, int, const char*, va_list) = av_log_callback;
    if (avc && avc->version >= (50 << 16 | 15 << 8 | 2) &&
        avc->log_level_offset_offset && level >= AV_LOG_FATAL)
        level += *(int *) (((uint8_t *) avcl) + avc->log_level_offset_offset);
    if (log_callback)
        log_callback(avcl, level, fmt, vl);
}
//done
int av_log_get_level(void)
{
    return av_log_level;
}
//done
void av_log_set_level(int level)
{
    av_log_level = level;
}
//done
void av_log_set_flags(int arg)
{
    flags = arg;
}
//done
int av_log_get_flags(void)
{
    return flags;
}
//done
void av_log_set_callback(void (*callback)(void*, int, const char*, va_list))
{
    av_log_callback = callback;
}

//done
static void missing_feature_sample(int sample, void *avc, const char *msg,
                                   va_list argument_list)
{
    av_vlog(avc, AV_LOG_WARNING, msg, argument_list);
    av_log(avc, AV_LOG_WARNING, " is not implemented. Update your FFmpeg "
           "version to the newest one from Git. If the problem still "
           "occurs, it means that your file has a feature which has not "
           "been implemented.\n");
    if (sample)
        av_log(avc, AV_LOG_WARNING, "If you want to help, upload a sample "
               "of this file to https://streams.videolan.org/upload/ "
               "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n");
}

//done
//该函数的声明在internal.h文件中
void avpriv_request_sample(void *avc, const char *msg, ...)
{
    va_list argument_list;

    va_start(argument_list, msg);
    missing_feature_sample(1, avc, msg, argument_list);
    va_end(argument_list);
}

//done
//该函数的声明在internal.h文件中
void avpriv_report_missing_feature(void *avc, const char *msg, ...)
{
    va_list argument_list;

    va_start(argument_list, msg);
    missing_feature_sample(0, avc, msg, argument_list);
    va_end(argument_list);
}
