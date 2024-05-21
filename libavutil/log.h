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

#ifndef AVUTIL_LOG_H
#define AVUTIL_LOG_H

#include <stdarg.h>
#include "attributes.h"
#include "version.h"

typedef enum {
    AV_CLASS_CATEGORY_NA = 0,
    AV_CLASS_CATEGORY_INPUT,
    AV_CLASS_CATEGORY_OUTPUT,
    AV_CLASS_CATEGORY_MUXER,
    AV_CLASS_CATEGORY_DEMUXER,
    AV_CLASS_CATEGORY_ENCODER,//5
    AV_CLASS_CATEGORY_DECODER,
    AV_CLASS_CATEGORY_FILTER,
    AV_CLASS_CATEGORY_BITSTREAM_FILTER,
    AV_CLASS_CATEGORY_SWSCALER,
    AV_CLASS_CATEGORY_SWRESAMPLER,//10
    AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_INPUT,
    AV_CLASS_CATEGORY_NB  ///< not part of ABI/API
}AVClassCategory;//done

//done
#define AV_IS_INPUT_DEVICE(category) \
    (((category) == AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_INPUT))

//done
#define AV_IS_OUTPUT_DEVICE(category) \
    (((category) == AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_OUTPUT))

//done
struct AVOptionRanges;

/**
 * Describe the class of an AVClass context structure. That is an
 * arbitrary struct of which the first field is a pointer to an
 * AVClass struct (e.g. AVCodecContext, AVFormatContext etc.).
 * 描述 AVClass 上下文结构的类。这是一个任意结构，其第一个字段是指向 AVClass 结构的指针
 * （例如 AVCodecContext、AVFormatContext 等）。
 */
