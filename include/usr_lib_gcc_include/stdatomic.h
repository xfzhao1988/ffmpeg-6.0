/* Copyright (C) 2013-2019 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/* ISO C11 Standard:  7.17  Atomics <stdatomic.h>.  */
/*
关于原子操作可参考如下链接：
文档链接：
1. C语言标准翻译版7.17：ISOIEC 98992011(en)，https://mb9axv8j44.feishu.cn/file/KVaRbmr3noFHzAx2fWOceWOenrF
2. 闲散文章：重要
(1) 内存顺序问题（一）: https://www.toutiao.com/article/7121910792928330243/?app=news_article&timestamp=1720008569&use_new_style=1&req_id=20240703200929A5C7BB61E8C1E02A19D9&group_id=7121910792928330243&pseries_type=0&pseries_style_type=2&pseries_id=7128358909588668942&share_token=923d3171-cfac-422d-8c25-0d2546ff3af0&source=m_redirect
(2) 内存顺序问题（二）https://www.toutiao.com/article/7121911379866698240/?channel=&source=search_tab
3. memory_order解释中文版：https://www.apiref.com/cpp-zh/c/atomic/memory_order.html
   memory_order解释英文版：https://en.cppreference.com/w/c/atomic/memory_order
4. C11标准的原子操作详解: https://github.com/zenny-chen/C11-atomic-operations-in-detail：重要
5. C++11新特性内存模型总结详解--一篇秒懂：https://www.cnblogs.com/bclshuai/p/15898116.html
6. compiler explorer: https://godbolt.org/
7. https://cloud.tencent.com/developer/article/1638672：
8. https://www.toutiao.com/article/7109778380631015968/?channel=&source=search_tab：重要
（1）https://mp.weixin.qq.com/s/t5_Up2YZEZt1NLbvgYz9FQ：重要

视频链接：
1. https://www.bilibili.com/video/BV1Bz4y1Q7E1/?spm_id_from=333.337.search-card.all.click&vd_source=084acc4a762745f776be7fbf50285d56
*/
#ifndef _STDATOMIC_H
#define _STDATOMIC_H


typedef enum
  {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
  } memory_order;

//done
typedef _Atomic _Bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic signed char atomic_schar;
typedef _Atomic unsigned char atomic_uchar;
typedef _Atomic short atomic_short;
typedef _Atomic unsigned short atomic_ushort;
typedef _Atomic int atomic_int;
typedef _Atomic unsigned int atomic_uint;
typedef _Atomic long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic __CHAR16_TYPE__ atomic_char16_t;
typedef _Atomic __CHAR32_TYPE__ atomic_char32_t;
typedef _Atomic __WCHAR_TYPE__ atomic_wchar_t;
typedef _Atomic __INT_LEAST8_TYPE__ atomic_int_least8_t;
typedef _Atomic __UINT_LEAST8_TYPE__ atomic_uint_least8_t;
typedef _Atomic __INT_LEAST16_TYPE__ atomic_int_least16_t;
typedef _Atomic __UINT_LEAST16_TYPE__ atomic_uint_least16_t;
typedef _Atomic __INT_LEAST32_TYPE__ atomic_int_least32_t;
typedef _Atomic __UINT_LEAST32_TYPE__ atomic_uint_least32_t;
typedef _Atomic __INT_LEAST64_TYPE__ atomic_int_least64_t;
typedef _Atomic __UINT_LEAST64_TYPE__ atomic_uint_least64_t;
typedef _Atomic __INT_FAST8_TYPE__ atomic_int_fast8_t;
typedef _Atomic __UINT_FAST8_TYPE__ atomic_uint_fast8_t;
typedef _Atomic __INT_FAST16_TYPE__ atomic_int_fast16_t;
typedef _Atomic __UINT_FAST16_TYPE__ atomic_uint_fast16_t;
typedef _Atomic __INT_FAST32_TYPE__ atomic_int_fast32_t;
typedef _Atomic __UINT_FAST32_TYPE__ atomic_uint_fast32_t;
typedef _Atomic __INT_FAST64_TYPE__ atomic_int_fast64_t;
typedef _Atomic __UINT_FAST64_TYPE__ atomic_uint_fast64_t;
typedef _Atomic __INTPTR_TYPE__ atomic_intptr_t;
typedef _Atomic __UINTPTR_TYPE__ atomic_uintptr_t;
typedef _Atomic __SIZE_TYPE__ atomic_size_t;
typedef _Atomic __PTRDIFF_TYPE__ atomic_ptrdiff_t;
typedef _Atomic __INTMAX_TYPE__ atomic_intmax_t;
typedef _Atomic __UINTMAX_TYPE__ atomic_uintmax_t;

