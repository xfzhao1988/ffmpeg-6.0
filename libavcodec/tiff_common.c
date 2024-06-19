/*
 * TIFF Common Routines
 * Copyright (c) 2013 Thilo Borgmann <thilo.borgmann _at_ mail.de>
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
 * TIFF Common Routines
 * @author Thilo Borgmann <thilo.borgmann _at_ mail.de>
 */

#include "libavutil/bprint.h"
#include "tiff_common.h"


int ff_tis_ifd(unsigned tag)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(ifd_tags); i++) {
        if (ifd_tags[i] == tag) {
            return i + 1;
        }
    }
    return 0;
}


unsigned ff_tget_short(GetByteContext *gb, int le)
{
    return le ? bytestream2_get_le16(gb) : bytestream2_get_be16(gb);
}


unsigned ff_tget_long(GetByteContext *gb, int le)
{
    return le ? bytestream2_get_le32(gb) : bytestream2_get_be32(gb);
}

/**
这段代码是一个函数 ff_tget_double，用于从字节流中读取一个 double 类型的数值，并根据指定的字节序（大小端）进行读取。

函数解析：

函数名和参数：
double ff_tget_double(GetByteContext *gb, int le)
GetByteContext *gb 是一个指向字节流上下文的指针，用于读取字节数据。
int le 是一个标志，表示是否使用小端字节序。如果 le 为真（非零），则使用小端字节序读取；否则使用大端字节序读取。

变量声明：
av_alias64 i = { .u64 = le ? bytestream2_get_le64(gb) : bytestream2_get_be64(gb)};
av_alias64 是一个联合体，包含了多种数据类型，用于数据类型的转换。
i 是一个 av_alias64 类型的变量，初始化为一个匿名联合体，根据 le 的值选择使用 bytestream2_get_le64(gb) 或
bytestream2_get_be64(gb) 从字节流中读取一个 uint64_t 类型的整数。

返回值：
return i.f64;

最后，函数返回联合体 i 中的 f64 成员，这是一个 double 类型的值。

解释代码功能：
bytestream2_get_le64(gb) 和 bytestream2_get_be64(gb) 是两个函数，分别用于从字节流中读取小端和大端格式的 uint64_t 类型整数。
av_alias64 联合体的作用是通过 u64 成员来访问同一内存空间中的不同数据类型，这里用于将读取的整数转换为 double 类型。
根据 le 参数的值，选择合适的字节序读取整数，并将其转换为 double 类型后返回。

注意事项：
代码假设 bytestream2_get_le64 和 bytestream2_get_be64 函数能够正确地从字节流中读取数据，并根据小端或大端格式返回正确的整数值。
使用联合体 av_alias64 的方式，确保在读取整数后能够直接将其视为 double 类型，而无需显式地进行类型转换。

这段代码适用于需要从字节流中解析 double 类型数据，并能够根据不同的字节序进行解析的情况。
*/
double ff_tget_double(GetByteContext *gb, int le)
{
    av_alias64 i = { .u64 = le ? bytestream2_get_le64(gb) : bytestream2_get_be64(gb)};
    return i.f64;
}


unsigned ff_tget(GetByteContext *gb, int type, int le)
{
    switch (type) {
    case TIFF_BYTE:  return bytestream2_get_byte(gb);
    case TIFF_SHORT: return ff_tget_short(gb, le);
    case TIFF_LONG:  return ff_tget_long(gb, le);
    default:         return UINT_MAX;
    }
}

static const char *auto_sep(int count, const char *sep, int i, int columns)
{
    if (sep)
        return i ? sep : "";
    if (i && i%columns) {
        return ", ";
    } else
        return columns < count ? "\n" : "";
}

static int bprint_to_avdict(AVBPrint *bp, const char *name,
                            AVDictionary **metadata)
{
    char *ap;
    int ret;

    if (!av_bprint_is_complete(bp)) {
        av_bprint_finalize(bp, NULL);
        return AVERROR(ENOMEM);
    }
    if ((ret = av_bprint_finalize(bp, &ap)) < 0)
        return ret;

    return av_dict_set(metadata, name, ap, AV_DICT_DONT_STRDUP_VAL);
}

int ff_tadd_rational_metadata(int count, const char *name, const char *sep,
                              GetByteContext *gb, int le, AVDictionary **metadata)
{
    AVBPrint bp;
    int32_t nom, denom;
    int i;

    if (count >= INT_MAX / sizeof(int64_t) || count <= 0)
        return AVERROR_INVALIDDATA;
    if (bytestream2_get_bytes_left(gb) < count * sizeof(int64_t))
        return AVERROR_INVALIDDATA;

    av_bprint_init(&bp, 10 * count, AV_BPRINT_SIZE_UNLIMITED);

    for (i = 0; i < count; i++) {
        nom   = ff_tget_long(gb, le);
        denom = ff_tget_long(gb, le);
        av_bprintf(&bp, "%s%7"PRId32":%-7"PRId32, auto_sep(count, sep, i, 4), nom, denom);
    }

    return bprint_to_avdict(&bp, name, metadata);
}