/*
 * AVClass是FFmpeg库中一个非常基础的结构，它通常用于提供有关FFmpeg中其他结构体的元信息。
 * AVClass通常用于日志、错误处理和AVOptions（FFmpeg中的一套用于统一处理模块配置选项的机制）等方面。
 * 每一个使用AVOptions的组件（如编解码器、过滤器等）都会有一个对应的AVClass实例。
*/
typedef struct AVClass {
    /**
     * The name of the class; usually it is the same name as the
     * context structure type to which the AVClass is associated.
     * 类名称； 通常它与 AVClass 关联的上下文结构类型同名。
     */
    /*
     * 这是一个指向常量字符串的指针，表示类的名称。这个名称用于日志、调试和识别对象的类型。
     */
    //done
    const char* class_name;

    /**
     * A pointer to a function which returns the name of a context
     * instance ctx associated with the class.
     * 指向函数的指针，该函数返回与该类关联的上下文实例 ctx 的名称。
     */
    /*
     * 这是一个函数指针，它应该返回当前实例的名称。这对于日志和错误报告特别有用，
     * 因为它允许区分同一类中的不同实例。
     */
    //done
    const char* (*item_name)(void* ctx);

    /**
     * a pointer to the first option specified in the class if any or NULL
     * 指向类中指定的第一个选项的指针（如果有或为 NULL）
     *
     * @see av_set_default_options()
     */
    /*
     * 这是一个指向AVOption数组的指针，AVOption数组定义了该类的可配置选项。
     * 通过这些选项，用户可以在运行时设置或查询组件的配置。
    */
    //done
    const struct AVOption *option;

    /**
     * LIBAVUTIL_VERSION with which this structure was created.
     * This is used to allow fields to be added without requiring major
     * version bumps everywhere.
     * 此结构创建时的LIBAVUTIL_VERSION。这用于允许添加字段而不需要到处进行主要版本碰撞。
     */
    //done
    int version;

    /**
     * Offset in the structure where log_level_offset is stored.
     * 0 means there is no such variable
     * log_level_offset存储在结构中的偏移量。 0表示没有这个变量
     */
    /*
     * log_level_offset_offset是AVClass结构体中的一个成员变量，
     * 用于提供日志级别偏移量的字节偏移。这个成员变量的类型是int，
     * 它指示了存储日志级别阈值的字段相对于包含AVClass指针的对象起始位置的偏移量。
     * 通过这个偏移量，FFmpeg的日志系统可以找到并使用特定对象的日志级别设置，
     * 这允许对每个使用AVClass的FFmpeg组件进行细粒度的日志控制。
    */
    //done
    int log_level_offset_offset;

    /**
     * Offset in the structure where a pointer to the parent context for
     * logging is stored. For example a decoder could pass its AVCodecContext
     * to eval as such a parent context, which an av_log() implementation
     * could then leverage to display the parent context.
     * The offset can be NULL.
     * 结构中存储指向日志记录父上下文的指针的偏移量。例如，解码器可以将其
     * AVCodecContext 作为父上下文传递给 eval，然后 av_log() 实现可以利用它来显示父上下文。
     * 偏移量可以为 NULL。
     */
    /*
     * 这是一个指向父类AVClass的指针，用于日志上下文。
     * 它使得日志系统可以构建一个层次结构，从而更容易地组织和过滤日志信息。
    */
   //done
    int parent_log_context_offset;

    /**
     * Category used for visualization (like color)
     * This is only set if the category is equal for all objects using this class.
     * available since version (51 << 16 | 56 << 8 | 100)
     * 用于可视化的类别（如颜色） 仅当使用此类的所有对象的类别相同时才设置。自版本
     * (51 << 16 | 56 << 8 | 100) 起可用
     */
    /*
     * 这是一个枚举值，表示对象的类别（例如编解码器、过滤器等）。
     * 这有助于在日志或用户接口中对不同类型的组件进行分类。
    */
    //done
    AVClassCategory category;

    /**
     * Callback to return the category.
     * available since version (51 << 16 | 59 << 8 | 100)
     * 返回类别的回调。自版本 (51 << 16 | 59 << 8 | 100) 起可用
     */
    /*
     * 这是一个函数指针，返回一个指向表示上下文类别名称的字符串的指针。
     * 这提供了一种灵活的方式来定义和获取自定义类别名称。
    */
    //done
    AVClassCategory (*get_category)(void* ctx);

    /**
     * Callback to return the supported/allowed ranges.
     * available since version (52.12)
     * 返回支持/允许范围的回调。自版本 (52.12) 起可用
     */
    /*
     * 这是一个函数指针，用于查询配置选项的有效范围。这对于验证和显示配置选项的可能值非常有用。
    */
    //done
    int (*query_ranges)(struct AVOptionRanges **, void *obj, const char *key, int flags);

    /**
     * Return next AVOptions-enabled child or NULL
     * 返回下一个启用 AVOptions 的子项或 NULL
     */
    /*
     * 这是一个函数指针，用于遍历所有的子对象。它通常用于调试和日志记录，
     * 可以帮助开发者或用户了解对象的结构。
    */
    //done
    void* (*child_next)(void *obj, void *prev);

    /**
     * Iterate over the AVClasses corresponding to potential AVOptions-enabled
     * children.
     * 迭代与潜在的启用 AVOptions 的子项对应的 AVClass。
     *
     * @param iter pointer to opaque iteration state. The caller must initialize
     *             *iter to NULL before the first call.
     * iter 指向不透明迭代状态的指针。 调用者必须在第一次调用之前将 *iter 初始化为 NULL。
     * @return AVClass for the next AVOptions-enabled child or NULL if there are
     *         no more such children.
     * 下一个启用 AVOptions 的子项的 AVClass 或 NULL（如果没有更多此类子项）。
     *
     * @note The difference between child_next and this is that child_next
     *       iterates over _already existing_ objects, while child_class_iterate
     *       iterates over _all possible_ children.
     * child_next 和 this 之间的区别在于 child_next 迭代已经存在的对象，而 child_class_iterate
     * 迭代所有可能的子对象。
     */
    /*
     * 这个函数指针用于遍历类的所有子类。这在动态列出所有可用的编解码器、过滤器等时非常有用。
    */
    //done
    const struct AVClass* (*child_class_iterate)(void **iter);
} AVClass;

/**
 * @addtogroup lavu_log
 *
 * @{
 *
 * @defgroup lavu_log_constants Logging Constants
 *
 * @{
 */

/**
 * Print no output.打印无输出。
 */
//done
#define AV_LOG_QUIET    -8

/**
 * Something went really wrong and we will crash now.
 * 出了什么问题，我们现在就要崩溃了。
 */
//done
#define AV_LOG_PANIC     0

/**
 * Something went wrong and recovery is not possible.
 * For example, no header was found for a format which depends
 * on headers or an illegal combination of parameters is used.
 * 出现问题并且无法恢复。例如，未找到依赖于标头的格式的标头或使用了非法参数组合。
 */
//done
#define AV_LOG_FATAL     8

/**
 * Something went wrong and cannot losslessly be recovered.
 * However, not all future data is affected.
 * 出现问题并且无法无损恢复。然而，并非所有未来数据都会受到影响。
 */
//done
#define AV_LOG_ERROR    16

/**
 * Something somehow does not look correct. This may or may not
 * lead to problems. An example would be the use of '-vstrict -2'.
 * 有些东西看起来不正确。这可能会也可能不会导致问题。 一个例子是使用“-vstrict -2”。
 */
