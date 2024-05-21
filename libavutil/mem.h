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
 * @ingroup lavu_mem
 * Memory handling functions
 */

#ifndef AVUTIL_MEM_H
#define AVUTIL_MEM_H

#include <limits.h>
#include <stdint.h>

#include "attributes.h"
#include "avutil.h"
#include "version.h"

/**
 * @addtogroup lavu_mem
 * Utilities for manipulating memory.
 *
 * FFmpeg has several applications of memory that are not required of a typical
 * program. For example, the computing-heavy components like video decoding and
 * encoding can be sped up significantly through the use of aligned memory.
 *
 * However, for each of FFmpeg's applications of memory, there might not be a
 * recognized or standardized API for that specific use. Memory alignment, for
 * instance, varies wildly depending on operating systems, architectures, and
 * compilers. Hence, this component of @ref libavutil is created to make
 * dealing with memory consistently possible on all platforms.
 *
 * @{
 */

/**
 * @defgroup lavu_mem_attrs Function Attributes
 * Function attributes applicable to memory handling functions.
 *
 * These function attributes can help compilers emit more useful warnings, or
 * generate better code.
 * @{
 */

/**
 * @def av_malloc_attrib
 * Function attribute denoting a malloc-like function.
 * 表示类似 malloc 的函数的函数属性。
 *
 * @see <a href="https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-g_t_0040code_007bmalloc_007d-function-attribute-3251">Function attribute `malloc` in GCC's documentation</a>
 */
/**
 * __attribute__((__malloc__)) 是 GCC 和 Clang 中的一个扩展属性（attribute），用于告诉编译器，
 * 函数的返回值是一个指向通过 malloc 分配的内存块的指针。这个属性对于静态分析工具、编译器和优化器等有用，
 * 可以帮助检测内存分配问题，并进行相关的优化。
 *
 * 具体来说，__attribute__((__malloc__)) 的作用包括：
 *
 * 静态分析支持：编译器可以通过这个属性来检查函数返回的指针是否指向通过 malloc 分配的内存，从而在编译期间提供更多
 * 的警告或错误信息。这有助于发现内存泄漏、悬挂指针等问题。
 *
 * 优化支持：编译器可以根据这个属性对代码进行优化。例如，它可以假设返回的指针不会为 NULL，
 * 因为 malloc 在分配失败时会抛出异常，这可以帮助编译器进行更有效的优化。
 *
 * 文档化：使用 __attribute__((__malloc__)) 属性可以明确地告诉其他开发人员或工具，函数的返回值
 * 是一个指向通过 malloc 分配的内存块的指针。这有助于提高代码的可读性和可维护性。
 *
 * 需要注意的是，__attribute__((__malloc__)) 是一个非标准的语言扩展，因此在使用时应该考虑可移植性的问题。
 * 另外，对于 C++ 代码，通常建议使用 [[gnu::malloc]] 或 [[gnu::malloc, nothrow]] 语法来实现相同的效果。
*/
#if AV_GCC_VERSION_AT_LEAST(3,1)
    #define av_malloc_attrib __attribute__((__malloc__))
#else
    #define av_malloc_attrib
#endif

/**
 * @def av_alloc_size(...)
 * Function attribute used on a function that allocates memory, whose size is
 * given by the specified parameter(s).
 * 函数属性用于分配内存的函数，其大小由指定的参数给定。
 *
 * @code{.c}
 * void *av_malloc(size_t size) av_alloc_size(1);
 * void *av_calloc(size_t nmemb, size_t size) av_alloc_size(1, 2);
 * @endcode
 *
 * @param ... One or two parameter indexes, separated by a comma
 * 一个或两个参数索引，以逗号分隔
 *
 * @see <a href="https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-g_t_0040code_007balloc_005fsize_007d-function-attribute-3220">Function attribute `alloc_size` in GCC's documentation</a>
 */

