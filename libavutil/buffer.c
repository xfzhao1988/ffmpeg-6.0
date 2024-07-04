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

#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

#include "avassert.h"
#include "buffer_internal.h"
#include "common.h"
#include "mem.h"
#include "thread.h"

//done
static AVBufferRef *buffer_create(AVBuffer *buf, uint8_t *data, size_t size,
                                  void (*free)(void *opaque, uint8_t *data),
                                  void *opaque, int flags)
{
    AVBufferRef *ref = NULL;

    buf->data     = data;
    buf->size     = size;
    buf->free     = free ? free : av_buffer_default_free;
    buf->opaque   = opaque;

    atomic_init(&buf->refcount, 1);

    buf->flags = flags;

    ref = av_mallocz(sizeof(*ref));
    if (!ref)
        return NULL;

    ref->buffer = buf;
    ref->data   = data;
    ref->size   = size;

    return ref;
}
//done
AVBufferRef *av_buffer_create(uint8_t *data, size_t size,
                              void (*free)(void *opaque, uint8_t *data),
                              void *opaque, int flags)
{
    AVBufferRef *ret;
    AVBuffer *buf = av_mallocz(sizeof(*buf));
    if (!buf)
        return NULL;

    ret = buffer_create(buf, data, size, free, opaque, flags);
    if (!ret) {
        av_free(buf);
        return NULL;
    }
    return ret;
}
//done
void av_buffer_default_free(void *opaque, uint8_t *data)
{
    av_free(data);
}
//done
AVBufferRef *av_buffer_alloc(size_t size)
{
    AVBufferRef *ret = NULL;
    uint8_t    *data = NULL;

    data = av_malloc(size);
    if (!data)
        return NULL;

    ret = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
    if (!ret)
        av_freep(&data);

    return ret;
}

AVBufferRef *av_buffer_allocz(size_t size)
{
    AVBufferRef *ret = av_buffer_alloc(size);
    if (!ret)
        return NULL;

    memset(ret->data, 0, size);
    return ret;
}

AVBufferRef *av_buffer_ref(const AVBufferRef *buf)
{
    AVBufferRef *ret = av_mallocz(sizeof(*ret));

    if (!ret)
        return NULL;

    *ret = *buf;
/*
这行代码是一个原子加法操作，用于增加 buf->buffer->refcount 的值。让我解释一下这段代码的含义和作用：

代码解析
atomic_fetch_add_explicit(&buf->buffer->refcount, 1, memory_order_relaxed);
atomic_fetch_add_explicit：这是一个函数或宏，用于以指定的内存顺序（这里是 memory_order_relaxed，
即松散的内存顺序）对指定的原子变量执行加法操作。

&buf->buffer->refcount：这是一个指向原子变量 refcount 的指针，refcount 是一个计数器，
通常用于跟踪引用该数据结构的线程数或者引用计数。

1：这是要加上的值，即每次调用这个操作，refcount 的值会增加 1。

memory_order_relaxed：这是指定的内存顺序，表示操作可以以任意顺序执行，不会引入额外的同步开销。
在这种模式下，编译器和处理器可以对操作进行优化和重排序，但不会影响其他线程的操作结果。

功能与用法
功能：atomic_fetch_add_explicit 用于原子地将指定的值加到原子变量中，并返回之前的值。
内存顺序：使用 memory_order_relaxed 意味着该加法操作可以被编译器和处理器以非常高效的方式执行，
适合于不需要强顺序保证的情况下，如简单的计数器增加。

示例用法
假设 buf 是一个结构体指针，其中 buffer 是一个包含 refcount 的原子变量，可以这样使用：

atomic_fetch_add_explicit(&buf->buffer->refcount, 1, memory_order_relaxed);

这行代码将原子地将 buf->buffer->refcount 的值增加 1，使用了松散的内存顺序，确保了并发环境下的线程安全性和性能。
*/
    atomic_fetch_add_explicit(&buf->buffer->refcount, 1, memory_order_relaxed);

    return ret;
}

