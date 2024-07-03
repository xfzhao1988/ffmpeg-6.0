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

/**
 * @file
 * @ingroup lavu_buffer
 * refcounted data buffer API
 * 引用计数数据缓冲区 API
 */

#ifndef AVUTIL_BUFFER_H
#define AVUTIL_BUFFER_H

#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup lavu_buffer AVBuffer
 * @ingroup lavu_data
 *
 * @{
 * AVBuffer is an API for reference-counted data buffers.
 * AVBuffer 是引用计数数据缓冲区的 API。
 *
 * There are two core objects in this API -- AVBuffer and AVBufferRef. AVBuffer
 * represents the data buffer itself; it is opaque and not meant to be accessed
 * by the caller directly, but only through AVBufferRef. However, the caller may
 * e.g. compare two AVBuffer pointers to check whether two different references
 * are describing the same data buffer. AVBufferRef represents a single
 * reference to an AVBuffer and it is the object that may be manipulated by the
 * caller directly.
 * 此 API 中有两个核心对象 - AVBuffer 和 AVBufferRef。AVBuffer 表示数据缓冲区本身；
 * 它是不透明的，不能由调用者直接访问，而只能通过 AVBufferRef 访问。但是，调用者可以比较
 * 两个 AVBuffer 指针，以检查两个不同的引用是否描述相同的数据缓冲区。AVBufferRef 表示对
 * AVBuffer 的单个引用，它是调用者可以直接操作的对象。
 *
 * There are two functions provided for creating a new AVBuffer with a single
 * reference -- av_buffer_alloc() to just allocate a new buffer, and
 * av_buffer_create() to wrap an existing array in an AVBuffer. From an existing
 * reference, additional references may be created with av_buffer_ref().
 * Use av_buffer_unref() to free a reference (this will automatically free the
 * data once all the references are freed).
 * 有两个函数可用于创建具有单个引用的新 AVBuffer - av_buffer_alloc() 仅分配新缓冲区，
 * av_buffer_create() 将现有数组包装在 AVBuffer 中。从现有引用，可以使用 av_buffer_ref()
 * 创建其他引用。使用 av_buffer_unref() 释放引用（一旦释放所有引用，这将自动释放数据）。
 *
 * The convention throughout this API and the rest of FFmpeg is such that the
 * buffer is considered writable if there exists only one reference to it (and
 * it has not been marked as read-only). The av_buffer_is_writable() function is
 * provided to check whether this is true and av_buffer_make_writable() will
 * automatically create a new writable buffer when necessary.
 * Of course nothing prevents the calling code from violating this convention,
 * however that is safe only when all the existing references are under its
 * control.
 * 此 API 和 FFmpeg 其余部分的惯例是，如果缓冲区仅存在一个引用（并且未将其标记为只读），
 * 则认为该缓冲区可写。av_buffer_is_writable() 函数用于检查这是否属实，av_buffer_make_writable()
 * 将在必要时自动创建一个新的可写缓冲区。当然，没有什么可以阻止调用代码违反此惯例，但只有当所有现有引用
 * 都在其控制之下时，这才是安全的。
 *
 * @note Referencing and unreferencing the buffers is thread-safe and thus
 * may be done from multiple threads simultaneously without any need for
 * additional locking.
 * 引用和取消引用缓冲区是线程安全的，因此可以同时从多个线程完成，而无需任何额外的锁定。
 *
 * @note Two different references to the same buffer can point to different
 * parts of the buffer (i.e. their AVBufferRef.data will not be equal).
 * 对同一缓冲区的两个不同引用可以指向缓冲区的不同部分（即它们的 AVBufferRef.data 不会相等）。
 */

/**
 * A reference counted buffer type. It is opaque and is meant to be used through
 * references (AVBufferRef).
 * 引用计数缓冲区类型。它是不透明的，旨在通过引用 (AVBufferRef) 使用。
 */
//done
typedef struct AVBuffer AVBuffer;

/**
 * A reference to a data buffer.
 * 对数据缓冲区的引用。
 *
 * The size of this struct is not a part of the public ABI and it is not meant
 * to be allocated directly.
 * 此结构的大小不属于公共 ABI 的一部分，并且不打算直接分配。
 */
//done
typedef struct AVBufferRef {
    AVBuffer *buffer;

    /**
     * The data buffer. It is considered writable if and only if
     * this is the only reference to the buffer, in which case
     * av_buffer_is_writable() returns 1.
     * 数据缓冲区。当且仅当这是对缓冲区的唯一引用时，它才被视为可写，在这种情况下
     * av_buffer_is_writable() 返回 1。
     */
    uint8_t *data;
    /**
     * Size of data in bytes.
     * 数据大小（以字节为单位）。
     */
    size_t   size;
} AVBufferRef;

/**
 * Allocate an AVBuffer of the given size using av_malloc().
 * 使用 av_malloc() 分配给定大小的 AVBuffer。
 *
 * @return an AVBufferRef of given size or NULL when out of memory
 * 指定大小的 AVBufferRef，或内存不足时返回 NULL
 */
AVBufferRef *av_buffer_alloc(size_t size);

/**
 * Same as av_buffer_alloc(), except the returned buffer will be initialized
 * to zero.
 * 与 av_buffer_alloc() 相同，但返回的缓冲区将被初始化为零。
 */
AVBufferRef *av_buffer_allocz(size_t size);

/**
 * Always treat the buffer as read-only, even when it has only one
 * reference.
 * 始终将缓冲区视为只读，即使它只有一个引用。
 */
#define AV_BUFFER_FLAG_READONLY (1 << 0)

/**
 * Create an AVBuffer from an existing array.
 * 从现有数组创建 AVBuffer。
 *
 * If this function is successful, data is owned by the AVBuffer. The caller may
 * only access data through the returned AVBufferRef and references derived from
 * it.
 * 如果此函数成功，数据将归 AVBuffer 所有。调用者只能通过返回的 AVBufferRef 及其派生的引用来访问数据。
 * If this function fails, data is left untouched.
 * 如果此函数失败，数据将保持不变。
 * @param data   data array
 *               数据数组
 * @param size   size of data in bytes
 *               数据大小（以字节为单位）
 * @param free   a callback for freeing this buffer's data
 *               释放此缓冲区数据的回调
 * @param opaque parameter to be got for processing or passed to free
 *               要获取以进行处理或传递给释放函数的参数
 * @param flags  a combination of AV_BUFFER_FLAG_*
 *               AV_BUFFER_FLAG_* 的组合
 *
 * @return an AVBufferRef referring to data on success, NULL on failure.
 *         成功时返回指向数据的 AVBufferRef，失败时返回 NULL。
 */
AVBufferRef *av_buffer_create(uint8_t *data, size_t size,
                              void (*free)(void *opaque, uint8_t *data),
                              void *opaque, int flags);

/**
 * Default free callback, which calls av_free() on the buffer data.
 * This function is meant to be passed to av_buffer_create(), not called
 * directly.
 * 默认释放回调，在缓冲区数据上调用 av_free()。此函数旨在传递给 av_buffer_create()，而不是直接调用。
 */
void av_buffer_default_free(void *opaque, uint8_t *data);

/**
 * Create a new reference to an AVBuffer.
 * 创建对 AVBuffer 的新引用。
 *
 * @return a new AVBufferRef referring to the same AVBuffer as buf or NULL on
 * failure.
 * 一个新的 AVBufferRef 引用与 buf 相同的 AVBuffer 或失败时为 NULL。
 */
AVBufferRef *av_buffer_ref(const AVBufferRef *buf);

/**
 * Free a given reference and automatically free the buffer if there are no more
 * references to it.
 *
 * @param buf the reference to be freed. The pointer is set to NULL on return.
 */
void av_buffer_unref(AVBufferRef **buf);

/**
 * @return 1 if the caller may write to the data referred to by buf (which is
 * true if and only if buf is the only reference to the underlying AVBuffer).
 * Return 0 otherwise.
 * A positive answer is valid until av_buffer_ref() is called on buf.
 */
int av_buffer_is_writable(const AVBufferRef *buf);

/**
 * @return the opaque parameter set by av_buffer_create.
 */
void *av_buffer_get_opaque(const AVBufferRef *buf);

int av_buffer_get_ref_count(const AVBufferRef *buf);

/**
 * Create a writable reference from a given buffer reference, avoiding data copy
 * if possible.
 *
 * @param buf buffer reference to make writable. On success, buf is either left
 *            untouched, or it is unreferenced and a new writable AVBufferRef is
 *            written in its place. On failure, buf is left untouched.
 * @return 0 on success, a negative AVERROR on failure.
 */
int av_buffer_make_writable(AVBufferRef **buf);

/**
 * Reallocate a given buffer.
 *
 * @param buf  a buffer reference to reallocate. On success, buf will be
 *             unreferenced and a new reference with the required size will be
 *             written in its place. On failure buf will be left untouched. *buf
 *             may be NULL, then a new buffer is allocated.
 * @param size required new buffer size.
 * @return 0 on success, a negative AVERROR on failure.
 *
 * @note the buffer is actually reallocated with av_realloc() only if it was
 * initially allocated through av_buffer_realloc(NULL) and there is only one
 * reference to it (i.e. the one passed to this function). In all other cases
 * a new buffer is allocated and the data is copied.
 */
int av_buffer_realloc(AVBufferRef **buf, size_t size);

/**
 * Ensure dst refers to the same data as src.
 *
 * When *dst is already equivalent to src, do nothing. Otherwise unreference dst
 * and replace it with a new reference to src.
 *
 * @param dst Pointer to either a valid buffer reference or NULL. On success,
 *            this will point to a buffer reference equivalent to src. On
 *            failure, dst will be left untouched.
 * @param src A buffer reference to replace dst with. May be NULL, then this
 *            function is equivalent to av_buffer_unref(dst).
 * @return 0 on success
 *         AVERROR(ENOMEM) on memory allocation failure.
 */
int av_buffer_replace(AVBufferRef **dst, const AVBufferRef *src);

/**
 * @}
 */

/**
 * @defgroup lavu_bufferpool AVBufferPool
 * @ingroup lavu_data
 *
 * @{
 * AVBufferPool is an API for a lock-free thread-safe pool of AVBuffers.
 *
 * Frequently allocating and freeing large buffers may be slow. AVBufferPool is
 * meant to solve this in cases when the caller needs a set of buffers of the
 * same size (the most obvious use case being buffers for raw video or audio
 * frames).
 *
 * At the beginning, the user must call av_buffer_pool_init() to create the
 * buffer pool. Then whenever a buffer is needed, call av_buffer_pool_get() to
 * get a reference to a new buffer, similar to av_buffer_alloc(). This new
 * reference works in all aspects the same way as the one created by
 * av_buffer_alloc(). However, when the last reference to this buffer is
 * unreferenced, it is returned to the pool instead of being freed and will be
 * reused for subsequent av_buffer_pool_get() calls.
 *
 * When the caller is done with the pool and no longer needs to allocate any new
 * buffers, av_buffer_pool_uninit() must be called to mark the pool as freeable.
 * Once all the buffers are released, it will automatically be freed.
 *
 * Allocating and releasing buffers with this API is thread-safe as long as
 * either the default alloc callback is used, or the user-supplied one is
 * thread-safe.
 */

/**
 * The buffer pool. This structure is opaque and not meant to be accessed
 * directly. It is allocated with av_buffer_pool_init() and freed with
 * av_buffer_pool_uninit().
 */
typedef struct AVBufferPool AVBufferPool;

/**
 * Allocate and initialize a buffer pool.
 *
 * @param size size of each buffer in this pool
 * @param alloc a function that will be used to allocate new buffers when the
 * pool is empty. May be NULL, then the default allocator will be used
 * (av_buffer_alloc()).
 * @return newly created buffer pool on success, NULL on error.
 */
AVBufferPool *av_buffer_pool_init(size_t size, AVBufferRef* (*alloc)(size_t size));

/**
 * Allocate and initialize a buffer pool with a more complex allocator.
 *
 * @param size size of each buffer in this pool
 * @param opaque arbitrary user data used by the allocator
 * @param alloc a function that will be used to allocate new buffers when the
 *              pool is empty. May be NULL, then the default allocator will be
 *              used (av_buffer_alloc()).
 * @param pool_free a function that will be called immediately before the pool
 *                  is freed. I.e. after av_buffer_pool_uninit() is called
 *                  by the caller and all the frames are returned to the pool
 *                  and freed. It is intended to uninitialize the user opaque
 *                  data. May be NULL.
 * @return newly created buffer pool on success, NULL on error.
 */
AVBufferPool *av_buffer_pool_init2(size_t size, void *opaque,
                                   AVBufferRef* (*alloc)(void *opaque, size_t size),
                                   void (*pool_free)(void *opaque));

/**
 * Mark the pool as being available for freeing. It will actually be freed only
 * once all the allocated buffers associated with the pool are released. Thus it
 * is safe to call this function while some of the allocated buffers are still
 * in use.
 *
 * @param pool pointer to the pool to be freed. It will be set to NULL.
 */
void av_buffer_pool_uninit(AVBufferPool **pool);

/**
 * Allocate a new AVBuffer, reusing an old buffer from the pool when available.
 * This function may be called simultaneously from multiple threads.
 *
 * @return a reference to the new buffer on success, NULL on error.
 */
AVBufferRef *av_buffer_pool_get(AVBufferPool *pool);

/**
 * Query the original opaque parameter of an allocated buffer in the pool.
 *
 * @param ref a buffer reference to a buffer returned by av_buffer_pool_get.
 * @return the opaque parameter set by the buffer allocator function of the
 *         buffer pool.
 *
 * @note the opaque parameter of ref is used by the buffer pool implementation,
 * therefore you have to use this function to access the original opaque
 * parameter of an allocated buffer.
 */
void *av_buffer_pool_buffer_get_opaque(const AVBufferRef *ref);

/**
 * @}
 */

#endif /* AVUTIL_BUFFER_H */