#if AV_GCC_VERSION_AT_LEAST(4,3)
/**
 * @brief 这段代码定义了一个宏 av_alloc_size，它使用了 GCC 的 __attribute__ 语法，用于指示编译器对
 * 函数的返回值进行分配大小的检查。
 *
 * 具体来说：
 *
 * __attribute__((alloc_size(__VA_ARGS__)))：这个属性告诉编译器，函数的返回值应该被视为分配了多少字节
 * 的内存。它可以用来帮助编译器进行内存分配大小的静态分析，以便在编译时捕获一些潜在的内存分配问题。
 *
 * __VA_ARGS__：是一个特殊的宏，用于表示宏参数的可变参数。在这里，它允许 av_alloc_size 宏接受任意数量的参数，
 * 并将其传递给 alloc_size 属性。
 *
 * 这个宏通常用于声明分配内存的函数，以便编译器能够检查函数的返回值，确保分配的内存大小与参数的预期值相匹配。例如：
 * void *my_alloc_function(size_t size) av_alloc_size(1);
 *
 * 这样声明的 my_alloc_function 函数，编译器将会检查其返回值，确保分配的内存大小等于 size 参数的值。
 */
    #define av_alloc_size(...) __attribute__((alloc_size(__VA_ARGS__)))
#else
    #define av_alloc_size(...)
#endif

/**
 * @}
 */

/**
 * @defgroup lavu_mem_funcs Heap Management
 * Functions responsible for allocating, freeing, and copying memory.
 *
 * All memory allocation functions have a built-in upper limit of `INT_MAX`
 * bytes. This may be changed with av_max_alloc(), although exercise extreme
 * caution when doing so.
 *
 * @{
 */

/**
 * Allocate a memory block with alignment suitable for all memory accesses
 * (including vectors if available on the CPU).
 *
 * @param size Size in bytes for the memory block to be allocated
 * @return Pointer to the allocated block, or `NULL` if the block cannot
 *         be allocated
 * @see av_mallocz()
 */
void *av_malloc(size_t size) av_malloc_attrib av_alloc_size(1);

/**
 * Allocate a memory block with alignment suitable for all memory accesses
 * (including vectors if available on the CPU) and zero all the bytes of the
 * block.
 *
 * @param size Size in bytes for the memory block to be allocated
 * @return Pointer to the allocated block, or `NULL` if it cannot be allocated
 * @see av_malloc()
 */
void *av_mallocz(size_t size) av_malloc_attrib av_alloc_size(1);

/**
 * Allocate a memory block for an array with av_malloc().
 *
 * The allocated memory will have size `size * nmemb` bytes.
 *
 * @param nmemb Number of element
 * @param size  Size of a single element
 * @return Pointer to the allocated block, or `NULL` if the block cannot
 *         be allocated
 * @see av_malloc()
 */
av_alloc_size(1, 2) void *av_malloc_array(size_t nmemb, size_t size);

/**
 * Allocate a memory block for an array with av_mallocz().
 *
 * The allocated memory will have size `size * nmemb` bytes.
 *
 * @param nmemb Number of elements
 * @param size  Size of the single element
 * @return Pointer to the allocated block, or `NULL` if the block cannot
 *         be allocated
 *
 * @see av_mallocz()
 * @see av_malloc_array()
 */
void *av_calloc(size_t nmemb, size_t size) av_malloc_attrib av_alloc_size(1, 2);

/**
 * Allocate, reallocate, or free a block of memory.
 * 分配、重新分配或释放内存块。
 *
 * If `ptr` is `NULL` and `size` > 0, allocate a new block. Otherwise, expand or
 * shrink that block of memory according to `size`.
 * 如果“ptr”为“NULL”且“size”> 0，则分配一个新块。 否则，根据“大小”扩展或缩小该内存块。
 *
 * @param ptr  Pointer to a memory block already allocated with
 *             av_realloc() or `NULL`
 * 指向已使用 av_realloc()分配的内存块的指针或NULL
 *
 * @param size Size in bytes of the memory block to be allocated or
 *             reallocated
 * 要分配或重新分配的内存块的大小（以字节为单位）
 *
 * @return Pointer to a newly-reallocated block or `NULL` if the block
 *         cannot be reallocated
 * 指向新重新分配的块的指针，如果该块无法重新分配，则为“NULL”
 *
 * @warning Unlike av_malloc(), the returned pointer is not guaranteed to be
 *          correctly aligned. The returned pointer must be freed after even
 *          if size is zero.
 * 与 av_malloc() 不同，返回的指针不能保证正确对齐。 即使大小为零，返回的指针也必须释放。
 * @see av_fast_realloc()
 * @see av_reallocp()
 */