//done
#define AV_LOG_WARNING  24

/**
 * Standard information.标准信息
 */
//done
#define AV_LOG_INFO     32

/**
 * Detailed information.详细信息
 */
//done
#define AV_LOG_VERBOSE  40

/**
 * Stuff which is only useful for libav* developers.
 * 仅对 libav* 开发人员有用的东西。
 */
//done
#define AV_LOG_DEBUG    48

/**
 * Extremely verbose debugging, useful for libav* development.
 * 极其详细的调试，对于 libav* 开发很有用。
 */
//done
#define AV_LOG_TRACE    56
//done
#define AV_LOG_MAX_OFFSET (AV_LOG_TRACE - AV_LOG_QUIET)

/**
 * @}
 */

/**
 * Sets additional colors for extended debugging sessions.
 * 为扩展调试会话设置附加颜色。
 * @code
   av_log(ctx, AV_LOG_DEBUG|AV_LOG_C(134), "Message in purple\n");
   @endcode
 * Requires 256color terminal support. Uses outside debugging is not
 * recommended.
 * 需要 256 色终端支持。 不建议使用外部调试。
 */
//done
#define AV_LOG_C(x) ((x) << 8)

/**
 * Send the specified message to the log if the level is less than or equal
 * to the current av_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * 如果级别小于或等于当前 av_log_level，则将指定消息发送到日志。
 * 默认情况下，所有日志消息都会发送到 stderr。 可以通过设置不同的日志记录回调函数来更改此行为。
 * @see av_log_set_callback
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct or NULL if general log.
 * 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针，如果是通用日志，则为 NULL。
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * 使用日志常量表示的消息的重要性级别。
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * 格式字符串（printf 兼容），指定后续参数如何转换为输出。
 */
//done
void av_log(void *avcl, int level, const char *fmt, ...) av_printf_format(3, 4);

/**
 * Send the specified message to the log once with the initial_level and then with
 * the subsequent_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * 将指定的消息用initial_level发送一次到日志，然后用subsequent_level。
 * 默认情况下，所有日志消息都发送到stderr。可以通过设置不同的日志回调函数来更改此行为。
 * @see av_log
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct or NULL if general log.
 * 指向任意结构的指针，其中第一个字段是指向AVClass 结构的指针，如果是常规日志，则为NULL。
 * @param initial_level importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant" for the first occurance.
 * 使用第一次出现的日志常量表示的消息的重要性级别。
 * @param subsequent_level importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant" after the first occurance.
 * 第一次出现后使用日志记录常量 表示的消息的重要性级别。
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * 格式字符串（printf 兼容），指定后续参数如何转换为输出。
 * @param state a variable to keep trak of if a message has already been printed
 *        this must be initialized to 0 before the first use. The same state
 *        must not be accessed by 2 Threads simultaneously.
 * 用于跟踪消息是否已打印的变量，在首次使用之前必须将其初始化为 0。 同一状态不能同时被 2 个线程访问。
 */
//done
void av_log_once(void* avcl, int initial_level, int subsequent_level, int *state, const char *fmt, ...) av_printf_format(5, 6);


/**
 * Send the specified message to the log if the level is less than or equal
 * to the current av_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * 如果级别小于或等于当前 av_log_level，则将指定消息发送到日志。
 * 默认情况下，所有日志消息都会发送到 stderr。 可以通过设置不同的日志记录回调函数来更改此行为。
 * @see av_log_set_callback
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct.
 * 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针。
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * 使用日志常量表示的消息的重要性级别。
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * 格式字符串（printf 兼容），指定后续参数如何转换为输出。
 * @param vl The arguments referenced by the format string.
 * 格式字符串引用的参数。
 */
//done
void av_vlog(void *avcl, int level, const char *fmt, va_list vl);

/**
 * Get the current log level
 * 获取当前日志级别。
 *
 * @see lavu_log_constants
 *
 * @return Current log level
 */
//done
int av_log_get_level(void);

/**
 * Set the log level
 * 设置日志级别
 *
 * @see lavu_log_constants
 *
 * @param level Logging level
 */
//done
void av_log_set_level(int level);

/**
 * Set the logging callback
 * 设置日志记录回调
 *
 * @note The callback must be thread safe, even if the application does not use
 *       threads itself as some codecs are multithreaded.
 * 回调必须是线程安全的，即使应用程序本身不使用线程，因为某些编解码器是多线程的。
 *
 * @see av_log_default_callback
 *
 * @param callback A logging function with a compatible signature.
 * 具有兼容签名的日志记录函数。
 */