//done
#define ATOMIC_VAR_INIT(VALUE)	(VALUE)

/* Initialize an atomic object pointed to by PTR with VAL. */
/* 用 VAL 初始化 PTR 指向的原子对象。 */
/*
这段代码是一个宏定义，用于初始化原子变量。让我解释一下它的作用和具体实现：

宏定义解析
#define atomic_init(PTR, VAL) \
  atomic_store_explicit(PTR, VAL, __ATOMIC_RELAXED)

atomic_init(PTR, VAL)：这是一个宏，用于初始化原子变量。
atomic_store_explicit(PTR, VAL, __ATOMIC_RELAXED)：
atomic_store_explicit 是一个函数或宏，用于以特定的内存顺序（这里是 __ATOMIC_RELAXED，即松散的内存顺序）存储值。

PTR 是指向原子变量的指针。
VAL 是要存储的值。
__ATOMIC_RELAXED 表示使用松散的内存顺序进行存储操作，即没有特定的顺序要求，操作可以被重排序。

功能与用法
功能：atomic_init 宏用于将 PTR 指向的原子变量初始化为 VAL。
内存顺序：使用 __ATOMIC_RELAXED 内存顺序意味着该初始化操作可以被编译器优化和重排序，不会引入额外的同步开销，
适用于不需要特定顺序保证的场景。

示例用法
假设有一个 atomic_int 类型的原子变量 my_atomic_var，可以这样初始化：

atomic_int my_atomic_var;
atomic_init(&my_atomic_var, 0);
这将会使用宏定义展开为类似以下的代码：

atomic_store_explicit(&my_atomic_var, 0, __ATOMIC_RELAXED);

这确保了 my_atomic_var 变量被原子地初始化为 0，使用了松散的内存顺序，适合于大多数不需要强顺序保证的情况下。
*/
#define atomic_init(PTR, VAL)                           \
  atomic_store_explicit (PTR, VAL, __ATOMIC_RELAXED)

#define kill_dependency(Y)			\
  __extension__					\
  ({						\
    __auto_type __kill_dependency_tmp = (Y);	\
    __kill_dependency_tmp;			\
  })

extern void atomic_thread_fence (memory_order);
#define atomic_thread_fence(MO)	__atomic_thread_fence (MO)
extern void atomic_signal_fence (memory_order);
#define atomic_signal_fence(MO)	__atomic_signal_fence  (MO)
#define atomic_is_lock_free(OBJ) __atomic_is_lock_free (sizeof (*(OBJ)), (OBJ))