//done
void *av_realloc(void *ptr, size_t size) av_alloc_size(2);

/**
 * Allocate, reallocate, or free a block of memory through a pointer to a
 * pointer.
 *
 * If `*ptr` is `NULL` and `size` > 0, allocate a new block. If `size` is
 * zero, free the memory block pointed to by `*ptr`. Otherwise, expand or
 * shrink that block of memory according to `size`.
 *
 * @param[in,out] ptr  Pointer to a pointer to a memory block already allocated
 *                     with av_realloc(), or a pointer to `NULL`. The pointer
 *                     is updated on success, or freed on failure.
 * @param[in]     size Size in bytes for the memory block to be allocated or
 *                     reallocated
 *
 * @return Zero on success, an AVERROR error code on failure
 *
 * @warning Unlike av_malloc(), the allocated memory is not guaranteed to be
 *          correctly aligned.
 */
av_warn_unused_result
int av_reallocp(void *ptr, size_t size);

/**
 * Allocate, reallocate, or free a block of memory.
 *
 * This function does the same thing as av_realloc(), except:
 * - It takes two size arguments and allocates `nelem * elsize` bytes,
 *   after checking the result of the multiplication for integer overflow.
 * - It frees the input block in case of failure, thus avoiding the memory
 *   leak with the classic
 *   @code{.c}
 *   buf = realloc(buf);
 *   if (!buf)
 *       return -1;
 *   @endcode
 *   pattern.
 */
void *av_realloc_f(void *ptr, size_t nelem, size_t elsize);

/**
 * Allocate, reallocate, or free an array.
 *
 * If `ptr` is `NULL` and `nmemb` > 0, allocate a new block.
 *
 * @param ptr   Pointer to a memory block already allocated with
 *              av_realloc() or `NULL`
 * @param nmemb Number of elements in the array
 * @param size  Size of the single element of the array
 *
 * @return Pointer to a newly-reallocated block or NULL if the block
 *         cannot be reallocated
 *
 * @warning Unlike av_malloc(), the allocated memory is not guaranteed to be
 *          correctly aligned. The returned pointer must be freed after even if
 *          nmemb is zero.
 * @see av_reallocp_array()
 */
av_alloc_size(2, 3) void *av_realloc_array(void *ptr, size_t nmemb, size_t size);

/**
 * Allocate, reallocate an array through a pointer to a pointer.
 *
 * If `*ptr` is `NULL` and `nmemb` > 0, allocate a new block.
 *
 * @param[in,out] ptr   Pointer to a pointer to a memory block already
 *                      allocated with av_realloc(), or a pointer to `NULL`.
 *                      The pointer is updated on success, or freed on failure.
 * @param[in]     nmemb Number of elements
 * @param[in]     size  Size of the single element
 *
 * @return Zero on success, an AVERROR error code on failure
 *
 * @warning Unlike av_malloc(), the allocated memory is not guaranteed to be
 *          correctly aligned. *ptr must be freed after even if nmemb is zero.
 */
int av_reallocp_array(void *ptr, size_t nmemb, size_t size);