static void buffer_replace(AVBufferRef **dst, AVBufferRef **src)
{
    AVBuffer *b;

    b = (*dst)->buffer;

    if (src) {
        **dst = **src;
        av_freep(src);
    } else
        av_freep(dst);

    if (atomic_fetch_sub_explicit(&b->refcount, 1, memory_order_acq_rel) == 1) {
        /* b->free below might already free the structure containing *b,
         * so we have to read the flag now to avoid use-after-free. */
        /* 下面的 b->free 可能已经释放了包含 *b 的结构，所以我们现在必须读取标志以避免释放后使用。*/
        int free_avbuffer = !(b->flags_internal & BUFFER_FLAG_NO_FREE);
        b->free(b->opaque, b->data);
        if (free_avbuffer)
            av_free(b);
    }
}

void av_buffer_unref(AVBufferRef **buf)
{
    if (!buf || !*buf)
        return;

    buffer_replace(buf, NULL);
}

int av_buffer_is_writable(const AVBufferRef *buf)
{
    if (buf->buffer->flags & AV_BUFFER_FLAG_READONLY)
        return 0;

    return atomic_load(&buf->buffer->refcount) == 1;
}

void *av_buffer_get_opaque(const AVBufferRef *buf)
{
    return buf->buffer->opaque;
}

int av_buffer_get_ref_count(const AVBufferRef *buf)
{
    return atomic_load(&buf->buffer->refcount);
}

int av_buffer_make_writable(AVBufferRef **pbuf)
{
    AVBufferRef *newbuf, *buf = *pbuf;

    if (av_buffer_is_writable(buf))
        return 0;

    newbuf = av_buffer_alloc(buf->size);
    if (!newbuf)
        return AVERROR(ENOMEM);

    memcpy(newbuf->data, buf->data, buf->size);

    buffer_replace(pbuf, &newbuf);

    return 0;
}

int av_buffer_realloc(AVBufferRef **pbuf, size_t size)
{
    AVBufferRef *buf = *pbuf;
    uint8_t *tmp;
    int ret;

    if (!buf) {
        /* allocate a new buffer with av_realloc(), so it will be reallocatable
         * later */
        uint8_t *data = av_realloc(NULL, size);
        if (!data)
            return AVERROR(ENOMEM);

        buf = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
        if (!buf) {
            av_freep(&data);
            return AVERROR(ENOMEM);
        }

        buf->buffer->flags_internal |= BUFFER_FLAG_REALLOCATABLE;
        *pbuf = buf;

        return 0;
    } else if (buf->size == size)
        return 0;

    if (!(buf->buffer->flags_internal & BUFFER_FLAG_REALLOCATABLE) ||
        !av_buffer_is_writable(buf) || buf->data != buf->buffer->data) {
        /* cannot realloc, allocate a new reallocable buffer and copy data */
        AVBufferRef *new = NULL;

        ret = av_buffer_realloc(&new, size);
        if (ret < 0)
            return ret;

        memcpy(new->data, buf->data, FFMIN(size, buf->size));

        buffer_replace(pbuf, &new);
        return 0;
    }

    tmp = av_realloc(buf->buffer->data, size);
    if (!tmp)
        return AVERROR(ENOMEM);

    buf->buffer->data = buf->data = tmp;
    buf->buffer->size = buf->size = size;
    return 0;
}

int av_buffer_replace(AVBufferRef **pdst, const AVBufferRef *src)
{
    AVBufferRef *dst = *pdst;
    AVBufferRef *tmp;

    if (!src) {
        av_buffer_unref(pdst);
        return 0;
    }

    if (dst && dst->buffer == src->buffer) {
        /* make sure the data pointers match */
        dst->data = src->data;
        dst->size = src->size;
        return 0;
    }

    tmp = av_buffer_ref(src);
    if (!tmp)
        return AVERROR(ENOMEM);

    av_buffer_unref(pdst);
    *pdst = tmp;
    return 0;
}

AVBufferPool *av_buffer_pool_init2(size_t size, void *opaque,
                                   AVBufferRef* (*alloc)(void *opaque, size_t size),
                                   void (*pool_free)(void *opaque))
{
    AVBufferPool *pool = av_mallocz(sizeof(*pool));
    if (!pool)
        return NULL;

    ff_mutex_init(&pool->mutex, NULL);

    pool->size      = size;
    pool->opaque    = opaque;
    pool->alloc2    = alloc;
    pool->alloc     = av_buffer_alloc; // fallback
    pool->pool_free = pool_free;

    atomic_init(&pool->refcount, 1);

    return pool;
}