int ff_tadd_long_metadata(int count, const char *name, const char *sep,
                          GetByteContext *gb, int le, AVDictionary **metadata)
{
    AVBPrint bp;
    int i;

    if (count >= INT_MAX / sizeof(int32_t) || count <= 0)
        return AVERROR_INVALIDDATA;
    if (bytestream2_get_bytes_left(gb) < count * sizeof(int32_t))
        return AVERROR_INVALIDDATA;

    av_bprint_init(&bp, 10 * count, AV_BPRINT_SIZE_UNLIMITED);

    for (i = 0; i < count; i++) {
        av_bprintf(&bp, "%s%7i", auto_sep(count, sep, i, 8), ff_tget_long(gb, le));
    }

    return bprint_to_avdict(&bp, name, metadata);
}


int ff_tadd_doubles_metadata(int count, const char *name, const char *sep,
                             GetByteContext *gb, int le, AVDictionary **metadata)
{
    AVBPrint bp;
    int i;

    if (count >= INT_MAX / sizeof(int64_t) || count <= 0)
        return AVERROR_INVALIDDATA;
    if (bytestream2_get_bytes_left(gb) < count * sizeof(int64_t))
        return AVERROR_INVALIDDATA;

    av_bprint_init(&bp, 10 * count, 100 * count);

    for (i = 0; i < count; i++) {
        av_bprintf(&bp, "%s%.15g", auto_sep(count, sep, i, 4), ff_tget_double(gb, le));
    }

    return bprint_to_avdict(&bp, name, metadata);
}


int ff_tadd_shorts_metadata(int count, const char *name, const char *sep,
                            GetByteContext *gb, int le, int is_signed, AVDictionary **metadata)
{
    AVBPrint bp;
    int i;

    if (count >= INT_MAX / sizeof(int16_t) || count <= 0)
        return AVERROR_INVALIDDATA;
    if (bytestream2_get_bytes_left(gb) < count * sizeof(int16_t))
        return AVERROR_INVALIDDATA;

    av_bprint_init(&bp, 10 * count, AV_BPRINT_SIZE_UNLIMITED);

    for (i = 0; i < count; i++) {
        int v = is_signed ? (int16_t)ff_tget_short(gb, le) :  ff_tget_short(gb, le);
        av_bprintf(&bp, "%s%5i", auto_sep(count, sep, i, 8), v);
    }

    return bprint_to_avdict(&bp, name, metadata);
}


int ff_tadd_bytes_metadata(int count, const char *name, const char *sep,
                           GetByteContext *gb, int le, int is_signed, AVDictionary **metadata)
{
    AVBPrint bp;
    int i;

    if (count >= INT_MAX / sizeof(int8_t) || count < 0)
        return AVERROR_INVALIDDATA;
    if (bytestream2_get_bytes_left(gb) < count * sizeof(int8_t))
        return AVERROR_INVALIDDATA;

    av_bprint_init(&bp, 10 * count, AV_BPRINT_SIZE_UNLIMITED);

    for (i = 0; i < count; i++) {
        int v = is_signed ? (int8_t)bytestream2_get_byte(gb) :  bytestream2_get_byte(gb);
        av_bprintf(&bp, "%s%3i", auto_sep(count, sep, i, 16), v);
    }

    return bprint_to_avdict(&bp, name, metadata);
}

int ff_tadd_string_metadata(int count, const char *name,
                            GetByteContext *gb, int le, AVDictionary **metadata)
{
    char *value;

    if (bytestream2_get_bytes_left(gb) < count || count < 0)
        return AVERROR_INVALIDDATA;

    value = av_malloc(count + 1);
    if (!value)
        return AVERROR(ENOMEM);

    bytestream2_get_bufferu(gb, value, count);
    value[count] = 0;

    av_dict_set(metadata, name, value, AV_DICT_DONT_STRDUP_VAL);
    return 0;
}


int ff_tdecode_header(GetByteContext *gb, int *le, int *ifd_offset)
{
    if (bytestream2_get_bytes_left(gb) < 8) {
        return AVERROR_INVALIDDATA;
    }

    *le = bytestream2_get_le16u(gb);
    if (*le == AV_RB16("II")) {
        *le = 1;
    } else if (*le == AV_RB16("MM")) {
        *le = 0;
    } else {
        return AVERROR_INVALIDDATA;
    }

    if (ff_tget_short(gb, *le) != 42) {
        return AVERROR_INVALIDDATA;
    }

    *ifd_offset = ff_tget_long(gb, *le);

    return 0;
}


int ff_tread_tag(GetByteContext *gb, int le, unsigned *tag, unsigned *type,
                 unsigned *count, int *next)
{
    int ifd_tag;
    int valid_type;

    *tag    = ff_tget_short(gb, le);
    *type   = ff_tget_short(gb, le);
    *count  = ff_tget_long (gb, le);

    ifd_tag    = ff_tis_ifd(*tag);
    valid_type = *type != 0 && *type < FF_ARRAY_ELEMS(type_sizes);

    *next = bytestream2_tell(gb) + 4;

    // check for valid type
    if (!valid_type) {
        return AVERROR_INVALIDDATA;
    }

    // seek to offset if this is an IFD-tag or
    // if count values do not fit into the offset value
    if (ifd_tag || (*count > 4 || !(type_sizes[*type] * (*count) <= 4 || *type == TIFF_STRING))) {
        bytestream2_seek(gb, ff_tget_long (gb, le), SEEK_SET);
    }

    return 0;
}