/**
 * Reallocate the given buffer if it is not large enough, otherwise do nothing.
 *
 * If the given buffer is `NULL`, then a new uninitialized buffer is allocated.
 *
 * If the given buffer is not large enough, and reallocation fails, `NULL` is
 * returned and `*size` is set to 0, but the original buffer is not changed or
 * freed.
 *
 * A typical use pattern follows:
 *
 * @code{.c}
 * uint8_t *buf = ...;
 * uint8_t *new_buf = av_fast_realloc(buf, &current_size, size_needed);
 * if (!new_buf) {
 *     // Allocation failed; clean up original buffer
 *     av_freep(&buf);
 *     return AVERROR(ENOMEM);
 * }
 * @endcode
 *
 * @param[in,out] ptr      Already allocated buffer, or `NULL`
 * @param[in,out] size     Pointer to the size of buffer `ptr`. `*size` is
 *                         updated to the new allocated size, in particular 0
 *                         in case of failure.
 * @param[in]     min_size Desired minimal size of buffer `ptr`
 * @return `ptr` if the buffer is large enough, a pointer to newly reallocated
 *         buffer if the buffer was not large enough, or `NULL` in case of
 *         error
 * @see av_realloc()
 * @see av_fast_malloc()
 */
void *av_fast_realloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * Allocate a buffer, reusing the given one if large enough.
 *
 * Contrary to av_fast_realloc(), the current buffer contents might not be
 * preserved and on error the old buffer is freed, thus no special handling to
 * avoid memleaks is necessary.
 *
 * `*ptr` is allowed to be `NULL`, in which case allocation always happens if
 * `size_needed` is greater than 0.
 *
 * @code{.c}
 * uint8_t *buf = ...;
 * av_fast_malloc(&buf, &current_size, size_needed);
 * if (!buf) {
 *     // Allocation failed; buf already freed
 *     return AVERROR(ENOMEM);
 * }
 * @endcode
 *
 * @param[in,out] ptr      Pointer to pointer to an already allocated buffer.
 *                         `*ptr` will be overwritten with pointer to new
 *                         buffer on success or `NULL` on failure
 * @param[in,out] size     Pointer to the size of buffer `*ptr`. `*size` is
 *                         updated to the new allocated size, in particular 0
 *                         in case of failure.
 * @param[in]     min_size Desired minimal size of buffer `*ptr`
 * @see av_realloc()
 * @see av_fast_mallocz()
 */
void av_fast_malloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * Allocate and clear a buffer, reusing the given one if large enough.
 *
 * Like av_fast_malloc(), but all newly allocated space is initially cleared.
 * Reused buffer is not cleared.
 *
 * `*ptr` is allowed to be `NULL`, in which case allocation always happens if
 * `size_needed` is greater than 0.
 *
 * @param[in,out] ptr      Pointer to pointer to an already allocated buffer.
 *                         `*ptr` will be overwritten with pointer to new
 *                         buffer on success or `NULL` on failure
 * @param[in,out] size     Pointer to the size of buffer `*ptr`. `*size` is
 *                         updated to the new allocated size, in particular 0
 *                         in case of failure.
 * @param[in]     min_size Desired minimal size of buffer `*ptr`
 * @see av_fast_malloc()
 */
void av_fast_mallocz(void *ptr, unsigned int *size, size_t min_size);

/**
 * Free a memory block which has been allocated with a function of av_malloc()
 * or av_realloc() family.
 * 释放已使用 av_malloc() 或 av_realloc() 系列函数分配的内存块。
 *
 * @param ptr Pointer to the memory block which should be freed.
 * 指向应释放的内存块的指针。
 *
 * @note `ptr = NULL` is explicitly allowed.
 * `ptr = NULL` 是明确允许的。
 * @note It is recommended that you use av_freep() instead, to prevent leaving
 *       behind dangling pointers.
 * 建议您使用 av_freep() 代替，以防止留下悬空指针。
 * @see av_freep()
 */
//done
void av_free(void *ptr);