//done
void av_log_set_callback(void (*callback)(void*, int, const char*, va_list));

/**
 * Default logging callback
 * 默认日志记录回调
 *
 * It prints the message to stderr, optionally colorizing it.
 * 它将消息打印到 stderr，并可选择对其进行着色。
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct.
 * 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针。
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * 使用日志常量表示的消息的重要性级别。
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * 格式字符串（printf 兼容），指定后续参数如何转换为输出。
 * @param vl The arguments referenced by the format string.
 * 格式字符串引用的参数。
 */
//done
void av_log_default_callback(void *avcl, int level, const char *fmt,
                             va_list vl);//done

/**
 * Return the context name
 * 返回上下文名称
 *
 * @param  ctx The AVClass context
 *
 * @return The AVClass class_name
 */
//done
const char* av_default_item_name(void* ctx);
//done
AVClassCategory av_default_get_category(void *ptr);

/**
 * Format a line of log the same way as the default callback.
 * 以与默认回调相同的方式格式化一行日志。
 * @param line          buffer to receive the formatted line
 * 用于接收格式化行的缓冲区
 * @param line_size     size of the buffer
 * 缓冲区的大小
 * @param print_prefix  used to store whether the prefix must be printed;
 *                      must point to a persistent integer initially set to 1
 * 用于存储是否必须打印前缀； 必须指向一个初始设置为 1 的持久整数
 */
//done
void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);

/**
 * Format a line of log the same way as the default callback.
 * 以与默认回调相同的方式格式化一行日志。
 * @param line          buffer to receive the formatted line;
 *                      may be NULL if line_size is 0
 * 用于接收格式化行的缓冲区； 如果 line_size 为 0，则可能为 NULL
 * @param line_size     size of the buffer; at most line_size-1 characters will
 *                      be written to the buffer, plus one null terminator
 * 缓冲区的大小； 最多将 line_size-1 个字符写入缓冲区，加上一个空终止符
 * @param print_prefix  used to store whether the prefix must be printed;
 *                      must point to a persistent integer initially set to 1
 * 用于存储是否必须打印前缀； 必须指向一个初始设置为 1 的持久整数
 * @return Returns a negative value if an error occurred, otherwise returns
 *         the number of characters that would have been written for a
 *         sufficiently large buffer, not including the terminating null
 *         character. If the return value is not less than line_size, it means
 *         that the log message was truncated to fit the buffer.
 * 如果发生错误，则返回负值，否则返回为足够大的缓冲区写入的字符数，不包括终止空字符。
 * 如果返回值不小于line_size，则意味着日志消息被截断以适合缓冲区。
 */
//done
int av_log_format_line2(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);

/**
 * Skip repeated messages, this requires the user app to use av_log() instead of
 * (f)printf as the 2 would otherwise interfere and lead to
 * "Last message repeated x times" messages below (f)printf messages with some
 * bad luck.
 * 跳过重复的消息，这需要用户应用程序使用av_log（）而不是（f）printf，否则2会干扰并导致
 *（f）printf消息下面的“最后一条消息重复x次”消息带来一些厄运。
 * Also to receive the last, "last repeated" line if any, the user app must
 * call av_log(NULL, AV_LOG_QUIET, "%s", ""); at the end
 * 此外，要接收最后重复的行（如果有），用户应用程序必须在末尾调用av_log（NULL，AV_LOG_QUIET，"%s", "");
 */
/*
 * 当该标志被设置时，如果一个日志消息与前一个日志消息相同，则不会重复记录该消息。
 * 这对于减少日志中重复信息的输出非常有用。
 */
//done
#define AV_LOG_SKIP_REPEATED 1

/**
 * Include the log severity in messages originating from codecs.
 * 在源自编解码器的消息中包含日志严重性。
 *
 * Results in messages such as:
 * [rawvideo @ 0xDEADBEEF] [error] encode did not produce valid pts
 * 结果消息如下：[rawvideo @ 0xDEADBEEF] [error] 编码未生成有效的点
 */
/*
 * 当该标志被设置时，日志消息将包含日志级别的显示。这对于区分不同级别的日志消息非常有用。
 */
//done
#define AV_LOG_PRINT_LEVEL 2

/*
 * av_log_set_flags 是 FFmpeg 中用于设置日志记录标志的函数。
 * 它的作用是允许用户更改日志记录的行为，以满足特定的需求。
 */
//done
void av_log_set_flags(int arg);
//done
int av_log_get_flags(void);

/**
 * @}
 */

#endif /* AVUTIL_LOG_H */