/**
这些预定义宏（如 ATOMIC_BOOL_LOCK_FREE 等）是用来检查某种类型在目标平台上是否可以无锁（lock-free）
地实现原子操作的。这些宏的值通常是 0、1 或 2：

0：类型的原子操作不是无锁的。
1：类型的原子操作可能是无锁的。
2：类型的原子操作一定是无锁的。

这些宏常用于编写跨平台代码，以确保在不同平台上类型的原子操作具有一致的行为。

下面是一个使用这些宏的示例代码，它检查某些类型是否可以进行无锁的原子操作：

#include <iostream>
#include <atomic>

int main() {
    std::cout << "ATOMIC_BOOL_LOCK_FREE: " << ATOMIC_BOOL_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_CHAR_LOCK_FREE: " << ATOMIC_CHAR_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_CHAR16_T_LOCK_FREE: " << ATOMIC_CHAR16_T_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_CHAR32_T_LOCK_FREE: " << ATOMIC_CHAR32_T_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_WCHAR_T_LOCK_FREE: " << ATOMIC_WCHAR_T_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_SHORT_LOCK_FREE: " << ATOMIC_SHORT_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_INT_LOCK_FREE: " << ATOMIC_INT_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_LONG_LOCK_FREE: " << ATOMIC_LONG_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_LLONG_LOCK_FREE: " << ATOMIC_LLONG_LOCK_FREE << std::endl;
    std::cout << "ATOMIC_POINTER_LOCK_FREE: " << ATOMIC_POINTER_LOCK_FREE << std::endl;

    if (ATOMIC_INT_LOCK_FREE == 2) {
        std::atomic<int> atomicInt(0);
        int expected = 0;
        if (atomicInt.compare_exchange_strong(expected, 1)) {
            std::cout << "Atomic int operation is lock-free and successful." << std::endl;
        }
    } else {
        std::cout << "Atomic int operation is not guaranteed to be lock-free." << std::endl;
    }

    return 0;
}

这个示例代码会输出每个宏的值，并检查 int 类型是否可以进行无锁的原子操作。如果 ATOMIC_INT_LOCK_FREE 的值为 2，
则表示 int 类型的原子操作一定是无锁的，并尝试进行一次原子比较和交换操作。

在实际使用中，你可以根据这些宏的值决定是否使用某种类型的原子操作，或者选择其他的同步机制（如互斥锁）
来确保线程安全性。

*/
//done
#define ATOMIC_BOOL_LOCK_FREE		__GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE		__GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE	__GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE	__GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE	__GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE		__GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE		__GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE		__GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE		__GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE	__GCC_ATOMIC_POINTER_LOCK_FREE


/* Note that these macros require __typeof__ and __auto_type to remove
   _Atomic qualifiers (and const qualifiers, if those are valid on
   macro operands).

   Also note that the header file uses the generic form of __atomic
   builtins, which requires the address to be taken of the value
   parameter, and then we pass that value on.  This allows the macros
   to work for any type, and the compiler is smart enough to convert
   these to lock-free _N variants if possible, and throw away the
   temps.  */

#define atomic_store_explicit(PTR, VAL, MO)				\
  __extension__								\
  ({									\
    __auto_type __atomic_store_ptr = (PTR);				\
    __typeof__ (*__atomic_store_ptr) __atomic_store_tmp = (VAL);	\
    __atomic_store (__atomic_store_ptr, &__atomic_store_tmp, (MO));	\
  })
//done
#define atomic_store(PTR, VAL)				\
  atomic_store_explicit (PTR, VAL, __ATOMIC_SEQ_CST)

/**
 * @brief 这段代码定义了一个宏 atomic_load_explicit，用于以指定的内存顺序加载一个原子变量的值。
 *
 * 具体来说：
 *
 * __auto_type：这是一个 GNU C 扩展，用于声明一个变量并自动推导其类型。在这里，__auto_type 用于声明一个指向
 * 原子变量的指针，并自动推导其类型。
 *
 * __typeof__：这是 GNU C 扩展，用于获取给定表达式的类型。在这里，__typeof__ (*__atomic_load_ptr) 用于
 * 获取指向原子变量的指针所指向对象的类型。
 *
 * __atomic_load：这是 C11 中引入的原子操作函数，用于以指定的内存顺序加载原子变量的值。
 * 在这里，__atomic_load 被调用来加载原子变量的值，并将结果存储在临时变量 __atomic_load_tmp 中。
 *
 * __extension__：这是 GNU C 扩展，用于告诉编译器忽略对扩展语法的警告。
 *
 * 这个宏允许以指定的内存顺序加载原子变量的值，并返回加载的值。
 *
 */
#define atomic_load_explicit(PTR, MO)					\
  __extension__								\
  ({									\
    __auto_type __atomic_load_ptr = (PTR);				\
    __typeof__ (*__atomic_load_ptr) __atomic_load_tmp;			\
    __atomic_load (__atomic_load_ptr, &__atomic_load_tmp, (MO));	\
    __atomic_load_tmp;							\
  })
//done
#define atomic_load(PTR)  atomic_load_explicit (PTR, __ATOMIC_SEQ_CST)