/**
 * Free a memory block which has been allocated with a function of av_malloc()
 * or av_realloc() family, and set the pointer pointing to it to `NULL`.
 * 释放已使用 av_malloc() 或 av_realloc() 系列函数分配的内存块，并将指向它的指针设置为“NULL”。
 *
 * @code{.c}
 * uint8_t *buf = av_malloc(16);
 * av_free(buf);
 * // buf now contains a dangling pointer to freed memory, and accidental
 * // dereference of buf will result in a use-after-free, which may be a
 * // security risk.
 * buf 现在包含一个指向已释放内存的悬空指针，意外解引用 buf 将导致释放后使用，这可能会带来安全风险。
 *
 * uint8_t *buf = av_malloc(16);
 * av_freep(&buf);
 * // buf is now NULL, and accidental dereference will only result in a
 * // NULL-pointer dereference.
 * buf 现在为 NULL，意外解引用只会导致 NULL 指针解引用。
 * @endcode
 *
 * @param ptr Pointer to the pointer to the memory block which should be freed
 * 指向应该释放的内存块的指针
 * @note `*ptr = NULL` is safe and leads to no action.
 * `*ptr = NULL` 是安全的并且不会导致任何操作。
 * @see av_free()
 */
//done
void av_freep(void *ptr);

/**
 * Duplicate a string.
 * 复制字符串。
 *
 * @param s String to be duplicated
 * 要复制的字符串
 * @return Pointer to a newly-allocated string containing a
 *         copy of `s` or `NULL` if the string cannot be allocated
 * 指向新分配的字符串的指针，其中包含“s”的副本，如果无法分配该字符串，则为“NULL”
 * @see av_strndup()
 */
//done
char *av_strdup(const char *s) av_malloc_attrib;

/**
 * Duplicate a substring of a string.
 *
 * @param s   String to be duplicated
 * @param len Maximum length of the resulting string (not counting the
 *            terminating byte)
 * @return Pointer to a newly-allocated string containing a
 *         substring of `s` or `NULL` if the string cannot be allocated
 */
char *av_strndup(const char *s, size_t len) av_malloc_attrib;

/**
 * Duplicate a buffer with av_malloc().
 *
 * @param p    Buffer to be duplicated
 * @param size Size in bytes of the buffer copied
 * @return Pointer to a newly allocated buffer containing a
 *         copy of `p` or `NULL` if the buffer cannot be allocated
 */
void *av_memdup(const void *p, size_t size);

/**
 * Overlapping memcpy() implementation.
 *
 * @param dst  Destination buffer
 * @param back Number of bytes back to start copying (i.e. the initial size of
 *             the overlapping window); must be > 0
 * @param cnt  Number of bytes to copy; must be >= 0
 *
 * @note `cnt > back` is valid, this will copy the bytes we just copied,
 *       thus creating a repeating pattern with a period length of `back`.
 */
void av_memcpy_backptr(uint8_t *dst, int back, int cnt);

/**
 * @}
 */

/**
 * @defgroup lavu_mem_dynarray Dynamic Array
 *
 * Utilities to make an array grow when needed.
 *
 * Sometimes, the programmer would want to have an array that can grow when
 * needed. The libavutil dynamic array utilities fill that need.
 *
 * libavutil supports two systems of appending elements onto a dynamically
 * allocated array, the first one storing the pointer to the value in the
 * array, and the second storing the value directly. In both systems, the
 * caller is responsible for maintaining a variable containing the length of
 * the array, as well as freeing of the array after use.
 *
 * The first system stores pointers to values in a block of dynamically
 * allocated memory. Since only pointers are stored, the function does not need
 * to know the size of the type. Both av_dynarray_add() and
 * av_dynarray_add_nofree() implement this system.
 *
 * @code
 * type **array = NULL; //< an array of pointers to values
 * int    nb    = 0;    //< a variable to keep track of the length of the array
 *
 * type to_be_added  = ...;
 * type to_be_added2 = ...;
 *
 * av_dynarray_add(&array, &nb, &to_be_added);
 * if (nb == 0)
 *     return AVERROR(ENOMEM);
 *
 * av_dynarray_add(&array, &nb, &to_be_added2);
 * if (nb == 0)
 *     return AVERROR(ENOMEM);
 *
 * // Now:
 * //  nb           == 2
 * // &to_be_added  == array[0]
 * // &to_be_added2 == array[1]
 *
 * av_freep(&array);
 * @endcode
 *
 * The second system stores the value directly in a block of memory. As a
 * result, the function has to know the size of the type. av_dynarray2_add()
 * implements this mechanism.
 *
 * @code
 * type *array = NULL; //< an array of values
 * int   nb    = 0;    //< a variable to keep track of the length of the array
 *
 * type to_be_added  = ...;
 * type to_be_added2 = ...;
 *
 * type *addr = av_dynarray2_add((void **)&array, &nb, sizeof(*array), NULL);
 * if (!addr)
 *     return AVERROR(ENOMEM);
 * memcpy(addr, &to_be_added, sizeof(to_be_added));
 *
 * // Shortcut of the above.
 * type *addr = av_dynarray2_add((void **)&array, &nb, sizeof(*array),
 *                               (const void *)&to_be_added2);
 * if (!addr)
 *     return AVERROR(ENOMEM);
 *
 * // Now:
 * //  nb           == 2
 * //  to_be_added  == array[0]
 * //  to_be_added2 == array[1]
 *
 * av_freep(&array);
 * @endcode
 *
 * @{
 */