AVBufferPool *av_buffer_pool_init(size_t size, AVBufferRef* (*alloc)(size_t size))
{
    AVBufferPool *pool = av_mallocz(sizeof(*pool));
    if (!pool)
        return NULL;

    ff_mutex_init(&pool->mutex, NULL);

    pool->size     = size;
    pool->alloc    = alloc ? alloc : av_buffer_alloc;

    atomic_init(&pool->refcount, 1);

    return pool;
}

static void buffer_pool_flush(AVBufferPool *pool)
{
    while (pool->pool) {
        BufferPoolEntry *buf = pool->pool;
        pool->pool = buf->next;

        buf->free(buf->opaque, buf->data);
        av_freep(&buf);
    }
}

/*
 * This function gets called when the pool has been uninited and
 * all the buffers returned to it.
 */
static void buffer_pool_free(AVBufferPool *pool)
{
    buffer_pool_flush(pool);
    ff_mutex_destroy(&pool->mutex);

    if (pool->pool_free)
        pool->pool_free(pool->opaque);

    av_freep(&pool);
}

void av_buffer_pool_uninit(AVBufferPool **ppool)
{
    AVBufferPool *pool;

    if (!ppool || !*ppool)
        return;
    pool   = *ppool;
    *ppool = NULL;

    ff_mutex_lock(&pool->mutex);
    buffer_pool_flush(pool);
    ff_mutex_unlock(&pool->mutex);

    if (atomic_fetch_sub_explicit(&pool->refcount, 1, memory_order_acq_rel) == 1)
        buffer_pool_free(pool);
}

static void pool_release_buffer(void *opaque, uint8_t *data)
{
    BufferPoolEntry *buf = opaque;
    AVBufferPool *pool = buf->pool;

    ff_mutex_lock(&pool->mutex);
    buf->next = pool->pool;
    pool->pool = buf;
    ff_mutex_unlock(&pool->mutex);

    if (atomic_fetch_sub_explicit(&pool->refcount, 1, memory_order_acq_rel) == 1)
        buffer_pool_free(pool);
}

/* allocate a new buffer and override its free() callback so that
 * it is returned to the pool on free */
static AVBufferRef *pool_alloc_buffer(AVBufferPool *pool)
{
    BufferPoolEntry *buf;
    AVBufferRef     *ret;

    av_assert0(pool->alloc || pool->alloc2);

    ret = pool->alloc2 ? pool->alloc2(pool->opaque, pool->size) :
                         pool->alloc(pool->size);
    if (!ret)
        return NULL;

    buf = av_mallocz(sizeof(*buf));
    if (!buf) {
        av_buffer_unref(&ret);
        return NULL;
    }

    buf->data   = ret->buffer->data;
    buf->opaque = ret->buffer->opaque;
    buf->free   = ret->buffer->free;
    buf->pool   = pool;

    ret->buffer->opaque = buf;
    ret->buffer->free   = pool_release_buffer;

    return ret;
}

AVBufferRef *av_buffer_pool_get(AVBufferPool *pool)
{
    AVBufferRef *ret;
    BufferPoolEntry *buf;

    ff_mutex_lock(&pool->mutex);
    buf = pool->pool;
    if (buf) {
        memset(&buf->buffer, 0, sizeof(buf->buffer));
        ret = buffer_create(&buf->buffer, buf->data, pool->size,
                            pool_release_buffer, buf, 0);
        if (ret) {
            pool->pool = buf->next;
            buf->next = NULL;
            buf->buffer.flags_internal |= BUFFER_FLAG_NO_FREE;
        }
    } else {
        ret = pool_alloc_buffer(pool);
    }
    ff_mutex_unlock(&pool->mutex);

    if (ret)
        atomic_fetch_add_explicit(&pool->refcount, 1, memory_order_relaxed);

    return ret;
}

void *av_buffer_pool_buffer_get_opaque(const AVBufferRef *ref)
{
    BufferPoolEntry *buf = ref->buffer->opaque;
    av_assert0(buf);
    return buf->opaque;
}