#define atomic_exchange_explicit(PTR, VAL, MO)				\
  __extension__								\
  ({									\
    __auto_type __atomic_exchange_ptr = (PTR);				\
    __typeof__ (*__atomic_exchange_ptr) __atomic_exchange_val = (VAL);	\
    __typeof__ (*__atomic_exchange_ptr) __atomic_exchange_tmp;		\
    __atomic_exchange (__atomic_exchange_ptr, &__atomic_exchange_val,	\
		       &__atomic_exchange_tmp, (MO));			\
    __atomic_exchange_tmp;						\
  })
//done
#define atomic_exchange(PTR, VAL) 			\
  atomic_exchange_explicit (PTR, VAL, __ATOMIC_SEQ_CST)


#define atomic_compare_exchange_strong_explicit(PTR, VAL, DES, SUC, FAIL) \
  __extension__								\
  ({									\
    __auto_type __atomic_compare_exchange_ptr = (PTR);			\
    __typeof__ (*__atomic_compare_exchange_ptr) __atomic_compare_exchange_tmp \
      = (DES);								\
    __atomic_compare_exchange (__atomic_compare_exchange_ptr, (VAL),	\
			       &__atomic_compare_exchange_tmp, 0,	\
			       (SUC), (FAIL));				\
  })
//done
#define atomic_compare_exchange_strong(PTR, VAL, DES) 			   \
  atomic_compare_exchange_strong_explicit (PTR, VAL, DES, __ATOMIC_SEQ_CST, \
					   __ATOMIC_SEQ_CST)

#define atomic_compare_exchange_weak_explicit(PTR, VAL, DES, SUC, FAIL) \
  __extension__								\
  ({									\
    __auto_type __atomic_compare_exchange_ptr = (PTR);			\
    __typeof__ (*__atomic_compare_exchange_ptr) __atomic_compare_exchange_tmp \
      = (DES);								\
    __atomic_compare_exchange (__atomic_compare_exchange_ptr, (VAL),	\
			       &__atomic_compare_exchange_tmp, 1,	\
			       (SUC), (FAIL));				\
  })
//done
#define atomic_compare_exchange_weak(PTR, VAL, DES)			\
  atomic_compare_exchange_weak_explicit (PTR, VAL, DES, __ATOMIC_SEQ_CST, \
					 __ATOMIC_SEQ_CST)


//done
#define atomic_fetch_add(PTR, VAL) __atomic_fetch_add ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_add_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_add ((PTR), (VAL), (MO))
//done
#define atomic_fetch_sub(PTR, VAL) __atomic_fetch_sub ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_sub_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_sub ((PTR), (VAL), (MO))
//done
#define atomic_fetch_or(PTR, VAL) __atomic_fetch_or ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_or_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_or ((PTR), (VAL), (MO))
//done
#define atomic_fetch_xor(PTR, VAL) __atomic_fetch_xor ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_xor_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_xor ((PTR), (VAL), (MO))
//done
#define atomic_fetch_and(PTR, VAL) __atomic_fetch_and ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_and_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_and ((PTR), (VAL), (MO))

//done
typedef _Atomic struct
{
#if __GCC_ATOMIC_TEST_AND_SET_TRUEVAL == 1
  _Bool __val;
#else
  unsigned char __val;
#endif
} atomic_flag;
//done
#define ATOMIC_FLAG_INIT	{ 0 }

/*
1. Volatile关键字的解释: https://www.runoob.com/w3cnote/c-volatile-keyword.html
*/
//done
extern _Bool atomic_flag_test_and_set (volatile atomic_flag *);
#define atomic_flag_test_and_set(PTR) 					\
			__atomic_test_and_set ((PTR), __ATOMIC_SEQ_CST)
extern _Bool atomic_flag_test_and_set_explicit (volatile atomic_flag *,
						memory_order);
#define atomic_flag_test_and_set_explicit(PTR, MO)			\
			__atomic_test_and_set ((PTR), (MO))

extern void atomic_flag_clear (volatile atomic_flag *);
#define atomic_flag_clear(PTR)	__atomic_clear ((PTR), __ATOMIC_SEQ_CST)
extern void atomic_flag_clear_explicit (volatile atomic_flag *, memory_order);
#define atomic_flag_clear_explicit(PTR, MO)   __atomic_clear ((PTR), (MO))

#endif  /* _STDATOMIC_H */
