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

#ifndef _STDATOMIC_H
#define _STDATOMIC_H

/**
 * @brief 这段代码定义了一个枚举类型 memory_order，用于表示 C++11 中的内存顺序（memory order）。
 * 具体来说：
 * memory_order_relaxed：这表示松散内存顺序，表示对原子操作的执行不会影响其他线程中的内存操作的执行顺序。
 * memory_order_consume：这表示消费内存顺序，用于在依赖于数据的值时，保证加载操作之后的读取操作在数据加载之后进行。
 * memory_order_acquire：这表示获取内存顺序，用于确保加载操作之前的读取操作在数据加载之前完成。
 * memory_order_release：这表示释放内存顺序，用于确保存储操作之后的写入操作在数据存储之后进行。
 * memory_order_acq_rel：这表示获取释放内存顺序，结合了获取和释放内存顺序的特性。
 * memory_order_seq_cst：这表示顺序一致内存顺序，是最严格的内存顺序，要求对原子操作的执行顺序遵循全局顺序。
 * 这个枚举类型用于在 C++11 中使用原子操作时指定内存顺序，以确保多线程程序的正确性。
 *
 */
typedef enum
  {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
  } memory_order;


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


#define ATOMIC_VAR_INIT(VALUE)	(VALUE)

/* Initialize an atomic object pointed to by PTR with VAL.  */
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

#define atomic_compare_exchange_weak(PTR, VAL, DES)			\
  atomic_compare_exchange_weak_explicit (PTR, VAL, DES, __ATOMIC_SEQ_CST, \
					 __ATOMIC_SEQ_CST)



#define atomic_fetch_add(PTR, VAL) __atomic_fetch_add ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_add_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_add ((PTR), (VAL), (MO))

#define atomic_fetch_sub(PTR, VAL) __atomic_fetch_sub ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_sub_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_sub ((PTR), (VAL), (MO))

#define atomic_fetch_or(PTR, VAL) __atomic_fetch_or ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_or_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_or ((PTR), (VAL), (MO))

#define atomic_fetch_xor(PTR, VAL) __atomic_fetch_xor ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_xor_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_xor ((PTR), (VAL), (MO))

#define atomic_fetch_and(PTR, VAL) __atomic_fetch_and ((PTR), (VAL), 	\
						       __ATOMIC_SEQ_CST)
#define atomic_fetch_and_explicit(PTR, VAL, MO) 			\
			  __atomic_fetch_and ((PTR), (VAL), (MO))


typedef _Atomic struct
{
#if __GCC_ATOMIC_TEST_AND_SET_TRUEVAL == 1
  _Bool __val;
#else
  unsigned char __val;
#endif
} atomic_flag;

#define ATOMIC_FLAG_INIT	{ 0 }


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