/**
 * Add the pointer to an element to a dynamic array.
 *
 * The array to grow is supposed to be an array of pointers to
 * structures, and the element to add must be a pointer to an already
 * allocated structure.
 *
 * The array is reallocated when its size reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the number pointed to by `nb_ptr`
 * is incremented.
 * In case of failure, the array is freed, `*tab_ptr` is set to `NULL` and
 * `*nb_ptr` is set to 0.
 *
 * @param[in,out] tab_ptr Pointer to the array to grow
 * @param[in,out] nb_ptr  Pointer to the number of elements in the array
 * @param[in]     elem    Element to add
 * @see av_dynarray_add_nofree(), av_dynarray2_add()
 */
void av_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem);

/**
 * Add an element to a dynamic array.
 *
 * Function has the same functionality as av_dynarray_add(),
 * but it doesn't free memory on fails. It returns error code
 * instead and leave current buffer untouched.
 *
 * @return >=0 on success, negative otherwise
 * @see av_dynarray_add(), av_dynarray2_add()
 */
av_warn_unused_result
int av_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem);

/**
 * Add an element of size `elem_size` to a dynamic array.
 *
 * The array is reallocated when its number of elements reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the number pointed to by `nb_ptr`
 * is incremented.
 * In case of failure, the array is freed, `*tab_ptr` is set to `NULL` and
 * `*nb_ptr` is set to 0.
 *
 * @param[in,out] tab_ptr   Pointer to the array to grow
 * @param[in,out] nb_ptr    Pointer to the number of elements in the array
 * @param[in]     elem_size Size in bytes of an element in the array
 * @param[in]     elem_data Pointer to the data of the element to add. If
 *                          `NULL`, the space of the newly added element is
 *                          allocated but left uninitialized.
 *
 * @return Pointer to the data of the element to copy in the newly allocated
 *         space
 * @see av_dynarray_add(), av_dynarray_add_nofree()
 */
void *av_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data);

/**
 * @}
 */

/**
 * @defgroup lavu_mem_misc Miscellaneous Functions
 *
 * Other functions related to memory allocation.
 *
 * @{
 */

/**
 * Multiply two `size_t` values checking for overflow.
 *
 * @param[in]  a   Operand of multiplication
 * @param[in]  b   Operand of multiplication
 * @param[out] r   Pointer to the result of the operation
 * @return 0 on success, AVERROR(EINVAL) on overflow
 */
int av_size_mult(size_t a, size_t b, size_t *r);

/**
 * Set the maximum size that may be allocated in one block.
 *
 * The value specified with this function is effective for all libavutil's @ref
 * lavu_mem_funcs "heap management functions."
 *
 * By default, the max value is defined as `INT_MAX`.
 *
 * @param max Value to be set as the new maximum size
 *
 * @warning Exercise extreme caution when using this function. Don't touch
 *          this if you do not understand the full consequence of doing so.
 */
void av_max_alloc(size_t max);

/**
 * @}
 * @}
 */

#endif /* AVUTIL_MEM_H */
