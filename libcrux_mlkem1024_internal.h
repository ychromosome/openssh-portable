/* ML-KEM-1024 from libcrux c46481ce3cd1cc8315e90db114581d8c992c3d7d. */
/*
MIT License

Copyright (c) 2024 Cryspen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#if !defined(__GNUC__) || (__GNUC__ < 2)
# define __attribute__(x)
#endif
#define KRML_MUSTINLINE inline
#define KRML_NOINLINE __attribute__((noinline, unused))
#define KRML_HOST_EPRINTF(...)
#define KRML_HOST_EXIT(x) fatal_f("internal error")
#define KRML_UNION_CONSTRUCTOR(T)

static inline void
store64_le(uint8_t dst[8], uint64_t src)
{
	dst[0] = src & 0xff;
	dst[1] = (src >> 8) & 0xff;
	dst[2] = (src >> 16) & 0xff;
	dst[3] = (src >> 24) & 0xff;
	dst[4] = (src >> 32) & 0xff;
	dst[5] = (src >> 40) & 0xff;
	dst[6] = (src >> 48) & 0xff;
	dst[7] = (src >> 56) & 0xff;
}

static inline void
store32_le(uint8_t dst[4], uint32_t src)
{
	dst[0] = src & 0xff;
	dst[1] = (src >> 8) & 0xff;
	dst[2] = (src >> 16) & 0xff;
	dst[3] = (src >> 24) & 0xff;
}

static inline void
store16_le(uint8_t dst[2], uint16_t src)
{
	dst[0] = src & 0xff;
	dst[1] = (src >> 8) & 0xff;
}

static inline void
store32_be(uint8_t dst[4], uint32_t src)
{
	dst[0] = (src >> 24) & 0xff;
	dst[1] = (src >> 16) & 0xff;
	dst[2] = (src >> 8) & 0xff;
	dst[3] = src & 0xff;
}

static inline uint64_t
load64_le(uint8_t src[8])
{
	return (uint64_t)(src[0]) |
	    ((uint64_t)(src[1]) << 8) |
	    ((uint64_t)(src[2]) << 16) |
	    ((uint64_t)(src[3]) << 24) |
	    ((uint64_t)(src[4]) << 32) |
	    ((uint64_t)(src[5]) << 40) |
	    ((uint64_t)(src[6]) << 48) |
	    ((uint64_t)(src[7]) << 56);
}

static inline uint32_t
load32_le(uint8_t src[4])
{
	return (uint32_t)(src[0]) |
	    ((uint32_t)(src[1]) << 8) |
	    ((uint32_t)(src[2]) << 16) |
	    ((uint32_t)(src[3]) << 24);
}

static inline uint16_t
load16_le(uint8_t src[4])
{
	return (uint16_t)(src[0]) |
	    ((uint16_t)(src[1]) << 8);
}

#ifdef MISSING_BUILTIN_POPCOUNT
static inline unsigned int
__builtin_popcount(unsigned int num)
{
  const int v[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
  return v[num & 0xf] + v[(num >> 4) & 0xf];
}
#endif


/* from libcrux/libcrux-ml-kem/extracts/c/generated/karamel/include/krml/internal/target.h */
/* Copyright (c) INRIA and Microsoft Corporation. All rights reserved.
   Licensed under the Apache 2.0 and MIT Licenses. */

#ifndef __KRML_TARGET_H
#define __KRML_TARGET_H


/* Since KaRaMeL emits the inline keyword unconditionally, we follow the
 * guidelines at https://gcc.gnu.org/onlinedocs/gcc/Inline.html and make this
 * __inline__ to ensure the code compiles with -std=c90 and earlier. */
#ifdef __GNUC__
#define inline __inline__
#endif

/* There is no support for aligned_alloc() in macOS before Catalina, so
 * let's make a macro to use _mm_malloc() and _mm_free() functions
 * from mm_malloc.h. */
#if defined(__APPLE__) && defined(__MACH__)
#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) && \
    (MAC_OS_X_VERSION_MIN_REQUIRED < 101500)
#define LEGACY_MACOS
#else
#undef LEGACY_MACOS
#endif
#endif

/******************************************************************************/
/* Macros that KaRaMeL will generate.                                         */
/******************************************************************************/

/* For "bare" targets that do not have a C stdlib, the user might want to use
 * [-add-early-include '"mydefinitions.h"'] and override these. */
#ifndef KRML_HOST_PRINTF
#define KRML_HOST_PRINTF printf
#endif

#if ((defined __STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
     (!(defined KRML_HOST_EPRINTF)))
#define KRML_HOST_EPRINTF(...) fprintf(stderr, __VA_ARGS__)
#elif !(defined KRML_HOST_EPRINTF) && defined(_MSC_VER)
#define KRML_HOST_EPRINTF(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef KRML_HOST_EXIT
#define KRML_HOST_EXIT exit
#endif

#ifndef KRML_HOST_MALLOC
#define KRML_HOST_MALLOC malloc
#endif

#ifndef KRML_HOST_CALLOC
#define KRML_HOST_CALLOC calloc
#endif

#ifndef KRML_HOST_FREE
#define KRML_HOST_FREE free
#endif

#ifndef KRML_HOST_IGNORE
#define KRML_HOST_IGNORE(x) (void)(x)
#endif

#ifndef KRML_MAYBE_UNUSED_VAR
#define KRML_MAYBE_UNUSED_VAR(x) KRML_HOST_IGNORE(x)
#endif

#ifndef KRML_MAYBE_UNUSED
#if defined(__GNUC__) || defined(__clang__)
#define KRML_MAYBE_UNUSED __attribute__((unused))
#else
#define KRML_MAYBE_UNUSED
#endif
#endif

#ifndef KRML_ATTRIBUTE_TARGET
#if defined(__GNUC__) || defined(__clang__)
#define KRML_ATTRIBUTE_TARGET(x) __attribute__((target(x)))
#else
#define KRML_ATTRIBUTE_TARGET(x)
#endif
#endif

#ifndef KRML_NOINLINE
#if defined(__GNUC__) || defined(__clang__)
#define KRML_NOINLINE __attribute__((noinline, unused))
#elif defined(_MSC_VER)
#define KRML_NOINLINE __declspec(noinline)
#elif defined(__SUNPRO_C)
#define KRML_NOINLINE __attribute__((noinline))
#else
#define KRML_NOINLINE
#warning "The KRML_NOINLINE macro is not defined for this toolchain!"
#warning "The compiler may defeat side-channel resistance with optimizations."
#warning \
    "Please locate target.h and try to fill it out with a suitable definition for this compiler."
#endif
#endif

#ifndef KRML_MUSTINLINE
#if defined(_MSC_VER)
#define KRML_MUSTINLINE inline __forceinline
#elif defined(__GNUC__)
#define KRML_MUSTINLINE inline __attribute__((always_inline))
#elif defined(__SUNPRO_C)
#define KRML_MUSTINLINE inline __attribute__((always_inline))
#else
#define KRML_MUSTINLINE inline
#warning \
    "The KRML_MUSTINLINE macro defaults to plain inline for this toolchain!"
#warning \
    "Please locate target.h and try to fill it out with a suitable definition for this compiler."
#endif
#endif

#ifndef KRML_PRE_ALIGN
#ifdef _MSC_VER
#define KRML_PRE_ALIGN(X) __declspec(align(X))
#else
#define KRML_PRE_ALIGN(X)
#endif
#endif

#ifndef KRML_POST_ALIGN
#ifdef _MSC_VER
#define KRML_POST_ALIGN(X)
#else
#define KRML_POST_ALIGN(X) __attribute__((aligned(X)))
#endif
#endif

/* MinGW-W64 does not support C11 aligned_alloc, but it supports
 * MSVC's _aligned_malloc.
 */
#ifndef KRML_ALIGNED_MALLOC
#ifdef __MINGW32__
#endif
#if (defined(_MSC_VER) || \
     (defined(__MINGW32__) && defined(__MINGW64_VERSION_MAJOR)))
#define KRML_ALIGNED_MALLOC(X, Y) _aligned_malloc(Y, X)
#elif defined(LEGACY_MACOS)
#define KRML_ALIGNED_MALLOC(X, Y) _mm_malloc(Y, X)
#else
#define KRML_ALIGNED_MALLOC(X, Y) aligned_alloc(X, Y)
#endif
#endif

/* Since aligned allocations with MinGW-W64 are done with
 * _aligned_malloc (see above), such pointers must be freed with
 * _aligned_free.
 */
#ifndef KRML_ALIGNED_FREE
#ifdef __MINGW32__
#endif
#if (defined(_MSC_VER) || \
     (defined(__MINGW32__) && defined(__MINGW64_VERSION_MAJOR)))
#define KRML_ALIGNED_FREE(X) _aligned_free(X)
#elif defined(LEGACY_MACOS)
#define KRML_ALIGNED_FREE(X) _mm_free(X)
#else
#define KRML_ALIGNED_FREE(X) free(X)
#endif
#endif

#ifndef KRML_HOST_TIME


/* Prims_nat not yet in scope */
inline static __attribute__((unused)) int32_t krml_time(void) { return (int32_t)time(NULL); }

#define KRML_HOST_TIME krml_time
#endif

/* In statement position, exiting is easy. */
#define KRML_EXIT                                                              \
  do {                                                                         \
    KRML_HOST_PRINTF("Unimplemented function at %s:%d\n", __FILE__, __LINE__); \
    KRML_HOST_EXIT(254);                                                       \
  } while (0)

/* In expression position, use the comma-operator and a malloc to return an
 * expression of the right size. KaRaMeL passes t as the parameter to the macro.
 */
#define KRML_EABORT(t, msg)                                                   \
  (KRML_HOST_PRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__, msg), \
   KRML_HOST_EXIT(255), *((t *)KRML_HOST_MALLOC(sizeof(t))))

/* In FStar.Buffer.fst, the size of arrays is uint32_t, but it's a number of
 * *elements*. Do an ugly, run-time check (some of which KaRaMeL can eliminate).
 */
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 4))
#define _KRML_CHECK_SIZE_PRAGMA \
  _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")
#else
#define _KRML_CHECK_SIZE_PRAGMA
#endif

#define KRML_CHECK_SIZE(size_elt, sz)                                       \
  do {                                                                      \
    _KRML_CHECK_SIZE_PRAGMA                                                 \
    if (((size_t)(sz)) > ((size_t)(SIZE_MAX / (size_elt)))) {               \
      KRML_HOST_PRINTF(                                                     \
          "Maximum allocatable size exceeded, aborting before overflow at " \
          "%s:%d\n",                                                        \
          __FILE__, __LINE__);                                              \
      KRML_HOST_EXIT(253);                                                  \
    }                                                                       \
  } while (0)

#if defined(_MSC_VER) && _MSC_VER < 1900
#define KRML_HOST_SNPRINTF(buf, sz, fmt, arg) \
  _snprintf_s(buf, sz, _TRUNCATE, fmt, arg)
#else
#define KRML_HOST_SNPRINTF(buf, sz, fmt, arg) snprintf(buf, sz, fmt, arg)
#endif

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 4))
#define KRML_DEPRECATED(x) __attribute__((deprecated(x)))
#elif defined(__GNUC__)
/* deprecated attribute is not defined in GCC < 4.5. */
#define KRML_DEPRECATED(x)
#elif defined(__SUNPRO_C)
#define KRML_DEPRECATED(x) __attribute__((deprecated(x)))
#elif defined(_MSC_VER)
#define KRML_DEPRECATED(x) __declspec(deprecated(x))
#endif

/* Macros for prettier unrolling of loops */
#define KRML_LOOP1(i, n, x) \
  {                         \
    x i += n;               \
    (void)i;                \
  }

#define KRML_LOOP2(i, n, x) \
  KRML_LOOP1(i, n, x)       \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP3(i, n, x) \
  KRML_LOOP2(i, n, x)       \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP4(i, n, x) \
  KRML_LOOP2(i, n, x)       \
  KRML_LOOP2(i, n, x)

#define KRML_LOOP5(i, n, x) \
  KRML_LOOP4(i, n, x)       \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP6(i, n, x) \
  KRML_LOOP4(i, n, x)       \
  KRML_LOOP2(i, n, x)

#define KRML_LOOP7(i, n, x) \
  KRML_LOOP4(i, n, x)       \
  KRML_LOOP3(i, n, x)

#define KRML_LOOP8(i, n, x) \
  KRML_LOOP4(i, n, x)       \
  KRML_LOOP4(i, n, x)

#define KRML_LOOP9(i, n, x) \
  KRML_LOOP8(i, n, x)       \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP10(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP2(i, n, x)

#define KRML_LOOP11(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP3(i, n, x)

#define KRML_LOOP12(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP4(i, n, x)

#define KRML_LOOP13(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP5(i, n, x)

#define KRML_LOOP14(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP6(i, n, x)

#define KRML_LOOP15(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP7(i, n, x)

#define KRML_LOOP16(i, n, x) \
  KRML_LOOP8(i, n, x)        \
  KRML_LOOP8(i, n, x)

#define KRML_UNROLL_FOR(i, z, n, k, x) \
  do {                                 \
    uint32_t i = z;                    \
    KRML_LOOP##n(i, k, x)              \
  } while (0)

#define KRML_ACTUAL_FOR(i, z, n, k, x)    \
  do {                                    \
    for (uint32_t i = z; i < n; i += k) { \
      x                                   \
    }                                     \
  } while (0)

#ifndef KRML_UNROLL_MAX
#define KRML_UNROLL_MAX 16
#endif

/* 1 is the number of loop iterations, i.e. (n - z)/k as evaluated by krml */
#if 0 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR0(i, z, n, k, x)
#else
#define KRML_MAYBE_FOR0(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 1 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR1(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 1, k, x)
#else
#define KRML_MAYBE_FOR1(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 2 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR2(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 2, k, x)
#else
#define KRML_MAYBE_FOR2(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 3 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR3(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 3, k, x)
#else
#define KRML_MAYBE_FOR3(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 4 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR4(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 4, k, x)
#else
#define KRML_MAYBE_FOR4(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 5 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR5(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 5, k, x)
#else
#define KRML_MAYBE_FOR5(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 6 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR6(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 6, k, x)
#else
#define KRML_MAYBE_FOR6(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 7 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR7(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 7, k, x)
#else
#define KRML_MAYBE_FOR7(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 8 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR8(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 8, k, x)
#else
#define KRML_MAYBE_FOR8(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 9 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR9(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 9, k, x)
#else
#define KRML_MAYBE_FOR9(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 10 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR10(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 10, k, x)
#else
#define KRML_MAYBE_FOR10(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 11 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR11(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 11, k, x)
#else
#define KRML_MAYBE_FOR11(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 12 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR12(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 12, k, x)
#else
#define KRML_MAYBE_FOR12(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 13 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR13(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 13, k, x)
#else
#define KRML_MAYBE_FOR13(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 14 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR14(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 14, k, x)
#else
#define KRML_MAYBE_FOR14(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 15 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR15(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 15, k, x)
#else
#define KRML_MAYBE_FOR15(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 16 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR16(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 16, k, x)
#else
#define KRML_MAYBE_FOR16(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif
#endif


/* from libcrux/libcrux-ml-kem/extracts/c/generated/eurydice_glue.h */
#pragma once


#ifdef _MSC_VER
// For __popcnt
#endif


// C++ HELPERS

#if defined(__cplusplus)

#ifndef KRML_HOST_EPRINTF
#define KRML_HOST_EPRINTF(...) fprintf(stderr, __VA_ARGS__)
#endif


#ifndef __cpp_lib_type_identity
template <class T>
struct type_identity {
  using type = T;
};

template <class T>
using type_identity_t = typename type_identity<T>::type;
#else
using std::type_identity_t;
#endif

#define KRML_UNION_CONSTRUCTOR(T)                              \
  template <typename V>                                        \
  constexpr T(int t, V U::*m, type_identity_t<V> v) : tag(t) { \
    val.*m = std::move(v);                                     \
  }                                                            \
  T() = default;

#endif

// GENERAL-PURPOSE STUFF

#define LowStar_Ignore_ignore(e, t, _ret_t) ((void)e)

#define EURYDICE_ASSERT(test, msg)                                            \
  do {                                                                        \
    if (!(test)) {                                                            \
      fprintf(stderr, "assertion \"%s\" failed: file \"%s\", line %d\n", msg, \
              __FILE__, __LINE__);                                            \
      exit(255);                                                              \
    }                                                                         \
  } while (0)

// SIZEOF, ALIGNOF

#define Eurydice_sizeof(t) sizeof(t)

#define Eurydice_alignof(t) alignof(t)

// SLICES, ARRAYS, ETC.

// For convenience, we give these common slice types, below, a distinguished
// status and rather than emit them in the client code, we skip their
// code-generation in Cleanup3.ml and write them by hand here. This makes it
// easy to write interop code that brings those definitions in scope.

// &[u8]
typedef struct Eurydice_borrow_slice_u8_s {
  const uint8_t *ptr;
  size_t meta;
} Eurydice_borrow_slice_u8;

// &[u16]
typedef struct Eurydice_borrow_slice_i16_s {
  const int16_t *ptr;
  size_t meta;
} Eurydice_borrow_slice_i16;

// &mut [u8]
typedef struct Eurydice_mut_borrow_slice_u8_s {
  uint8_t *ptr;
  size_t meta;
} Eurydice_mut_borrow_slice_u8;

// &mut [u16]
typedef struct Eurydice_mut_borrow_slice_i16_s {
  int16_t *ptr;
  size_t meta;
} Eurydice_mut_borrow_slice_i16;

#if defined(__cplusplus)
#define KRML_CLITERAL(type) type
#else
#define KRML_CLITERAL(type) (type)
#endif

#if defined(__cplusplus) && defined(__cpp_designated_initializers) || \
    !(defined(__cplusplus))
#define EURYDICE_CFIELD(X) X
#else
#define EURYDICE_CFIELD(X)
#endif

#define Eurydice_array_repeat(dst, len, init, t) \
  ERROR "should've been desugared"

// Copy a slice with memcopy
#define Eurydice_slice_copy(dst, src, t) \
  memcpy(dst.ptr, src.ptr, dst.meta * sizeof(t))

#define core_array___T__N___as_slice(len_, ptr_, t, ret_t)   \
  (KRML_CLITERAL(ret_t){EURYDICE_CFIELD(.ptr =)(ptr_)->data, \
                        EURYDICE_CFIELD(.meta =) len_})

#define core_array__core__clone__Clone_for__T__N___clone(len, src, elem_type, \
                                                         _ret_t)              \
  (*(src))
#define TryFromSliceError uint8_t
#define core_array_TryFromSliceError uint8_t

// Distinguished support for some PartialEq trait implementations
//
// core::cmp::PartialEq<@Array<U, N>> for @Array<T, N>
#define Eurydice_array_eq(sz, a1, a2, t) \
  (memcmp((a1)->data, (a2)->data, sz * sizeof(t)) == 0)
// core::cmp::PartialEq<&0 (@Slice<U>)> for @Array<T, N>
#define Eurydice_array_eq_slice_shared(sz, a1, s2, t, _) \
  (memcmp((a1)->data, (s2)->ptr, sz * sizeof(t)) == 0)
#define Eurydice_array_eq_slice_mut(sz, a1, s2, t, _) \
  Eurydice_array_eq_slice_shared(sz, a1, s2, t, _)

// DEPRECATED -- should no longer be generated
#define core_array_equality__core__cmp__PartialEq__Array_U__N___for__Array_T__N___eq( \
    sz, a1, a2, t, _, _ret_t)                                                         \
  Eurydice_array_eq(sz, a1, a2, t)
#define core_array_equality__core__cmp__PartialEq__0___Slice_U____for__Array_T__N___eq( \
    sz, a1, a2, t, _, _ret_t)                                                           \
  Eurydice_array_eq(sz, a1, ((a2)->ptr), t)
#define core_cmp_impls__core__cmp__PartialEq__0_mut__B___for__1_mut__A___eq( \
    _m0, _m1, src1, src2, _0, _1, T)                                         \
  Eurydice_slice_eq(src1, src2, _, _, T, _)

#define Eurydice_slice_split_at(slice, mid, element_type, ret_t)        \
  KRML_CLITERAL(ret_t) {                                                \
    EURYDICE_CFIELD(.fst =){EURYDICE_CFIELD(.ptr =)((slice).ptr),       \
                            EURYDICE_CFIELD(.meta =) mid},              \
        EURYDICE_CFIELD(.snd =) {                                       \
      EURYDICE_CFIELD(.ptr =)                                           \
      ((slice).ptr + mid), EURYDICE_CFIELD(.meta =)((slice).meta - mid) \
    }                                                                   \
  }

#define Eurydice_slice_split_at_mut(slice, mid, element_type, ret_t)    \
  KRML_CLITERAL(ret_t) {                                                \
    EURYDICE_CFIELD(.fst =){EURYDICE_CFIELD(.ptr =)((slice).ptr),       \
                            EURYDICE_CFIELD(.meta =) mid},              \
        EURYDICE_CFIELD(.snd =) {                                       \
      EURYDICE_CFIELD(.ptr =)                                           \
      ((slice).ptr + mid), EURYDICE_CFIELD(.meta =)((slice).meta - mid) \
    }                                                                   \
  }

// Conversion of slice to an array, rewritten (by Eurydice) to name the
// destination array, since arrays are not values in C.
// N.B.: see note in karamel/lib/Inlining.ml if you change this.

#define Eurydice_slice_to_ref_array2(len_, src, arr_ptr, t_ptr, t_arr, t_err, \
                                     t_res)                                   \
  (src.meta >= len_                                                           \
       ? ((t_res){.tag = core_result_Ok, .val = {.case_Ok = arr_ptr}})        \
       : ((t_res){.tag = core_result_Err, .val = {.case_Err = 0}}))

// CORE STUFF (conversions, endianness, ...)

// We slap extern "C" on declarations that intend to implement a prototype
// generated by Eurydice, because Eurydice prototypes are always emitted within
// an extern "C" block, UNLESS you use -fcxx17-compat, in which case, you must
// pass -DKRML_CXX17_COMPAT="" to your C++ compiler.
#if defined(__cplusplus) && !defined(KRML_CXX17_COMPAT)
extern "C" {
#endif

#define core_hint_black_box(X, _0, _1) (X)

// [ u8; 2 ]
typedef struct Eurydice_array_u8x2_s {
  uint8_t data[2];
} Eurydice_array_u8x2;

// [ u8; 4 ]
typedef struct Eurydice_array_u8x4_s {
  uint8_t data[4];
} Eurydice_array_u8x4;

// [ u8; 8 ]
typedef struct Eurydice_array_u8x8_s {
  uint8_t data[8];
} Eurydice_array_u8x8;

static __attribute__((unused)) inline uint16_t core_num__u16__from_le_bytes(Eurydice_array_u8x2 buf) {
  return load16_le(buf.data);
}

static __attribute__((unused)) inline Eurydice_array_u8x4 core_num__u32__to_be_bytes(uint32_t src) {
  // TODO: why not store32_be?
  Eurydice_array_u8x4 a;
  uint32_t x = htobe32(src);
  memcpy(a.data, &x, 4);
  return a;
}

static __attribute__((unused)) inline Eurydice_array_u8x4 core_num__u32__to_le_bytes(uint32_t src) {
  Eurydice_array_u8x4 a;
  store32_le(a.data, src);
  return a;
}

static __attribute__((unused)) inline uint32_t core_num__u32__from_le_bytes(Eurydice_array_u8x4 buf) {
  return load32_le(buf.data);
}

static __attribute__((unused)) inline Eurydice_array_u8x8 core_num__u64__to_le_bytes(uint64_t v) {
  Eurydice_array_u8x8 a;
  store64_le(a.data, v);
  return a;
}

static __attribute__((unused)) inline uint64_t core_num__u64__from_le_bytes(Eurydice_array_u8x8 buf) {
  return load64_le(buf.data);
}

static __attribute__((unused)) inline int64_t core_convert_num__core__convert__From_i32__for_i64__from(
    int32_t x) {
  return x;
}

static __attribute__((unused)) inline uint64_t core_convert_num__core__convert__From_u8__for_u64__from(
    uint8_t x) {
  return x;
}

static __attribute__((unused)) inline uint64_t core_convert_num__core__convert__From_u16__for_u64__from(
    uint16_t x) {
  return x;
}

static __attribute__((unused)) inline size_t core_convert_num__core__convert__From_u16__for_usize__from(
    uint16_t x) {
  return x;
}

static __attribute__((unused)) inline uint32_t core_num__u8__count_ones(uint8_t x0) {
#ifdef _MSC_VER
  return __popcnt(x0);
#else
  return __builtin_popcount(x0);
#endif
}

static __attribute__((unused)) inline uint32_t core_num__u32__count_ones(uint32_t x0) {
#ifdef _MSC_VER
  return __popcnt(x0);
#else
  return __builtin_popcount(x0);
#endif
}

static __attribute__((unused)) inline uint32_t core_num__i32__count_ones(int32_t x0) {
#ifdef _MSC_VER
  return __popcnt(x0);
#else
  return __builtin_popcount(x0);
#endif
}

static __attribute__((unused)) inline size_t core_cmp_impls__core__cmp__Ord_for_usize__min(size_t a,
                                                                   size_t b) {
  if (a <= b)
    return a;
  else
    return b;
}

// unsigned overflow wraparound semantics in C
static __attribute__((unused)) inline uint8_t core_num__u8__wrapping_sub(uint8_t x, uint8_t y) {
  return x - y;
}
static __attribute__((unused)) inline uint8_t core_num__u8__wrapping_add(uint8_t x, uint8_t y) {
  return x + y;
}
static __attribute__((unused)) inline uint8_t core_num__u8__wrapping_mul(uint8_t x, uint8_t y) {
  return x * y;
}
static __attribute__((unused)) inline uint16_t core_num__u16__wrapping_sub(uint16_t x, uint16_t y) {
  return x - y;
}
static __attribute__((unused)) inline uint16_t core_num__u16__wrapping_add(uint16_t x, uint16_t y) {
  return x + y;
}
static __attribute__((unused)) inline uint16_t core_num__u16__wrapping_mul(uint16_t x, uint16_t y) {
  return x * y;
}
static __attribute__((unused)) inline uint32_t core_num__u32__wrapping_sub(uint32_t x, uint32_t y) {
  return x - y;
}
static __attribute__((unused)) inline uint32_t core_num__u32__wrapping_add(uint32_t x, uint32_t y) {
  return x + y;
}
static __attribute__((unused)) inline uint32_t core_num__u32__wrapping_mul(uint32_t x, uint32_t y) {
  return x * y;
}
static __attribute__((unused)) inline uint64_t core_num__u64__wrapping_sub(uint64_t x, uint64_t y) {
  return x - y;
}
static __attribute__((unused)) inline uint64_t core_num__u64__wrapping_add(uint64_t x, uint64_t y) {
  return x + y;
}
static __attribute__((unused)) inline uint64_t core_num__u64__wrapping_mul(uint64_t x, uint64_t y) {
  return x * y;
}
static __attribute__((unused)) inline size_t core_num__usize__wrapping_sub(size_t x, size_t y) {
  return x - y;
}
static __attribute__((unused)) inline size_t core_num__usize__wrapping_add(size_t x, size_t y) {
  return x + y;
}
static __attribute__((unused)) inline size_t core_num__usize__wrapping_mul(size_t x, size_t y) {
  return x * y;
}

static __attribute__((unused)) inline int8_t core_num__i8__wrapping_add(int8_t x, int8_t y) {
  return (int8_t)((uint8_t)x + (uint8_t)y);
}
static __attribute__((unused)) inline int8_t core_num__i8__wrapping_sub(int8_t x, int8_t y) {
  return (int8_t)((uint8_t)x - (uint8_t)y);
}
static __attribute__((unused)) inline int8_t core_num__i8__wrapping_mul(int8_t x, int8_t y) {
  return (int8_t)((uint8_t)x * (uint8_t)y);
}
static __attribute__((unused)) inline int16_t core_num__i16__wrapping_add(int16_t x, int16_t y) {
  return (int16_t)((uint16_t)x + (uint16_t)y);
}
static __attribute__((unused)) inline int16_t core_num__i16__wrapping_sub(int16_t x, int16_t y) {
  return (int16_t)((uint16_t)x - (uint16_t)y);
}
static __attribute__((unused)) inline int16_t core_num__i16__wrapping_mul(int16_t x, int16_t y) {
  return (int16_t)((uint16_t)x * (uint16_t)y);
}
static __attribute__((unused)) inline int32_t core_num__i32__wrapping_add(int32_t x, int32_t y) {
  return (int32_t)((uint32_t)x + (uint32_t)y);
}
static __attribute__((unused)) inline int32_t core_num__i32__wrapping_sub(int32_t x, int32_t y) {
  return (int32_t)((uint32_t)x - (uint32_t)y);
}
static __attribute__((unused)) inline int32_t core_num__i32__wrapping_mul(int32_t x, int32_t y) {
  return (int32_t)((uint32_t)x * (uint32_t)y);
}
static __attribute__((unused)) inline int64_t core_num__i64__wrapping_add(int64_t x, int64_t y) {
  return (int64_t)((uint64_t)x + (uint64_t)y);
}
static __attribute__((unused)) inline int64_t core_num__i64__wrapping_sub(int64_t x, int64_t y) {
  return (int64_t)((uint64_t)x - (uint64_t)y);
}
static __attribute__((unused)) inline int64_t core_num__i64__wrapping_mul(int64_t x, int64_t y) {
  return (int64_t)((uint64_t)x * (uint64_t)y);
}
static __attribute__((unused)) inline int8_t core_num__i8__wrapping_neg(int8_t x) {
  return (int8_t)(-(uint8_t)x);
}
static __attribute__((unused)) inline int16_t core_num__i16__wrapping_neg(int16_t x) {
  return (int16_t)(-(uint16_t)x);
}
static __attribute__((unused)) inline int32_t core_num__i32__wrapping_neg(int32_t x) {
  return (int32_t)(-(uint32_t)x);
}
static __attribute__((unused)) inline int64_t core_num__i64__wrapping_neg(int64_t x) {
  return (int64_t)(-(uint64_t)x);
}

static __attribute__((unused)) inline uint64_t core_num__u64__rotate_left(uint64_t x0, uint32_t x1) {
  return (x0 << x1) | (x0 >> ((-x1) & 63));
}

static __attribute__((unused)) inline void core_ops_arith__i32__add_assign(int32_t *x0, int32_t *x1) {
  *x0 = *x0 + *x1;
}

static __attribute__((unused)) inline uint8_t Eurydice_bitand_pv_u8(const uint8_t *p, uint8_t v) {
  return (*p) & v;
}
static __attribute__((unused)) inline uint8_t Eurydice_shr_pv_u8(const uint8_t *p, int32_t v) {
  return (*p) >> v;
}
static __attribute__((unused)) inline uint32_t Eurydice_min_u32(uint32_t x, uint32_t y) {
  return x < y ? x : y;
}

static __attribute__((unused)) inline uint8_t
core_ops_bit__core__ops__bit__BitAnd_u8__u8__for__0__u8___bitand(
    const uint8_t *x0, uint8_t x1) {
  return Eurydice_bitand_pv_u8(x0, x1);
}

static __attribute__((unused)) inline uint8_t
core_ops_bit__core__ops__bit__Shr_i32__u8__for__0__u8___shr(const uint8_t *x0,
                                                            int32_t x1) {
  return Eurydice_shr_pv_u8(x0, x1);
}

#define core_num_nonzero_private_NonZeroUsizeInner size_t
static __attribute__((unused)) inline core_num_nonzero_private_NonZeroUsizeInner
core_num_nonzero_private___core__clone__Clone_for_core__num__nonzero__private__NonZeroUsizeInner___clone(
    core_num_nonzero_private_NonZeroUsizeInner *x0) {
  return *x0;
}

#if defined(__cplusplus) && !defined(KRML_CXX17_COMPAT)
}
#endif

// ITERATORS

#define Eurydice_range_iter_next(iter_ptr, t, ret_t)      \
  (((iter_ptr)->start >= (iter_ptr)->end)                 \
       ? (KRML_CLITERAL(ret_t){EURYDICE_CFIELD(.tag =) 0, \
                               EURYDICE_CFIELD(.f0 =) 0}) \
       : (KRML_CLITERAL(ret_t){EURYDICE_CFIELD(.tag =) 1, \
                               EURYDICE_CFIELD(.f0 =)(iter_ptr)->start++}))

#define core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next \
  Eurydice_range_iter_next

// See note in karamel/lib/Inlining.ml if you change this
#define Eurydice_into_iter(x, t, _ret_t, _) (x)
#define core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter \
  Eurydice_into_iter

// STRINGS

typedef char Eurydice_c_char_t;
typedef const Eurydice_c_char_t *Prims_string;
typedef void Eurydice_c_void_t;

// UNSAFE CODE

#define core_slice___Slice_T___as_mut_ptr(x, t, _) (x.ptr)
#define core_mem_size_of(t, _) (sizeof(t))
#define core_slice_raw_from_raw_parts_mut(ptr, len, _0, _1) \
  (KRML_CLITERAL(Eurydice_slice){(void *)(ptr), len})
#define core_slice_raw_from_raw_parts(ptr, len, _0, _1) \
  (KRML_CLITERAL(Eurydice_slice){(void *)(ptr), len})

// FIXME: add dedicated extraction to extract NonNull<T> as T*
#define core_ptr_non_null_NonNull void *

// PRINTING
//
// This is temporary. Ultimately we want to be able to extract all of this.

typedef void *core_fmt_Formatter;
#define core_fmt_rt__core__fmt__rt__Argument__a___new_display(x1, x2, x3, x4) \
  NULL

// BOXES

#ifndef EURYDICE_MALLOC
#define EURYDICE_MALLOC malloc
#endif

#ifndef EURYDICE_REALLOC
#define EURYDICE_REALLOC realloc
#endif

static __attribute__((unused)) inline char *malloc_and_init(size_t sz, char *init) {
  char *ptr = (char *)EURYDICE_MALLOC(sz);
  if (ptr != NULL) memcpy(ptr, init, sz);
  return ptr;
}

#define Eurydice_box_new(init, t, t_dst) \
  ((t_dst)(malloc_and_init(sizeof(t), (char *)(&init))))

// Initializer for array of size zero
#define Eurydice_empty_array(dummy, t, t_dst) ((t_dst){.data = {}})

#define Eurydice_box_new_array(len, ptr, t, t_dst) \
  ((t_dst)(malloc_and_init(len * sizeof(t), (char *)(ptr))))

// FIXME this needs to handle allocation failure errors, but this seems hard to
// do without evaluating malloc_and_init twice...
#define alloc_boxed__alloc__boxed__Box_T___try_new(init, t, t_ret) \
  ((t_ret){.tag = core_result_Ok,                                  \
           .f0 = (t *)malloc_and_init(sizeof(t), (char *)(&init))})


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_core.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef libcrux_core_H
#define libcrux_core_H


#if defined(__cplusplus)
extern "C" {
#endif

#define LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE ((size_t)32U)

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $32size_t
*/
typedef struct Eurydice_arr_ec_s {
  uint8_t data[32U];
} Eurydice_arr_ec;

#define LIBCRUX_ML_KEM_CONSTANTS_BITS_PER_COEFFICIENT ((size_t)12U)

#define LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT ((size_t)256U)

#define LIBCRUX_ML_KEM_CONSTANTS_BITS_PER_RING_ELEMENT \
  (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT * (size_t)12U)

#define LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE ((size_t)32U)

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $3168size_t
*/
typedef struct Eurydice_arr_a8_s {
  uint8_t data[3168U];
} Eurydice_arr_a8;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1568size_t
*/
typedef struct Eurydice_arr_d1_s {
  uint8_t data[1568U];
} Eurydice_arr_d1;

typedef struct libcrux_ml_kem_mlkem1024_MlKem1024KeyPair_s {
  Eurydice_arr_a8 sk;
  Eurydice_arr_d1 pk;
} libcrux_ml_kem_mlkem1024_MlKem1024KeyPair;

/**
A monomorphic instance of n-tuple
with types libcrux_ml_kem_mlkem1024_MlKem1024Ciphertext, Eurydice_arr_ec

*/
typedef struct tuple_25_s {
  Eurydice_arr_d1 fst;
  Eurydice_arr_ec snd;
} tuple_25;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $168size_t
*/
typedef struct Eurydice_arr_c5_s {
  uint8_t data[168U];
} Eurydice_arr_c5;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $2400size_t
*/
typedef struct Eurydice_arr_7d_s {
  uint8_t data[2400U];
} Eurydice_arr_7d;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1184size_t
*/
typedef struct Eurydice_arr_5f_s {
  uint8_t data[1184U];
} Eurydice_arr_5f;

typedef struct libcrux_ml_kem_mlkem768_MlKem768KeyPair_s {
  Eurydice_arr_7d sk;
  Eurydice_arr_5f pk;
} libcrux_ml_kem_mlkem768_MlKem768KeyPair;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $24size_t
*/
typedef struct Eurydice_arr_94_s {
  uint8_t data[24U];
} Eurydice_arr_94;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $64size_t
*/
typedef struct Eurydice_arr_c7_s {
  uint8_t data[64U];
} Eurydice_arr_c7;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1088size_t
*/
typedef struct Eurydice_arr_2b_s {
  uint8_t data[1088U];
} Eurydice_arr_2b;

/**
A monomorphic instance of n-tuple
with types libcrux_ml_kem_mlkem768_MlKem768Ciphertext, Eurydice_arr_ec

*/
typedef struct tuple_f4_s {
  Eurydice_arr_2b fst;
  Eurydice_arr_ec snd;
} tuple_f4;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $10size_t
*/
typedef struct Eurydice_arr_6d_s {
  uint8_t data[10U];
} Eurydice_arr_6d;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $22size_t
*/
typedef struct Eurydice_arr_80_s {
  uint8_t data[22U];
} Eurydice_arr_80;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $20size_t
*/
typedef struct Eurydice_arr_fc_s {
  uint8_t data[20U];
} Eurydice_arr_fc;

/**
A monomorphic instance of Eurydice.arr
with types int16_t
with const generics
- $16size_t
*/
typedef struct Eurydice_arr_d6_s {
  int16_t data[16U];
} Eurydice_arr_d6;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $136size_t
*/
typedef struct Eurydice_arr_ff_s {
  uint8_t data[136U];
} Eurydice_arr_ff;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_ff
with const generics
- $1size_t
*/
typedef struct Eurydice_arr_0b_s {
  Eurydice_arr_ff data[1U];
} Eurydice_arr_0b;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_c5
with const generics
- $1size_t
*/
typedef struct Eurydice_arr_88_s {
  Eurydice_arr_c5 data[1U];
} Eurydice_arr_88;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $48size_t
*/
typedef struct Eurydice_arr_65_s {
  uint8_t data[48U];
} Eurydice_arr_65;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $28size_t
*/
typedef struct Eurydice_arr_a2_s {
  uint8_t data[28U];
} Eurydice_arr_a2;

/**
A monomorphic instance of Eurydice.arr
with types uint64_t
with const generics
- $5size_t
*/
typedef struct Eurydice_arr_84_s {
  uint64_t data[5U];
} Eurydice_arr_84;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_borrow_slice_u8
with const generics
- $1size_t
*/
typedef struct Eurydice_arr_dc_s {
  Eurydice_borrow_slice_u8 data[1U];
} Eurydice_arr_dc;

/**
A monomorphic instance of Eurydice.arr
with types uint64_t
with const generics
- $25size_t
*/
typedef struct Eurydice_arr_7c_s {
  uint64_t data[25U];
} Eurydice_arr_7c;

/**
A monomorphic instance of Eurydice.arr
with types uint64_t
with const generics
- $24size_t
*/
typedef struct Eurydice_arr_22_s {
  uint64_t data[24U];
} Eurydice_arr_22;

#if defined(__cplusplus)
}
#endif

#define libcrux_core_H_DEFINED
#endif /* libcrux_core_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/internal/libcrux_core.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef internal_libcrux_core_H
#define internal_libcrux_core_H


#if defined(__cplusplus)
extern "C" {
#endif


#define core_option_None 0
#define core_option_Some 1

typedef uint8_t core_option_Option_87_tags;

/**
A monomorphic instance of core.option.Option
with types size_t

*/
typedef struct core_option_Option_87_s {
  core_option_Option_87_tags tag;
  size_t f0;
} core_option_Option_87;

static __attribute__((unused)) inline uint64_t core_num__u64__from_le_bytes(Eurydice_array_u8x8 x0);

static __attribute__((unused)) inline uint64_t core_num__u64__rotate_left(uint64_t x0, uint32_t x1);

static __attribute__((unused)) inline Eurydice_array_u8x8 core_num__u64__to_le_bytes(uint64_t x0);

/**
A monomorphic instance of core.ops.range.Range
with types size_t

*/
typedef struct core_ops_range_Range_87_s {
  size_t start;
  size_t end;
} core_ops_range_Range_87;

static __attribute__((unused)) uint8_t libcrux_ml_kem_constant_time_ops_compare_ciphertexts_in_constant_time(
    Eurydice_borrow_slice_u8 lhs, Eurydice_borrow_slice_u8 rhs);

static __attribute__((unused)) Eurydice_arr_ec
libcrux_ml_kem_constant_time_ops_select_shared_secret_in_constant_time(
    Eurydice_borrow_slice_u8 lhs, Eurydice_borrow_slice_u8 rhs,
    uint8_t selector);

static __attribute__((unused)) Eurydice_arr_ec
libcrux_ml_kem_constant_time_ops_compare_ciphertexts_select_shared_secret_in_constant_time(
    Eurydice_borrow_slice_u8 lhs_c, Eurydice_borrow_slice_u8 rhs_c,
    Eurydice_borrow_slice_u8 lhs_s, Eurydice_borrow_slice_u8 rhs_s);

#define LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT \
  (LIBCRUX_ML_KEM_CONSTANTS_BITS_PER_RING_ELEMENT / (size_t)8U)

#define LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE ((size_t)32U)

/**
 K * BITS_PER_RING_ELEMENT / 8

 [eurydice] Note that we can't use const generics here because that breaks
            C extraction with eurydice.
*/
static __attribute__((unused)) size_t libcrux_ml_kem_constants_ranked_bytes_per_ring_element(size_t rank);

static __attribute__((unused)) int16_t libcrux_secrets_int_I16(int16_t v);

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types int16_t

*/
static __attribute__((unused)) int16_t libcrux_secrets_int_public_integers_classify_27_39(int16_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for u8}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_59(uint8_t self);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types int16_t

*/
static __attribute__((unused)) int16_t libcrux_secrets_int_public_integers_declassify_d8_39(int16_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) uint8_t libcrux_secrets_int_as_u8_f5(int16_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) int32_t libcrux_secrets_int_as_i32_f5(int16_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for i32}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_36(int32_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for u32}
*/
static __attribute__((unused)) int32_t libcrux_secrets_int_as_i32_b8(uint32_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) uint16_t libcrux_secrets_int_as_u16_f5(int16_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for u16}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_ca(uint16_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for u16}
*/
static __attribute__((unused)) uint64_t libcrux_secrets_int_as_u64_ca(uint16_t self);

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types uint32_t

*/
static __attribute__((unused)) uint32_t libcrux_secrets_int_public_integers_classify_27_df(uint32_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for u64}
*/
static __attribute__((unused)) uint32_t libcrux_secrets_int_as_u32_a3(uint64_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for u32}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_b8(uint32_t self);

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_f5(int16_t self);

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.default_d3
with const generics
- SIZE= 3168
*/
static __attribute__((unused)) Eurydice_arr_a8 libcrux_ml_kem_types_default_d3_0e(void);

/**
A monomorphic instance of Eurydice.array_to_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_to_shared_211(
    const Eurydice_arr_d1 *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 3168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d46(
    const Eurydice_arr_a8 *a, core_ops_range_Range_87 r);

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_51
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_d1 libcrux_ml_kem_types_from_51_d9(Eurydice_arr_d1 value);

/**
 Create a new [`MlKemKeyPair`] from the secret and public key.
*/
/**
This function found in impl
{libcrux_ml_kem::types::MlKemKeyPair<PRIVATE_KEY_SIZE, PUBLIC_KEY_SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_17
with const generics
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair libcrux_ml_kem_types_from_17_70(
    Eurydice_arr_a8 sk, Eurydice_arr_d1 pk);

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_b2
with const generics
- SIZE= 3168
*/
static __attribute__((unused)) Eurydice_arr_a8 libcrux_ml_kem_types_from_b2_0e(Eurydice_arr_a8 value);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1536size_t
*/
typedef struct Eurydice_arr_df_s {
  uint8_t data[1536U];
} Eurydice_arr_df;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1536
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_2f(
    const Eurydice_arr_df *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 3168
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d417(
    Eurydice_arr_a8 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 1536
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_2f(Eurydice_arr_df *a);

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_19
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_d1 libcrux_ml_kem_types_from_19_d9(Eurydice_arr_d1 value);

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_e6
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) const Eurydice_arr_d1 *libcrux_ml_kem_types_as_slice_e6_d9(
    const Eurydice_arr_d1 *self);

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_a9
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) const Eurydice_arr_d1 *libcrux_ml_kem_types_as_slice_a9_d9(
    const Eurydice_arr_d1 *self);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f4(
    Eurydice_arr_d1 *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d416(
    Eurydice_arr_d1 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $352size_t
*/
typedef struct Eurydice_arr_e7_s {
  uint8_t data[352U];
} Eurydice_arr_e7;

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 352
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d415(
    Eurydice_arr_e7 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 352
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_25(
    const Eurydice_arr_e7 *a);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $128size_t
*/
typedef struct Eurydice_arr_89_s {
  uint8_t data[128U];
} Eurydice_arr_89;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_89
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_3b_s {
  Eurydice_arr_89 data[4U];
} Eurydice_arr_3b;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $33size_t
*/
typedef struct Eurydice_arr_fa_s {
  uint8_t data[33U];
} Eurydice_arr_fa;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_fa
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_890_s {
  Eurydice_arr_fa data[4U];
} Eurydice_arr_890;

/**
A monomorphic instance of libcrux_ml_kem.utils.prf_input_inc
with const generics
- K= 4
*/
static __attribute__((unused)) uint8_t libcrux_ml_kem_utils_prf_input_inc_23(Eurydice_arr_890 *prf_inputs,
                                              uint8_t domain_separator);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_c5
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_9c_s {
  Eurydice_arr_c5 data[4U];
} Eurydice_arr_9c;

/**
A monomorphic instance of Eurydice.arr
with types int16_t
with const generics
- $272size_t
*/
typedef struct Eurydice_arr_5b_s {
  int16_t data[272U];
} Eurydice_arr_5b;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_5b
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_24_s {
  Eurydice_arr_5b data[4U];
} Eurydice_arr_24;

/**
A monomorphic instance of Eurydice.arr
with types size_t
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_cc_s {
  size_t data[4U];
} Eurydice_arr_cc;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $504size_t
*/
typedef struct Eurydice_arr_79_s {
  uint8_t data[504U];
} Eurydice_arr_79;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_79
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_7c0_s {
  Eurydice_arr_79 data[4U];
} Eurydice_arr_7c0;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $34size_t
*/
typedef struct Eurydice_arr_31_s {
  uint8_t data[34U];
} Eurydice_arr_31;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_31
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_56_s {
  Eurydice_arr_31 data[4U];
} Eurydice_arr_56;

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1600size_t
*/
typedef struct Eurydice_arr_14_s {
  uint8_t data[1600U];
} Eurydice_arr_14;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1600
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_720(
    const Eurydice_arr_14 *a);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_b50(
    const Eurydice_arr_d1 *a);

/**
This function found in impl {core::convert::AsRef<[u8]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_ref_c1
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 libcrux_ml_kem_types_as_ref_c1_d9(
    const Eurydice_arr_d1 *self);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1600
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f3(
    Eurydice_arr_14 *a, size_t r);

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 1600
*/
static __attribute__((unused)) Eurydice_arr_14 libcrux_ml_kem_utils_into_padded_array_49(
    Eurydice_borrow_slice_u8 slice);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f2(
    const Eurydice_arr_d1 *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d45(
    const Eurydice_arr_d1 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 3168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_68(
    const Eurydice_arr_a8 *a);

typedef struct Eurydice_borrow_slice_u8_x4_s {
  Eurydice_borrow_slice_u8 fst;
  Eurydice_borrow_slice_u8 snd;
  Eurydice_borrow_slice_u8 thd;
  Eurydice_borrow_slice_u8 f3;
} Eurydice_borrow_slice_u8_x4;

typedef struct Eurydice_borrow_slice_u8_x2_s {
  Eurydice_borrow_slice_u8 fst;
  Eurydice_borrow_slice_u8 snd;
} Eurydice_borrow_slice_u8_x2;

/**
 Unpack an incoming private key into it's different parts.

 We have this here in types to extract into a common core for C.
*/
/**
A monomorphic instance of libcrux_ml_kem.types.unpack_private_key
with const generics
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8_x4 libcrux_ml_kem_types_unpack_private_key_e3(
    Eurydice_borrow_slice_u8 private_key);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d44(
    Eurydice_arr_ec *a, core_ops_range_Range_87 r);

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 32
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_utils_into_padded_array_ce(
    Eurydice_borrow_slice_u8 slice);

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.default_d3
with const generics
- SIZE= 2400
*/
static __attribute__((unused)) Eurydice_arr_7d libcrux_ml_kem_types_default_d3_79(void);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f1(
    const Eurydice_arr_5f *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_to_shared_210(
    const Eurydice_arr_5f *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 2400
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d44(
    const Eurydice_arr_7d *a, core_ops_range_Range_87 r);

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_51
with const generics
- SIZE= 1184
*/
static __attribute__((unused)) Eurydice_arr_5f libcrux_ml_kem_types_from_51_3d(Eurydice_arr_5f value);

/**
 Create a new [`MlKemKeyPair`] from the secret and public key.
*/
/**
This function found in impl
{libcrux_ml_kem::types::MlKemKeyPair<PRIVATE_KEY_SIZE, PUBLIC_KEY_SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_17
with const generics
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem768_MlKem768KeyPair libcrux_ml_kem_types_from_17_bc(
    Eurydice_arr_7d sk, Eurydice_arr_5f pk);

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_b2
with const generics
- SIZE= 2400
*/
static __attribute__((unused)) Eurydice_arr_7d libcrux_ml_kem_types_from_b2_79(Eurydice_arr_7d value);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1152size_t
*/
typedef struct Eurydice_arr_0e_s {
  uint8_t data[1152U];
} Eurydice_arr_0e;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1152
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_f4(
    const Eurydice_arr_0e *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 2400
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d413(
    Eurydice_arr_7d *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 1152
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_f4(Eurydice_arr_0e *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f2(
    Eurydice_arr_5f *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d412(
    Eurydice_arr_5f *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 24
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_ed(
    const Eurydice_arr_94 *a);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $384size_t
*/
typedef struct Eurydice_arr_b2_s {
  uint8_t data[384U];
} Eurydice_arr_b2;

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 384
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d411(
    Eurydice_arr_b2 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 384
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_a9(
    const Eurydice_arr_b2 *a);

#define core_result_Ok 0
#define core_result_Err 1

typedef uint8_t core_result_Result_07_tags;

/**
A monomorphic instance of core.result.Result
with types Eurydice_arr_ec, core_array_TryFromSliceError

*/
typedef struct core_result_Result_07_s {
  core_result_Result_07_tags tag;
  union {
    Eurydice_arr_ec case_Ok;
    core_array_TryFromSliceError case_Err;
  } val;
} core_result_Result_07;

/**
This function found in impl {core::result::Result<T, E>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of core.result.unwrap_26
with types Eurydice_arr uint8_t[[$32size_t]], core_array_TryFromSliceError

*/
static __attribute__((unused)) Eurydice_arr_ec core_result_unwrap_26_39(core_result_Result_07 self);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f0(
    const Eurydice_arr_c7 *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d43(
    const Eurydice_arr_c7 *a, core_ops_range_Range_87 r);

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_19
with const generics
- SIZE= 1088
*/
static __attribute__((unused)) Eurydice_arr_2b libcrux_ml_kem_types_from_19_52(Eurydice_arr_2b value);

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_e6
with const generics
- SIZE= 1184
*/
static __attribute__((unused)) const Eurydice_arr_5f *libcrux_ml_kem_types_as_slice_e6_3d(
    const Eurydice_arr_5f *self);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_ff(
    const Eurydice_arr_5f *a);

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_a9
with const generics
- SIZE= 1088
*/
static __attribute__((unused)) const Eurydice_arr_2b *libcrux_ml_kem_types_as_slice_a9_52(
    const Eurydice_arr_2b *self);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f1(
    Eurydice_arr_2b *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 10
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_30(
    const Eurydice_arr_6d *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d42(
    const Eurydice_arr_ec *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d410(
    Eurydice_arr_2b *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 22
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_98(
    const Eurydice_arr_80 *a);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 20
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_8f(
    const Eurydice_arr_fc *a);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $320size_t
*/
typedef struct Eurydice_arr_b0_s {
  uint8_t data[320U];
} Eurydice_arr_b0;

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 320
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d49(
    Eurydice_arr_b0 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 320
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_56(
    const Eurydice_arr_b0 *a);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 128
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_78(
    const Eurydice_arr_89 *a);

/**
A monomorphic instance of Eurydice.arr
with types int16_t
with const generics
- $256size_t
*/
typedef struct Eurydice_arr_04_s {
  int16_t data[256U];
} Eurydice_arr_04;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types int16_t
with const generics
- N= 256
*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_array_to_slice_shared_99(
    const Eurydice_arr_04 *a);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_89
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_58_s {
  Eurydice_arr_89 data[3U];
} Eurydice_arr_58;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 33
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_b5(
    const Eurydice_arr_fa *a);

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 128
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_78(Eurydice_arr_89 *a);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_fa
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_800_s {
  Eurydice_arr_fa data[3U];
} Eurydice_arr_800;

/**
A monomorphic instance of libcrux_ml_kem.utils.prf_input_inc
with const generics
- K= 3
*/
static __attribute__((unused)) uint8_t libcrux_ml_kem_utils_prf_input_inc_78(Eurydice_arr_800 *prf_inputs,
                                              uint8_t domain_separator);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 33
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d48(
    Eurydice_arr_fa *a, core_ops_range_Range_87 r);

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 33
*/
static __attribute__((unused)) Eurydice_arr_fa libcrux_ml_kem_utils_into_padded_array_29(
    Eurydice_borrow_slice_u8 slice);

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 34
*/
static __attribute__((unused)) Eurydice_arr_31 libcrux_ml_kem_utils_into_padded_array_de(
    Eurydice_borrow_slice_u8 slice);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t
with const generics
- N= 272
*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_array_to_subslice_shared_e70(
    const Eurydice_arr_5b *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d41(
    const Eurydice_arr_c5 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_c5
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_2c_s {
  Eurydice_arr_c5 data[3U];
} Eurydice_arr_2c;

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_2c(Eurydice_arr_c5 *a);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_5b
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_b1_s {
  Eurydice_arr_5b data[3U];
} Eurydice_arr_b1;

/**
A monomorphic instance of Eurydice.arr
with types size_t
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_eb_s {
  size_t data[3U];
} Eurydice_arr_eb;

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t
with const generics
- N= 272
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_i16 Eurydice_array_to_subslice_mut_e7(
    Eurydice_arr_5b *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 504
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d40(
    const Eurydice_arr_79 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_79
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_7e_s {
  Eurydice_arr_79 data[3U];
} Eurydice_arr_7e;

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 504
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_48(Eurydice_arr_79 *a);

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_31
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_81_s {
  Eurydice_arr_31 data[3U];
} Eurydice_arr_81;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 34
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_e9(
    const Eurydice_arr_31 *a);

/**
A monomorphic instance of Eurydice.slice_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_slice_subslice_from_shared_6d(
    Eurydice_borrow_slice_u8 s, size_t r);

/**
A monomorphic instance of Eurydice.slice_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_slice_subslice_to_shared_72(
    Eurydice_borrow_slice_u8 s, size_t r);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $1120size_t
*/
typedef struct Eurydice_arr_af_s {
  uint8_t data[1120U];
} Eurydice_arr_af;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1120
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_81(
    const Eurydice_arr_af *a);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_06(
    const Eurydice_arr_2b *a);

/**
This function found in impl {core::convert::AsRef<[u8]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_ref_c1
with const generics
- SIZE= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 libcrux_ml_kem_types_as_ref_c1_52(
    const Eurydice_arr_2b *self);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1120
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f0(
    Eurydice_arr_af *a, size_t r);

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 1120
*/
static __attribute__((unused)) Eurydice_arr_af libcrux_ml_kem_utils_into_padded_array_66(
    Eurydice_borrow_slice_u8 slice);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_17(
    const Eurydice_arr_c7 *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f(
    Eurydice_arr_c7 *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_01(
    const Eurydice_arr_ec *a);

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 64
*/
static __attribute__((unused)) Eurydice_arr_c7 libcrux_ml_kem_utils_into_padded_array_c9(
    Eurydice_borrow_slice_u8 slice);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 2
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_82(
    const Eurydice_array_u8x2 *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f(
    const Eurydice_arr_2b *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d4(
    const Eurydice_arr_2b *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 2400
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_51(
    const Eurydice_arr_7d *a);

/**
 Unpack an incoming private key into it's different parts.

 We have this here in types to extract into a common core for C.
*/
/**
A monomorphic instance of libcrux_ml_kem.types.unpack_private_key
with const generics
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8_x4 libcrux_ml_kem_types_unpack_private_key_64(
    Eurydice_borrow_slice_u8 private_key);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$24size_t]]

*/
static __attribute__((unused)) Eurydice_arr_94 libcrux_secrets_int_public_integers_declassify_d8_40(
    Eurydice_arr_94 self);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$22size_t]]

*/
static __attribute__((unused)) Eurydice_arr_80 libcrux_secrets_int_public_integers_declassify_d8_0b(
    Eurydice_arr_80 self);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$20size_t]]

*/
static __attribute__((unused)) Eurydice_arr_fc libcrux_secrets_int_public_integers_declassify_d8_2b(
    Eurydice_arr_fc self);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$10size_t]]

*/
static __attribute__((unused)) Eurydice_arr_6d libcrux_secrets_int_public_integers_declassify_d8_37(
    Eurydice_arr_6d self);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$8size_t]]

*/
static __attribute__((unused)) Eurydice_array_u8x8 libcrux_secrets_int_public_integers_declassify_d8_52(
    Eurydice_array_u8x8 self);

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t
with const generics
- N= 16
*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_array_to_subslice_shared_e7(
    const Eurydice_arr_d6 *a, core_ops_range_Range_87 r);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$2size_t]]

*/
static __attribute__((unused)) Eurydice_array_u8x2 libcrux_secrets_int_public_integers_declassify_d8_75(
    Eurydice_array_u8x2 self);

/**
 Classify a mutable slice (identity)
 We define a separate function for this because hax has limited support for
 &mut-returning functions
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_mut_slice
with types Eurydice_dst_ref_mut uint8_t size_t

*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8
libcrux_secrets_int_public_integers_classify_mut_slice_75(
    Eurydice_mut_borrow_slice_u8 x);

/**
This function found in impl {libcrux_secrets::traits::ClassifyRef<&'a ([T])> for
&'a ([T])}
*/
/**
A monomorphic instance of libcrux_secrets.int.classify_public.classify_ref_6d
with types uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 libcrux_secrets_int_classify_public_classify_ref_6d_90(
    Eurydice_borrow_slice_u8 self);

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr int16_t[[$16size_t]]

*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_secrets_int_public_integers_declassify_d8_4b(
    Eurydice_arr_d6 self);

/**
This function found in impl {libcrux_secrets::traits::ClassifyRef<&'a ([T])> for
&'a ([T])}
*/
/**
A monomorphic instance of libcrux_secrets.int.classify_public.classify_ref_6d
with types int16_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_i16
libcrux_secrets_int_classify_public_classify_ref_6d_39(
    Eurydice_borrow_slice_i16 self);

/**
A monomorphic instance of Eurydice.slice_subslice_shared
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_slice_subslice_shared_a6(
    Eurydice_borrow_slice_i16 s, core_ops_range_Range_87 r);

/**
A monomorphic instance of core.result.Result
with types Eurydice_arr_d6, core_array_TryFromSliceError

*/
typedef struct core_result_Result_ec_s {
  core_result_Result_07_tags tag;
  union {
    Eurydice_arr_d6 case_Ok;
    core_array_TryFromSliceError case_Err;
  } val;
} core_result_Result_ec;

/**
This function found in impl {core::result::Result<T, E>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of core.result.unwrap_26
with types Eurydice_arr int16_t[[$16size_t]], core_array_TryFromSliceError

*/
static __attribute__((unused)) Eurydice_arr_d6 core_result_unwrap_26_d3(core_result_Result_ec self);

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types Eurydice_arr int16_t[[$16size_t]]

*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_secrets_int_public_integers_classify_27_4b(
    Eurydice_arr_d6 self);

/**
A monomorphic instance of Eurydice.arr
with types int16_t
with const generics
- $128size_t
*/
typedef struct Eurydice_arr_34_s {
  int16_t data[128U];
} Eurydice_arr_34;

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_17(Eurydice_arr_c7 *a);

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 48
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_9f(Eurydice_arr_65 *a);

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_01(Eurydice_arr_ec *a);

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 28
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_5e(Eurydice_arr_a2 *a);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $104size_t
*/
typedef struct Eurydice_arr_c4_s {
  uint8_t data[104U];
} Eurydice_arr_c4;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 104
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_72(
    const Eurydice_arr_c4 *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 104
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d43(
    Eurydice_arr_c4 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $144size_t
*/
typedef struct Eurydice_arr_f4_s {
  uint8_t data[144U];
} Eurydice_arr_f4;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 144
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_38(
    const Eurydice_arr_f4 *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 144
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d42(
    Eurydice_arr_f4 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_2c(
    const Eurydice_arr_c5 *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d41(
    Eurydice_arr_c5 *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 136
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_58(
    const Eurydice_arr_ff *a);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 136
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d40(
    Eurydice_arr_ff *a, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 8
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_to_shared_21(
    const Eurydice_array_u8x8 *a, size_t r);

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 8
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_6e(
    const Eurydice_array_u8x8 *a);

/**
A monomorphic instance of Eurydice.slice_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t

*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_slice_subslice_mut_c8(
    Eurydice_mut_borrow_slice_u8 s, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.arr
with types uint8_t
with const generics
- $72size_t
*/
typedef struct Eurydice_arr_ab_s {
  uint8_t data[72U];
} Eurydice_arr_ab;

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 72
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_e2(
    const Eurydice_arr_ab *a);

/**
A monomorphic instance of core.result.Result
with types Eurydice_array_u8x8, core_array_TryFromSliceError

*/
typedef struct core_result_Result_8e_s {
  core_result_Result_07_tags tag;
  union {
    Eurydice_array_u8x8 case_Ok;
    core_array_TryFromSliceError case_Err;
  } val;
} core_result_Result_8e;

/**
This function found in impl {core::result::Result<T, E>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of core.result.unwrap_26
with types Eurydice_arr uint8_t[[$8size_t]], core_array_TryFromSliceError

*/
static __attribute__((unused)) Eurydice_array_u8x8 core_result_unwrap_26_e0(core_result_Result_8e self);

/**
A monomorphic instance of Eurydice.slice_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_slice_subslice_shared_c8(
    Eurydice_borrow_slice_u8 s, core_ops_range_Range_87 r);

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 72
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d4(
    Eurydice_arr_ab *a, core_ops_range_Range_87 r);

typedef struct libcrux_ml_kem_utils_extraction_helper_Keypair768_s {
  Eurydice_arr_0e fst;
  Eurydice_arr_5f snd;
} libcrux_ml_kem_utils_extraction_helper_Keypair768;

typedef struct libcrux_ml_kem_utils_extraction_helper_Keypair1024_s {
  Eurydice_arr_df fst;
  Eurydice_arr_d1 snd;
} libcrux_ml_kem_utils_extraction_helper_Keypair1024;

#if defined(__cplusplus)
}
#endif

#define internal_libcrux_core_H_DEFINED
#endif /* internal_libcrux_core_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_sha3_portable.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef libcrux_sha3_portable_H
#define libcrux_sha3_portable_H


#if defined(__cplusplus)
extern "C" {
#endif


/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_zero_d2(void);

static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__veor5q_u64(uint64_t a, uint64_t b,
                                                uint64_t c, uint64_t d,
                                                uint64_t e);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor5_d2(uint64_t a, uint64_t b, uint64_t c,
                                            uint64_t d, uint64_t e);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 1
- RIGHT= 63
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_76(uint64_t x);

static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vrax1q_u64(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(uint64_t a,
                                                            uint64_t b);

static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vbcaxq_u64(uint64_t a, uint64_t b,
                                                uint64_t c);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_and_not_xor_d2(uint64_t a, uint64_t b,
                                                   uint64_t c);

static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__veorq_n_u64(uint64_t a, uint64_t c);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_constant_d2(uint64_t a, uint64_t c);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_d2(uint64_t a, uint64_t b);

#define LIBCRUX_SHA3_GENERIC_KECCAK_CONSTANTS_ROUNDCONSTANTS        \
  ((KRML_CLITERAL(Eurydice_arr_22){.data = {1ULL,                   \
                                            32898ULL,               \
                                            9223372036854808714ULL, \
                                            9223372039002292224ULL, \
                                            32907ULL,               \
                                            2147483649ULL,          \
                                            9223372039002292353ULL, \
                                            9223372036854808585ULL, \
                                            138ULL,                 \
                                            136ULL,                 \
                                            2147516425ULL,          \
                                            2147483658ULL,          \
                                            2147516555ULL,          \
                                            9223372036854775947ULL, \
                                            9223372036854808713ULL, \
                                            9223372036854808579ULL, \
                                            9223372036854808578ULL, \
                                            9223372036854775936ULL, \
                                            32778ULL,               \
                                            9223372039002259466ULL, \
                                            9223372039002292353ULL, \
                                            9223372036854808704ULL, \
                                            2147483649ULL,          \
                                            9223372039002292232ULL}}))

typedef struct size_t_x2_s {
  size_t fst;
  size_t snd;
} size_t_x2;

/**
A monomorphic instance of libcrux_sha3.generic_keccak.KeccakState
with types uint64_t
with const generics
- $1size_t
*/
typedef Eurydice_arr_7c libcrux_sha3_generic_keccak_KeccakState_f3;

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.new_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) Eurydice_arr_7c libcrux_sha3_generic_keccak_new_80_71(void);

/**
A monomorphic instance of libcrux_sha3.traits.get_ij
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) const uint64_t *libcrux_sha3_traits_get_ij_71(const Eurydice_arr_7c *arr,
                                              size_t i, size_t j);

/**
A monomorphic instance of libcrux_sha3.traits.set_ij
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_traits_set_ij_71(Eurydice_arr_7c *arr, size_t i, size_t j,
                                   uint64_t value);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_c6(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 72
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_dc(Eurydice_arr_7c *state,
                                             Eurydice_borrow_slice_u8 blocks,
                                             size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 72
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_dc(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len);

/**
This function found in impl {core::ops::index::Index<(usize, usize), T> for
libcrux_sha3::generic_keccak::KeccakState<T, N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.index_c2
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) const uint64_t *libcrux_sha3_generic_keccak_index_c2_71(
    const Eurydice_arr_7c *self, size_t_x2 index);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.theta_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) Eurydice_arr_84 libcrux_sha3_generic_keccak_theta_80_71(Eurydice_arr_7c *self);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.set_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_set_80_71(Eurydice_arr_7c *self, size_t i,
                                           size_t j, uint64_t v);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 36
- RIGHT= 28
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_02(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 36
- RIGHT= 28
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_02(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 36
- RIGHT= 28
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_02(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 3
- RIGHT= 61
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_ac(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 3
- RIGHT= 61
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_ac(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 3
- RIGHT= 61
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_ac(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 41
- RIGHT= 23
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_020(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 41
- RIGHT= 23
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_020(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 41
- RIGHT= 23
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_020(uint64_t a,
                                                          uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 18
- RIGHT= 46
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_a9(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 18
- RIGHT= 46
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_a9(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 18
- RIGHT= 46
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_a9(uint64_t a,
                                                         uint64_t b);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_0_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_rho_0_80_71(Eurydice_arr_7c *self,
                                             Eurydice_arr_84 t);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 1
- RIGHT= 63
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_76(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 1
- RIGHT= 63
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_76(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 44
- RIGHT= 20
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_58(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 44
- RIGHT= 20
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_58(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 44
- RIGHT= 20
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_58(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 10
- RIGHT= 54
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_e0(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 10
- RIGHT= 54
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_e0(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 10
- RIGHT= 54
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_e0(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 45
- RIGHT= 19
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_63(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 45
- RIGHT= 19
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_63(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 45
- RIGHT= 19
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_63(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 2
- RIGHT= 62
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_6a(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 2
- RIGHT= 62
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_6a(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 2
- RIGHT= 62
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_6a(uint64_t a,
                                                         uint64_t b);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_1_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_rho_1_80_71(Eurydice_arr_7c *self,
                                             Eurydice_arr_84 t);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 62
- RIGHT= 2
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_ab(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 62
- RIGHT= 2
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_ab(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 62
- RIGHT= 2
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_ab(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 6
- RIGHT= 58
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_5b(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 6
- RIGHT= 58
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_5b(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 6
- RIGHT= 58
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_5b(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 43
- RIGHT= 21
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_6f(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 43
- RIGHT= 21
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_6f(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 43
- RIGHT= 21
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_6f(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 15
- RIGHT= 49
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_62(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 15
- RIGHT= 49
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_62(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 15
- RIGHT= 49
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_62(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 61
- RIGHT= 3
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_23(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 61
- RIGHT= 3
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_23(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 61
- RIGHT= 3
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_23(uint64_t a,
                                                         uint64_t b);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_2_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_rho_2_80_71(Eurydice_arr_7c *self,
                                             Eurydice_arr_84 t);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 28
- RIGHT= 36
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_37(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 28
- RIGHT= 36
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_37(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 28
- RIGHT= 36
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_37(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 55
- RIGHT= 9
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_bb(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 55
- RIGHT= 9
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_bb(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 55
- RIGHT= 9
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_bb(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 25
- RIGHT= 39
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_b9(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 25
- RIGHT= 39
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_b9(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 25
- RIGHT= 39
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_b9(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 21
- RIGHT= 43
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_54(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 21
- RIGHT= 43
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_54(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 21
- RIGHT= 43
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_54(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 56
- RIGHT= 8
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_4c(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 56
- RIGHT= 8
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_4c(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 56
- RIGHT= 8
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_4c(uint64_t a,
                                                         uint64_t b);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_3_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_rho_3_80_71(Eurydice_arr_7c *self,
                                             Eurydice_arr_84 t);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 27
- RIGHT= 37
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_ce(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 27
- RIGHT= 37
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_ce(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 27
- RIGHT= 37
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_ce(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 20
- RIGHT= 44
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_77(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 20
- RIGHT= 44
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_77(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 20
- RIGHT= 44
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_77(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 39
- RIGHT= 25
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_25(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 39
- RIGHT= 25
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_25(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 39
- RIGHT= 25
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_25(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 8
- RIGHT= 56
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_af(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 8
- RIGHT= 56
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_af(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 8
- RIGHT= 56
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_af(uint64_t a,
                                                         uint64_t b);

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 14
- RIGHT= 50
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_rotate_left_fd(uint64_t x);

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 14
- RIGHT= 50
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable__vxarq_u64_fd(uint64_t a, uint64_t b);

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 14
- RIGHT= 50
*/
static __attribute__((unused)) uint64_t libcrux_sha3_simd_portable_xor_and_rotate_d2_fd(uint64_t a,
                                                         uint64_t b);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_4_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_rho_4_80_71(Eurydice_arr_7c *self,
                                             Eurydice_arr_84 t);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_rho_80_71(Eurydice_arr_7c *self,
                                           Eurydice_arr_84 t);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_0_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_pi_0_80_71(Eurydice_arr_7c *self,
                                            Eurydice_arr_7c old);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_1_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_pi_1_80_71(Eurydice_arr_7c *self,
                                            Eurydice_arr_7c old);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_2_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_pi_2_80_71(Eurydice_arr_7c *self,
                                            Eurydice_arr_7c old);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_3_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_pi_3_80_71(Eurydice_arr_7c *self,
                                            Eurydice_arr_7c old);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_4_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_pi_4_80_71(Eurydice_arr_7c *self,
                                            Eurydice_arr_7c old);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_pi_80_71(Eurydice_arr_7c *self);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.chi_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_chi_80_71(Eurydice_arr_7c *self);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.iota_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_iota_80_71(Eurydice_arr_7c *self, size_t i);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.keccakf1600_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_keccakf1600_80_71(Eurydice_arr_7c *self);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 72
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_final_80_bd(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len);

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_store_block_c6(const Eurydice_arr_7c *s,
                                               Eurydice_mut_borrow_slice_u8 out,
                                               size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_c6(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_c6(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_block_80_e9(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 72
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_keccak1_dc(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output);

/**
 A portable SHA3 512 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_sha512(Eurydice_mut_borrow_slice_u8 digest,
                                  Eurydice_borrow_slice_u8 data);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_b2(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 136
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_22(Eurydice_arr_7c *state,
                                             Eurydice_borrow_slice_u8 blocks,
                                             size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 136
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_22(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 136
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_final_80_bd0(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len);

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_store_block_b2(const Eurydice_arr_7c *s,
                                               Eurydice_mut_borrow_slice_u8 out,
                                               size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_b2(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_b2(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_block_80_e90(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 136
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_keccak1_22(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output);

/**
 A portable SHA3 256 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_sha256(Eurydice_mut_borrow_slice_u8 digest,
                                  Eurydice_borrow_slice_u8 data);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 136
- DELIMITER= 31
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_220(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 136
- DELIMITER= 31
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_220(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 136
- DELIM= 31
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_final_80_bd1(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 136
- DELIM= 31
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_keccak1_220(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output);

/**
 A portable SHAKE256 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_shake256(Eurydice_mut_borrow_slice_u8 digest,
                                    Eurydice_borrow_slice_u8 data);

typedef libcrux_sha3_generic_keccak_KeccakState_f3
    libcrux_sha3_portable_KeccakState;

/**
 Create a new SHAKE-128 state object.
*/
static __attribute__((unused)) Eurydice_arr_7c libcrux_sha3_portable_incremental_shake128_init(void);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_60(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 168
- DELIMITER= 31
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_37(Eurydice_arr_7c *state,
                                             Eurydice_borrow_slice_u8 blocks,
                                             size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 168
- DELIMITER= 31
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_37(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 168
- DELIM= 31
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_final_80_bd2(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len);

/**
 Absorb
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_absorb_final(
    Eurydice_arr_7c *s, Eurydice_borrow_slice_u8 data0);

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_store_block_60(const Eurydice_arr_7c *s,
                                               Eurydice_mut_borrow_slice_u8 out,
                                               size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_60(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_first_three_blocks_b4 with const
generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_squeeze_first_three_blocks_b4_60(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out);

/**
 Squeeze three blocks
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_squeeze_first_three_blocks(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out0);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_next_block_b4 with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_squeeze_next_block_b4_60(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out, size_t start);

/**
 Squeeze another block
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_squeeze_next_block(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out0);

#define libcrux_sha3_Algorithm_Sha224 1
#define libcrux_sha3_Algorithm_Sha256 2
#define libcrux_sha3_Algorithm_Sha384 3
#define libcrux_sha3_Algorithm_Sha512 4

typedef uint8_t libcrux_sha3_Algorithm;

#define LIBCRUX_SHA3_SHA3_224_DIGEST_SIZE ((size_t)28U)

#define LIBCRUX_SHA3_SHA3_256_DIGEST_SIZE ((size_t)32U)

#define LIBCRUX_SHA3_SHA3_384_DIGEST_SIZE ((size_t)48U)

#define LIBCRUX_SHA3_SHA3_512_DIGEST_SIZE ((size_t)64U)

/**
 Returns the output size of a digest.
*/
static __attribute__((unused)) size_t libcrux_sha3_digest_size(libcrux_sha3_Algorithm mode);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_9e(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 144
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_3a(Eurydice_arr_7c *state,
                                             Eurydice_borrow_slice_u8 blocks,
                                             size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 144
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_3a(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 144
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_final_80_bd3(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len);

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_store_block_9e(const Eurydice_arr_7c *s,
                                               Eurydice_mut_borrow_slice_u8 out,
                                               size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_9e(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_9e(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_block_80_e91(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 144
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_keccak1_3a(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output);

/**
 A portable SHA3 224 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_sha224(Eurydice_mut_borrow_slice_u8 digest,
                                  Eurydice_borrow_slice_u8 data);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_53(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start);

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 104
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_dc0(Eurydice_arr_7c *state,
                                              Eurydice_borrow_slice_u8 blocks,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 104
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_dc0(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 104
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_final_80_bd4(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len);

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_store_block_53(const Eurydice_arr_7c *s,
                                               Eurydice_mut_borrow_slice_u8 out,
                                               size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_53(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_53(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_block_80_e92(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 104
- DELIM= 6
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_keccak1_dc0(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output);

/**
 A portable SHA3 384 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_sha384(Eurydice_mut_borrow_slice_u8 digest,
                                  Eurydice_borrow_slice_u8 data);

/**
 SHA3 224

 Preconditions:
 - `digest.len() == 28`
*/
static __attribute__((unused)) void libcrux_sha3_sha224_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload);

/**
 SHA3 224
*/
static __attribute__((unused)) Eurydice_arr_a2 libcrux_sha3_sha224(Eurydice_borrow_slice_u8 data);

/**
 SHA3 256
*/
static __attribute__((unused)) void libcrux_sha3_sha256_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload);

/**
 SHA3 256
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_sha3_sha256(Eurydice_borrow_slice_u8 data);

/**
 SHA3 384
*/
static __attribute__((unused)) void libcrux_sha3_sha384_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload);

/**
 SHA3 384
*/
static __attribute__((unused)) Eurydice_arr_65 libcrux_sha3_sha384(Eurydice_borrow_slice_u8 data);

/**
 SHA3 512
*/
static __attribute__((unused)) void libcrux_sha3_sha512_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload);

/**
 SHA3 512
*/
static __attribute__((unused)) Eurydice_arr_c7 libcrux_sha3_sha512(Eurydice_borrow_slice_u8 data);

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_60(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_absorb_block_80_e93(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 168
- DELIM= 31
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_keccak1_37(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output);

/**
 A portable SHAKE128 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_shake128(Eurydice_mut_borrow_slice_u8 digest,
                                    Eurydice_borrow_slice_u8 data);

/**
 SHAKE 128

 Writes `out.len()` bytes.
*/
static __attribute__((unused)) void libcrux_sha3_shake128_ema(Eurydice_mut_borrow_slice_u8 out,
                               Eurydice_borrow_slice_u8 data);

/**
 SHAKE 256

 Writes `out.len()` bytes.
*/
static __attribute__((unused)) void libcrux_sha3_shake256_ema(Eurydice_mut_borrow_slice_u8 out,
                               Eurydice_borrow_slice_u8 data);

/**
A monomorphic instance of libcrux_sha3.generic_keccak.xof.KeccakXofState
with types uint64_t
with const generics
- $1size_t
- $168size_t
*/
typedef struct libcrux_sha3_generic_keccak_xof_KeccakXofState_55_s {
  Eurydice_arr_7c inner;
  Eurydice_arr_88 buf;
  size_t buf_len;
  bool sponge;
} libcrux_sha3_generic_keccak_xof_KeccakXofState_55;

typedef libcrux_sha3_generic_keccak_xof_KeccakXofState_55
    libcrux_sha3_portable_incremental_Shake128Xof;

/**
A monomorphic instance of libcrux_sha3.generic_keccak.xof.KeccakXofState
with types uint64_t
with const generics
- $1size_t
- $136size_t
*/
typedef struct libcrux_sha3_generic_keccak_xof_KeccakXofState_8d_s {
  Eurydice_arr_7c inner;
  Eurydice_arr_0b buf;
  size_t buf_len;
  bool sponge;
} libcrux_sha3_generic_keccak_xof_KeccakXofState_8d;

typedef libcrux_sha3_generic_keccak_xof_KeccakXofState_8d
    libcrux_sha3_portable_incremental_Shake256Xof;

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_first_five_blocks_b4 with const
generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_squeeze_first_five_blocks_b4_60(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out);

/**
 Squeeze five blocks
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_squeeze_first_five_blocks(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out0);

/**
 Absorb some data for SHAKE-256 for the last time
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake256_absorb_final(
    Eurydice_arr_7c *s, Eurydice_borrow_slice_u8 data);

/**
 Create a new SHAKE-256 state object.
*/
static __attribute__((unused)) Eurydice_arr_7c libcrux_sha3_portable_incremental_shake256_init(void);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_first_block_b4 with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_squeeze_first_block_b4_b2(
    const Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out);

/**
 Squeeze the first SHAKE-256 block
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake256_squeeze_first_block(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out);

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_next_block_b4 with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_portable_squeeze_next_block_b4_b2(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out, size_t start);

/**
 Squeeze the next SHAKE-256 block
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake256_squeeze_next_block(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out);

/**
A monomorphic instance of Eurydice.arr
with types libcrux_sha3_portable_KeccakState
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_4a_s {
  Eurydice_arr_7c data[4U];
} Eurydice_arr_4a;

/**
A monomorphic instance of Eurydice.arr
with types libcrux_sha3_portable_KeccakState
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_1b_s {
  Eurydice_arr_7c data[3U];
} Eurydice_arr_1b;

#if defined(__cplusplus)
}
#endif

#define libcrux_sha3_portable_H_DEFINED
#endif /* libcrux_sha3_portable_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_mlkem_portable.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef libcrux_mlkem_portable_H
#define libcrux_mlkem_portable_H


#if defined(__cplusplus)
extern "C" {
#endif


static __attribute__((unused)) Eurydice_arr_c7 libcrux_ml_kem_hash_functions_portable_G(
    Eurydice_borrow_slice_u8 input);

static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_hash_functions_portable_H(
    Eurydice_borrow_slice_u8 input);

#define LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR ((size_t)16U)

#define LIBCRUX_ML_KEM_VECTOR_TRAITS_MONTGOMERY_R_SQUARED_MOD_FIELD_MODULUS \
  (1353)

typedef Eurydice_arr_d6
    libcrux_ml_kem_vector_portable_vector_type_PortableVector;

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_vector_type_zero(void);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ZERO_b8(void);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_vector_type_from_i16_array(
    Eurydice_borrow_slice_i16 array);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_from_i16_array_b8(
    Eurydice_borrow_slice_i16 array);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_vector_type_to_i16_array(
    Eurydice_arr_d6 x);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_to_i16_array_b8(
    Eurydice_arr_d6 x);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_vector_type_from_bytes(
    Eurydice_borrow_slice_u8 array);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_from_bytes_b8(
    Eurydice_borrow_slice_u8 array);

static __attribute__((unused)) void libcrux_ml_kem_vector_portable_vector_type_to_bytes(
    Eurydice_arr_d6 x, Eurydice_mut_borrow_slice_u8 bytes);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) void libcrux_ml_kem_vector_portable_to_bytes_b8(
    Eurydice_arr_d6 x, Eurydice_mut_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_add(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_add_b8(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_sub(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_sub_b8(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_multiply_by_constant(
    Eurydice_arr_d6 vec, int16_t c);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_multiply_by_constant_b8(
    Eurydice_arr_d6 vec, int16_t c);

/**
 Note: This function is not secret independent
 Only use with public values.
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_cond_subtract_3329(
    Eurydice_arr_d6 vec);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_cond_subtract_3329_b8(
    Eurydice_arr_d6 v);

#define LIBCRUX_ML_KEM_VECTOR_PORTABLE_ARITHMETIC_BARRETT_MULTIPLIER (20159)

#define LIBCRUX_ML_KEM_VECTOR_TRAITS_BARRETT_SHIFT (26)

#define LIBCRUX_ML_KEM_VECTOR_TRAITS_BARRETT_R \
  (                                            \
      (int32_t)((uint32_t)1                    \
                << (uint32_t)LIBCRUX_ML_KEM_VECTOR_TRAITS_BARRETT_SHIFT))

#define LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS (3329)

/**
 Signed Barrett Reduction

 Given an input `value`, `barrett_reduce` outputs a representative `result`
 such that:

 - result ≡ value (mod FIELD_MODULUS)
 - the absolute value of `result` is bound as follows:

 `|result| ≤ FIELD_MODULUS / 2 · (|value|/BARRETT_R + 1)

 Note: The input bound is 28296 to prevent overflow in the multiplication of
 quotient by FIELD_MODULUS

*/
static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce_element(
    int16_t value);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce(
    Eurydice_arr_d6 vec);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_barrett_reduce_b8(
    Eurydice_arr_d6 vector);

#define LIBCRUX_ML_KEM_VECTOR_TRAITS_INVERSE_OF_MODULUS_MOD_MONTGOMERY_R \
  (62209U)

#define LIBCRUX_ML_KEM_VECTOR_PORTABLE_ARITHMETIC_MONTGOMERY_SHIFT (16U)

/**
 Signed Montgomery Reduction

 Given an input `value`, `montgomery_reduce` outputs a representative `o`
 such that:

 - o ≡ value · MONTGOMERY_R^(-1) (mod FIELD_MODULUS)
 - the absolute value of `o` is bound as follows:

 `|result| ≤ ceil(|value| / MONTGOMERY_R) + 1665

 In particular, if `|value| ≤ FIELD_MODULUS-1 * FIELD_MODULUS-1`, then `|o| <=
 FIELD_MODULUS-1`. And, if `|value| ≤ pow2 16 * FIELD_MODULUS-1`, then `|o| <=
 FIELD_MODULUS + 1664

*/
static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_arithmetic_montgomery_reduce_element(
    int32_t value);

/**
 If `fe` is some field element 'x' of the Kyber field and `fer` is congruent to
 `y · MONTGOMERY_R`, this procedure outputs a value that is congruent to
 `x · y`, as follows:

    `fe · fer ≡ x · y · MONTGOMERY_R (mod FIELD_MODULUS)`

 `montgomery_reduce` takes the value `x · y · MONTGOMERY_R` and outputs a
 representative `x · y · MONTGOMERY_R * MONTGOMERY_R^{-1} ≡ x · y (mod
 FIELD_MODULUS)`.
*/
static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_fe_by_fer(
    int16_t fe, int16_t fer);

static __attribute__((unused)) Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_by_constant(
    Eurydice_arr_d6 vec, int16_t c);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6
libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
    Eurydice_arr_d6 vector, int16_t constant);

static __attribute__((unused)) Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_bitwise_and_with_constant(
    Eurydice_arr_d6 vec, int16_t c);

static __attribute__((unused)) Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_to_unsigned_representative(
    Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_to_unsigned_representative_b8(
    Eurydice_arr_d6 a);

/**
 The `compress_*` functions implement the `Compress` function specified in the
 NIST FIPS 203 standard (Page 18, Expression 4.5), which is defined as:

 ```plaintext
 Compress_d: ℤq -> ℤ_{2ᵈ}
 Compress_d(x) = ⌈(2ᵈ/q)·x⌋
 ```

 Since `⌈x⌋ = ⌊x + 1/2⌋` we have:

 ```plaintext
 Compress_d(x) = ⌊(2ᵈ/q)·x + 1/2⌋
               = ⌊(2^{d+1}·x + q) / 2q⌋
 ```

 For further information about the function implementations, consult the
 `implementation_notes.pdf` document in this directory.

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
static __attribute__((unused)) uint8_t libcrux_ml_kem_vector_portable_compress_compress_message_coefficient(
    uint16_t fe);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_compress_compress_1(
    Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_compress_1_b8(Eurydice_arr_d6 a);

static __attribute__((unused)) uint32_t libcrux_ml_kem_vector_portable_arithmetic_get_n_least_significant_bits(
    uint8_t n, uint32_t value);

static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_compress_compress_ciphertext_coefficient(
    uint8_t coefficient_bits, uint16_t fe);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_compress_decompress_1(
    Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_decompress_1_b8(
    Eurydice_arr_d6 a);

static __attribute__((unused)) void libcrux_ml_kem_vector_portable_ntt_ntt_step(Eurydice_arr_d6 *vec,
                                                 int16_t zeta, size_t i,
                                                 size_t j);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_ntt_layer_1_step(
    Eurydice_arr_d6 vec, int16_t zeta0, int16_t zeta1, int16_t zeta2,
    int16_t zeta3);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_layer_1_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1, int16_t zeta2,
    int16_t zeta3);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_ntt_layer_2_step(
    Eurydice_arr_d6 vec, int16_t zeta0, int16_t zeta1);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_layer_2_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_ntt_layer_3_step(
    Eurydice_arr_d6 vec, int16_t zeta);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_layer_3_step_b8(
    Eurydice_arr_d6 a, int16_t zeta);

static __attribute__((unused)) void libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(Eurydice_arr_d6 *vec,
                                                     int16_t zeta, size_t i,
                                                     size_t j);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_1_step(
    Eurydice_arr_d6 vec, int16_t zeta0, int16_t zeta1, int16_t zeta2,
    int16_t zeta3);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_inv_ntt_layer_1_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1, int16_t zeta2,
    int16_t zeta3);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_2_step(
    Eurydice_arr_d6 vec, int16_t zeta0, int16_t zeta1);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_inv_ntt_layer_2_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_3_step(
    Eurydice_arr_d6 vec, int16_t zeta);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_inv_ntt_layer_3_step_b8(
    Eurydice_arr_d6 a, int16_t zeta);

/**
 Compute the product of two Kyber binomials with respect to the
 modulus `X² - zeta`.

 This function almost implements <strong>Algorithm 11</strong> of the
 NIST FIPS 203 standard, which is reproduced below:

 ```plaintext
 Input:  a₀, a₁, b₀, b₁ ∈ ℤq.
 Input: γ ∈ ℤq.
 Output: c₀, c₁ ∈ ℤq.

 c₀ ← a₀·b₀ + a₁·b₁·γ
 c₁ ← a₀·b₁ + a₁·b₀
 return c₀, c₁
 ```
 We say "almost" because the coefficients output by this function are in
 the Montgomery domain (unlike in the specification).

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
static __attribute__((unused)) void libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
    const Eurydice_arr_d6 *a, const Eurydice_arr_d6 *b, int16_t zeta, size_t i,
    Eurydice_arr_d6 *out);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_ntt_multiply(
    const Eurydice_arr_d6 *lhs, const Eurydice_arr_d6 *rhs, int16_t zeta0,
    int16_t zeta1, int16_t zeta2, int16_t zeta3);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_multiply_b8(
    const Eurydice_arr_d6 *lhs, const Eurydice_arr_d6 *rhs, int16_t zeta0,
    int16_t zeta1, int16_t zeta2, int16_t zeta3);

static __attribute__((unused)) Eurydice_array_u8x2 libcrux_ml_kem_vector_portable_serialize_serialize_1(
    Eurydice_arr_d6 v);

static __attribute__((unused)) Eurydice_array_u8x2 libcrux_ml_kem_vector_portable_serialize_1(
    Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_array_u8x2 libcrux_ml_kem_vector_portable_serialize_1_b8(
    Eurydice_arr_d6 a);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_serialize_deserialize_1(
    Eurydice_borrow_slice_u8 v);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_1(
    Eurydice_borrow_slice_u8 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_1_b8(
    Eurydice_borrow_slice_u8 a);

typedef struct uint8_t_x4_s {
  uint8_t fst;
  uint8_t snd;
  uint8_t thd;
  uint8_t f3;
} uint8_t_x4;

static __attribute__((unused)) uint8_t_x4 libcrux_ml_kem_vector_portable_serialize_serialize_4_int(
    Eurydice_borrow_slice_i16 v);

static __attribute__((unused)) Eurydice_array_u8x8 libcrux_ml_kem_vector_portable_serialize_serialize_4(
    Eurydice_arr_d6 v);

static __attribute__((unused)) Eurydice_array_u8x8 libcrux_ml_kem_vector_portable_serialize_4(
    Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_array_u8x8 libcrux_ml_kem_vector_portable_serialize_4_b8(
    Eurydice_arr_d6 a);

typedef struct int16_t_x8_s {
  int16_t fst;
  int16_t snd;
  int16_t thd;
  int16_t f3;
  int16_t f4;
  int16_t f5;
  int16_t f6;
  int16_t f7;
} int16_t_x8;

static __attribute__((unused)) int16_t_x8 libcrux_ml_kem_vector_portable_serialize_deserialize_4_int(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_serialize_deserialize_4(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_4(
    Eurydice_borrow_slice_u8 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_4_b8(
    Eurydice_borrow_slice_u8 a);

typedef struct uint8_t_x5_s {
  uint8_t fst;
  uint8_t snd;
  uint8_t thd;
  uint8_t f3;
  uint8_t f4;
} uint8_t_x5;

static __attribute__((unused)) uint8_t_x5 libcrux_ml_kem_vector_portable_serialize_serialize_5_int(
    Eurydice_borrow_slice_i16 v);

static __attribute__((unused)) Eurydice_arr_6d libcrux_ml_kem_vector_portable_serialize_serialize_5(
    Eurydice_arr_d6 v);

static __attribute__((unused)) Eurydice_arr_6d libcrux_ml_kem_vector_portable_serialize_5(Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_6d libcrux_ml_kem_vector_portable_serialize_5_b8(
    Eurydice_arr_d6 a);

static __attribute__((unused)) int16_t_x8 libcrux_ml_kem_vector_portable_serialize_deserialize_5_int(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_serialize_deserialize_5(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_5(
    Eurydice_borrow_slice_u8 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_5_b8(
    Eurydice_borrow_slice_u8 a);

static __attribute__((unused)) uint8_t_x5 libcrux_ml_kem_vector_portable_serialize_serialize_10_int(
    Eurydice_borrow_slice_i16 v);

static __attribute__((unused)) Eurydice_arr_fc libcrux_ml_kem_vector_portable_serialize_serialize_10(
    Eurydice_arr_d6 v);

static __attribute__((unused)) Eurydice_arr_fc libcrux_ml_kem_vector_portable_serialize_10(Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_fc libcrux_ml_kem_vector_portable_serialize_10_b8(
    Eurydice_arr_d6 a);

static __attribute__((unused)) int16_t_x8 libcrux_ml_kem_vector_portable_serialize_deserialize_10_int(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_serialize_deserialize_10(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_10(
    Eurydice_borrow_slice_u8 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_10_b8(
    Eurydice_borrow_slice_u8 a);

typedef struct uint8_t_x11_s {
  uint8_t fst;
  uint8_t snd;
  uint8_t thd;
  uint8_t f3;
  uint8_t f4;
  uint8_t f5;
  uint8_t f6;
  uint8_t f7;
  uint8_t f8;
  uint8_t f9;
  uint8_t f10;
} uint8_t_x11;

static __attribute__((unused)) uint8_t_x11 libcrux_ml_kem_vector_portable_serialize_serialize_11_int(
    Eurydice_borrow_slice_i16 v);

static __attribute__((unused)) Eurydice_arr_80 libcrux_ml_kem_vector_portable_serialize_serialize_11(
    Eurydice_arr_d6 v);

static __attribute__((unused)) Eurydice_arr_80 libcrux_ml_kem_vector_portable_serialize_11(Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_80 libcrux_ml_kem_vector_portable_serialize_11_b8(
    Eurydice_arr_d6 a);

static __attribute__((unused)) int16_t_x8 libcrux_ml_kem_vector_portable_serialize_deserialize_11_int(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_serialize_deserialize_11(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_11(
    Eurydice_borrow_slice_u8 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_11_b8(
    Eurydice_borrow_slice_u8 a);

typedef struct uint8_t_x3_s {
  uint8_t fst;
  uint8_t snd;
  uint8_t thd;
} uint8_t_x3;

static __attribute__((unused)) uint8_t_x3 libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
    Eurydice_borrow_slice_i16 v);

static __attribute__((unused)) Eurydice_arr_94 libcrux_ml_kem_vector_portable_serialize_serialize_12(
    Eurydice_arr_d6 v);

static __attribute__((unused)) Eurydice_arr_94 libcrux_ml_kem_vector_portable_serialize_12(Eurydice_arr_d6 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_94 libcrux_ml_kem_vector_portable_serialize_12_b8(
    Eurydice_arr_d6 a);

typedef struct int16_t_x2_s {
  int16_t fst;
  int16_t snd;
} int16_t_x2;

static __attribute__((unused)) int16_t_x2 libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_serialize_deserialize_12(
    Eurydice_borrow_slice_u8 bytes);

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_12(
    Eurydice_borrow_slice_u8 a);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_12_b8(
    Eurydice_borrow_slice_u8 a);

static __attribute__((unused)) size_t libcrux_ml_kem_vector_portable_sampling_rej_sample(
    Eurydice_borrow_slice_u8 a, Eurydice_mut_borrow_slice_i16 result);

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) size_t libcrux_ml_kem_vector_portable_rej_sample_b8(
    Eurydice_borrow_slice_u8 a, Eurydice_mut_borrow_slice_i16 out);

typedef int16_t
    libcrux_ml_kem_vector_portable_arithmetic_FieldElementTimesMontgomeryR;

typedef int16_t
    libcrux_ml_kem_vector_portable_arithmetic_MontgomeryFieldElement;

typedef int16_t libcrux_ml_kem_vector_portable_vector_type_FieldElement;

/**
This function found in impl {core::clone::Clone for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_vector_type_clone_9c(
    const Eurydice_arr_d6 *self);

#if defined(__cplusplus)
}
#endif

#define libcrux_mlkem_portable_H_DEFINED
#endif /* libcrux_mlkem_portable_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/internal/libcrux_mlkem_portable.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef internal_libcrux_mlkem_portable_H
#define internal_libcrux_mlkem_portable_H


#if defined(__cplusplus)
extern "C" {
#endif


/**
A monomorphic instance of Eurydice.arr
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- $16size_t
*/
typedef struct Eurydice_arr_9e_s {
  Eurydice_arr_d6 data[16U];
} Eurydice_arr_9e;

/**
A monomorphic instance of Eurydice.arr
with types libcrux_ml_kem_polynomial_PolynomialRingElement_1d
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_d20_s {
  Eurydice_arr_9e data[4U];
} Eurydice_arr_d20;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_d20
with const generics
- $4size_t
*/
typedef struct Eurydice_arr_60_s {
  Eurydice_arr_d20 data[4U];
} Eurydice_arr_60;

/**
A monomorphic instance of
libcrux_ml_kem.ind_cpa.unpacked.IndCpaPublicKeyUnpacked with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- $4size_t
*/
typedef struct libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94_s {
  Eurydice_arr_d20 t_as_ntt;
  Eurydice_arr_ec seed_for_A;
  Eurydice_arr_60 A;
} libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94;

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.MlKemPublicKeyUnpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- $4size_t
*/
typedef struct libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94_s {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94 ind_cpa_public_key;
  Eurydice_arr_ec public_key_hash;
} libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94;

/**
 Generate an unpacked key from a serialized key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.unpack_public_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- T_AS_NTT_ENCODED_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_unpack_public_key_29(
    const Eurydice_arr_d1 *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
        *unpacked_public_key);

/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.MlKemPrivateKeyUnpacked with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- $4size_t
*/
typedef struct libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_94_s {
  Eurydice_arr_d20 ind_cpa_private_key;
  Eurydice_arr_ec implicit_rejection_value;
} libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_94;

typedef struct
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked_s {
  libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_94 private_key;
  libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 public_key;
} libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked;

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.serialized_mut_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_1c(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *self,
    Eurydice_arr_d1 *serialized);

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_mut_11_1c(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self,
    Eurydice_arr_d1 *serialized);

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_d1 libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_11_1c(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self);

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_2e(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self,
    Eurydice_arr_a8 *serialized);

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_a8 libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_11_2e(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self);

/**
 Take a serialized private key and generate an unpacked key pair from it.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.keys_from_private_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_keys_from_private_key_38(
    const Eurydice_arr_a8 *private_key,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair);

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_30
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
libcrux_ml_kem_ind_cca_unpacked_default_30_ee(void);

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_7b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
libcrux_ml_kem_ind_cca_unpacked_default_7b_ee(void);

/**
 Generate Unpacked Keys
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_generate_keypair_b80(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked *out);

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- VECTOR_U_BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_ind_cca_unpacked_encapsulate_a70(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    const Eurydice_arr_ec *randomness);

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1600
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_unpacked_decapsulate_0c0(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    const Eurydice_arr_d1 *ciphertext);

/**
 Validate an ML-KEM public key.

 This implements the Modulus check in 7.2 2.
 Note that the size check in 7.2 1 is covered by the `PUBLIC_KEY_SIZE` in the
 `public_key` type.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_public_key_1c(
    const Eurydice_arr_d1 *public_key);

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key_only
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_only_4c(
    const Eurydice_arr_a8 *private_key);

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
 Note that the size checks in 7.2 1 and 2 are covered by the `SECRET_KEY_SIZE`
 and `CIPHERTEXT_SIZE` in the `private_key` and `ciphertext` types.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CIPHERTEXT_SIZE= 1568
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_79(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *_ciphertext);

/**
 Packed API

 Generate a key pair.

 Depending on the `Vector` and `Hasher` used, this requires different hardware
 features
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair
libcrux_ml_kem_ind_cca_generate_keypair_b80(const Eurydice_arr_c7 *randomness);

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_ind_cca_encapsulate_990(
    const Eurydice_arr_d1 *public_key, const Eurydice_arr_ec *randomness);

/**
 This code verifies on some machines, runs out of memory on others
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1600
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_decapsulate_fd0(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext);

/**
A monomorphic instance of Eurydice.arr
with types libcrux_ml_kem_polynomial_PolynomialRingElement_1d
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_bb_s {
  Eurydice_arr_9e data[3U];
} Eurydice_arr_bb;

/**
A monomorphic instance of Eurydice.arr
with types Eurydice_arr_bb
with const generics
- $3size_t
*/
typedef struct Eurydice_arr_1d_s {
  Eurydice_arr_bb data[3U];
} Eurydice_arr_1d;

/**
A monomorphic instance of
libcrux_ml_kem.ind_cpa.unpacked.IndCpaPublicKeyUnpacked with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- $3size_t
*/
typedef struct libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51_s {
  Eurydice_arr_bb t_as_ntt;
  Eurydice_arr_ec seed_for_A;
  Eurydice_arr_1d A;
} libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51;

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.MlKemPublicKeyUnpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- $3size_t
*/
typedef struct libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51_s {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 ind_cpa_public_key;
  Eurydice_arr_ec public_key_hash;
} libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51;

/**
 Generate an unpacked key from a serialized key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.unpack_public_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- T_AS_NTT_ENCODED_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_unpack_public_key_22(
    const Eurydice_arr_5f *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51
        *unpacked_public_key);

/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.MlKemPrivateKeyUnpacked with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- $3size_t
*/
typedef struct libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_51_s {
  Eurydice_arr_bb ind_cpa_private_key;
  Eurydice_arr_ec implicit_rejection_value;
} libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_51;

typedef struct
    libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked_s {
  libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_51 private_key;
  libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 public_key;
} libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked;

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.public_key_11
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *
libcrux_ml_kem_ind_cca_unpacked_public_key_11_68(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self);

/**
This function found in impl {core::clone::Clone for
libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@2]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.clone_d7
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51
libcrux_ml_kem_ind_cca_unpacked_clone_d7_68(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *self);

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.serialized_mut_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_b6(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *self,
    Eurydice_arr_5f *serialized);

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_mut_11_b6(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self,
    Eurydice_arr_5f *serialized);

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) Eurydice_arr_5f libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_11_b6(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self);

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_21(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self,
    Eurydice_arr_7d *serialized);

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) Eurydice_arr_7d libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_11_21(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self);

/**
 Take a serialized private key and generate an unpacked key pair from it.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.keys_from_private_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- T_AS_NTT_ENCODED_SIZE= 1152
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_keys_from_private_key_01(
    const Eurydice_arr_7d *private_key,
    libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *key_pair);

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_30
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51
libcrux_ml_kem_ind_cca_unpacked_default_30_68(void);

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_7b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
libcrux_ml_kem_ind_cca_unpacked_default_7b_68(void);

/**
 Generate Unpacked Keys
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_generate_keypair_b8(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked *out);

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- PUBLIC_KEY_SIZE= 1184
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- VECTOR_U_BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_f4 libcrux_ml_kem_ind_cca_unpacked_encapsulate_a7(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *public_key,
    const Eurydice_arr_ec *randomness);

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- CIPHERTEXT_SIZE= 1088
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- C1_BLOCK_SIZE= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1120
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_unpacked_decapsulate_0c(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *key_pair,
    const Eurydice_arr_2b *ciphertext);

/**
 Validate an ML-KEM public key.

 This implements the Modulus check in 7.2 2.
 Note that the size check in 7.2 1 is covered by the `PUBLIC_KEY_SIZE` in the
 `public_key` type.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_public_key_b6(
    const Eurydice_arr_5f *public_key);

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key_only
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_only_52(
    const Eurydice_arr_7d *private_key);

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
 Note that the size checks in 7.2 1 and 2 are covered by the `SECRET_KEY_SIZE`
 and `CIPHERTEXT_SIZE` in the `private_key` and `ciphertext` types.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CIPHERTEXT_SIZE= 1088
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_ba(
    const Eurydice_arr_7d *private_key, const Eurydice_arr_2b *_ciphertext);

/**
 Packed API

 Generate a key pair.

 Depending on the `Vector` and `Hasher` used, this requires different hardware
 features
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem768_MlKem768KeyPair
libcrux_ml_kem_ind_cca_generate_keypair_b8(const Eurydice_arr_c7 *randomness);

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- PUBLIC_KEY_SIZE= 1184
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- C1_BLOCK_SIZE= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_f4 libcrux_ml_kem_ind_cca_encapsulate_99(
    const Eurydice_arr_5f *public_key, const Eurydice_arr_ec *randomness);

/**
 This code verifies on some machines, runs out of memory on others
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- CIPHERTEXT_SIZE= 1088
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- C1_BLOCK_SIZE= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1120
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_decapsulate_fd(
    const Eurydice_arr_7d *private_key, const Eurydice_arr_2b *ciphertext);

#if defined(__cplusplus)
}
#endif

#define internal_libcrux_mlkem_portable_H_DEFINED
#endif /* internal_libcrux_mlkem_portable_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_mlkem1024_portable.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef libcrux_mlkem1024_portable_H
#define libcrux_mlkem1024_portable_H


#if defined(__cplusplus)
extern "C" {
#endif


/**
 Decapsulate ML-KEM 1024

 Generates an [`MlKemSharedSecret`].
 The input is a reference to an [`MlKem1024PrivateKey`] and an
 [`MlKem1024Ciphertext`].
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_mlkem1024_portable_decapsulate(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext);

/**
 Encapsulate ML-KEM 1024

 Generates an ([`MlKem1024Ciphertext`], [`MlKemSharedSecret`]) tuple.
 The input is a reference to an [`MlKem1024PublicKey`] and
 [`SHARED_SECRET_SIZE`] bytes of `randomness`.
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_mlkem1024_portable_encapsulate(
    const Eurydice_arr_d1 *public_key, Eurydice_arr_ec randomness);

/**
 Generate ML-KEM 1024 Key Pair
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair
libcrux_ml_kem_mlkem1024_portable_generate_key_pair(Eurydice_arr_c7 randomness);

/**
 Validate a private key.

 Returns `true` if valid, and `false` otherwise.
*/
static __attribute__((unused)) bool libcrux_ml_kem_mlkem1024_portable_validate_private_key(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext);

/**
 Validate the private key only.

 Returns `true` if valid, and `false` otherwise.
*/
static __attribute__((unused)) bool libcrux_ml_kem_mlkem1024_portable_validate_private_key_only(
    const Eurydice_arr_a8 *private_key);

/**
 Validate a public key.

 Returns `true` if valid, and `false` otherwise.
*/
static __attribute__((unused)) bool libcrux_ml_kem_mlkem1024_portable_validate_public_key(
    const Eurydice_arr_d1 *public_key);

#if defined(__cplusplus)
}
#endif

#define libcrux_mlkem1024_portable_H_DEFINED
#endif /* libcrux_mlkem1024_portable_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/internal/libcrux_mlkem1024_portable.h */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */

#ifndef internal_libcrux_mlkem1024_portable_H
#define internal_libcrux_mlkem1024_portable_H


#if defined(__cplusplus)
extern "C" {
#endif


typedef libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024PublicKeyUnpacked;

/**
 Decapsulate ML-KEM 1024 (unpacked)

 Generates an [`MlKemSharedSecret`].
 The input is a reference to an unpacked key pair of type
 [`MlKem1024KeyPairUnpacked`] and an [`MlKem1024Ciphertext`].
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_mlkem1024_portable_unpacked_decapsulate(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *private_key,
    const Eurydice_arr_d1 *ciphertext);

/**
 Encapsulate ML-KEM 1024 (unpacked)

 Generates an ([`MlKem1024Ciphertext`], [`MlKemSharedSecret`]) tuple.
 The input is a reference to an unpacked public key of type
 [`MlKem1024PublicKeyUnpacked`], the SHA3-256 hash of this public key, and
 [`SHARED_SECRET_SIZE`] bytes of `randomness`.
 TODO: The F* prefix opens required modules, it should go away when the
 following issue is resolved: <https://github.com/hacspec/hax/issues/770>
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_mlkem1024_portable_unpacked_encapsulate(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    Eurydice_arr_ec randomness);

/**
 Generate ML-KEM 1024 Key Pair in "unpacked" form
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_generate_key_pair_mut(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair);

/**
 Generate ML-KEM 1024 Key Pair in "unpacked" form.
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
libcrux_ml_kem_mlkem1024_portable_unpacked_generate_key_pair(
    Eurydice_arr_c7 randomness);

/**
 Create a new, empty unpacked key.
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
libcrux_ml_kem_mlkem1024_portable_unpacked_init_key_pair(void);

/**
 Create a new, empty unpacked public key.
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
libcrux_ml_kem_mlkem1024_portable_unpacked_init_public_key(void);

/**
 Get an unpacked key from a private key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_from_private_mut(
    const Eurydice_arr_a8 *private_key,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair);

/**
 Get the serialized private key.
*/
static __attribute__((unused)) Eurydice_arr_a8
libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_private_key(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair);

/**
 Get the serialized private key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_private_key_mut(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    Eurydice_arr_a8 *serialized);

/**
 Get the serialized public key.
*/
static __attribute__((unused)) Eurydice_arr_d1
libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_public_key(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair);

/**
 Get the serialized public key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_public_key_mut(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    Eurydice_arr_d1 *serialized);

/**
 Get the serialized public key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_serialized_public_key(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    Eurydice_arr_d1 *serialized);

/**
 Get the unpacked public key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_unpacked_public_key(
    const Eurydice_arr_d1 *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
        *unpacked_public_key);

#if defined(__cplusplus)
}
#endif

#define internal_libcrux_mlkem1024_portable_H_DEFINED
#endif /* internal_libcrux_mlkem1024_portable_H */


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_core.c */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */


/**
 Return 1 if `value` is not zero and 0 otherwise.
*/
static __attribute__((unused)) KRML_NOINLINE uint8_t inz(uint8_t value) {
  uint16_t value0 = (uint16_t)(uint32_t)value;
  uint8_t result =
      (uint8_t)((uint32_t)core_num__u16__wrapping_add(~value0, 1U) >> 8U &
                0xFFFFU);
  return (uint32_t)result & 1U;
}

static __attribute__((unused)) KRML_NOINLINE uint8_t is_non_zero(uint8_t value) { return inz(value); }

/**
 Return 1 if the bytes of `lhs` and `rhs` do not exactly
 match and 0 otherwise.
*/
static __attribute__((unused)) KRML_NOINLINE uint8_t compare(Eurydice_borrow_slice_u8 lhs,
                                     Eurydice_borrow_slice_u8 rhs) {
  uint8_t r = 0U;
  for (size_t i = (size_t)0U; i < lhs.meta; i++) {
    size_t i0 = i;
    uint8_t nr = (uint32_t)r | ((uint32_t)lhs.ptr[i0] ^ (uint32_t)rhs.ptr[i0]);
    r = nr;
  }
  return is_non_zero(r);
}

static __attribute__((unused)) KRML_NOINLINE uint8_t
libcrux_ml_kem_constant_time_ops_compare_ciphertexts_in_constant_time(
    Eurydice_borrow_slice_u8 lhs, Eurydice_borrow_slice_u8 rhs) {
  return compare(lhs, rhs);
}

/**
 If `selector` is not zero, return the bytes in `rhs`; return the bytes in
 `lhs` otherwise.
*/
static __attribute__((unused)) KRML_NOINLINE Eurydice_arr_ec select_ct(Eurydice_borrow_slice_u8 lhs,
                                               Eurydice_borrow_slice_u8 rhs,
                                               uint8_t selector) {
  uint8_t mask = core_num__u8__wrapping_sub(is_non_zero(selector), 1U);
  Eurydice_arr_ec out = {.data = {0U}};
  for (size_t i = (size_t)0U; i < LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE;
       i++) {
    size_t i0 = i;
    uint8_t outi = ((uint32_t)lhs.ptr[i0] & (uint32_t)mask) |
                   ((uint32_t)rhs.ptr[i0] & (~(uint32_t)mask & 0xFFU));
    out.data[i0] = outi;
  }
  return out;
}

static __attribute__((unused)) KRML_NOINLINE Eurydice_arr_ec
libcrux_ml_kem_constant_time_ops_select_shared_secret_in_constant_time(
    Eurydice_borrow_slice_u8 lhs, Eurydice_borrow_slice_u8 rhs,
    uint8_t selector) {
  return select_ct(lhs, rhs, selector);
}

static __attribute__((unused)) KRML_NOINLINE Eurydice_arr_ec
libcrux_ml_kem_constant_time_ops_compare_ciphertexts_select_shared_secret_in_constant_time(
    Eurydice_borrow_slice_u8 lhs_c, Eurydice_borrow_slice_u8 rhs_c,
    Eurydice_borrow_slice_u8 lhs_s, Eurydice_borrow_slice_u8 rhs_s) {
  uint8_t selector =
      libcrux_ml_kem_constant_time_ops_compare_ciphertexts_in_constant_time(
          lhs_c, rhs_c);
  return libcrux_ml_kem_constant_time_ops_select_shared_secret_in_constant_time(
      lhs_s, rhs_s, selector);
}

/**
 K * BITS_PER_RING_ELEMENT / 8

 [eurydice] Note that we can't use const generics here because that breaks
            C extraction with eurydice.
*/
static __attribute__((unused)) size_t libcrux_ml_kem_constants_ranked_bytes_per_ring_element(size_t rank) {
  return rank * LIBCRUX_ML_KEM_CONSTANTS_BITS_PER_RING_ELEMENT / (size_t)8U;
}

/**
 Construct a public integer (identity)
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.secret
with types int16_t

*/
static __attribute__((unused)) KRML_MUSTINLINE int16_t secret_39(int16_t x) { return x; }

static __attribute__((unused)) int16_t libcrux_secrets_int_I16(int16_t v) { return secret_39(v); }

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types int16_t

*/
static __attribute__((unused)) int16_t libcrux_secrets_int_public_integers_classify_27_39(int16_t self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types uint8_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint8_t declassify_d8_90(uint8_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for u8}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_59(uint8_t self) {
  return libcrux_secrets_int_public_integers_classify_27_39(
      (int16_t)(uint32_t)declassify_d8_90(self));
}

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types uint8_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint8_t classify_27_90(uint8_t self) { return self; }

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types int16_t

*/
static __attribute__((unused)) int16_t libcrux_secrets_int_public_integers_declassify_d8_39(int16_t self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) uint8_t libcrux_secrets_int_as_u8_f5(int16_t self) {
  return classify_27_90(
      (uint8_t)libcrux_secrets_int_public_integers_declassify_d8_39(self));
}

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types int32_t

*/
static __attribute__((unused)) KRML_MUSTINLINE int32_t classify_27_a8(int32_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) int32_t libcrux_secrets_int_as_i32_f5(int16_t self) {
  return classify_27_a8(
      (int32_t)libcrux_secrets_int_public_integers_declassify_d8_39(self));
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types int32_t

*/
static __attribute__((unused)) KRML_MUSTINLINE int32_t declassify_d8_a8(int32_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for i32}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_36(int32_t self) {
  return libcrux_secrets_int_public_integers_classify_27_39(
      (int16_t)declassify_d8_a8(self));
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types uint32_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint32_t declassify_d8_df(uint32_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for u32}
*/
static __attribute__((unused)) int32_t libcrux_secrets_int_as_i32_b8(uint32_t self) {
  return classify_27_a8((int32_t)declassify_d8_df(self));
}

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types uint16_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint16_t classify_27_de(uint16_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) uint16_t libcrux_secrets_int_as_u16_f5(int16_t self) {
  return classify_27_de(
      (uint16_t)libcrux_secrets_int_public_integers_declassify_d8_39(self));
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types uint16_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint16_t declassify_d8_de(uint16_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for u16}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_ca(uint16_t self) {
  return libcrux_secrets_int_public_integers_classify_27_39(
      (int16_t)(uint32_t)declassify_d8_de(self));
}

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types uint64_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t classify_27_49(uint64_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for u16}
*/
static __attribute__((unused)) uint64_t libcrux_secrets_int_as_u64_ca(uint16_t self) {
  return classify_27_49((uint64_t)(uint32_t)declassify_d8_de(self));
}

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types uint32_t

*/
static __attribute__((unused)) uint32_t libcrux_secrets_int_public_integers_classify_27_df(uint32_t self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types uint64_t

*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t declassify_d8_49(uint64_t self) { return self; }

/**
This function found in impl {libcrux_secrets::int::CastOps for u64}
*/
static __attribute__((unused)) uint32_t libcrux_secrets_int_as_u32_a3(uint64_t self) {
  return libcrux_secrets_int_public_integers_classify_27_df(
      (uint32_t)declassify_d8_49(self));
}

/**
This function found in impl {libcrux_secrets::int::CastOps for u32}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_b8(uint32_t self) {
  return libcrux_secrets_int_public_integers_classify_27_39(
      (int16_t)declassify_d8_df(self));
}

/**
This function found in impl {libcrux_secrets::int::CastOps for i16}
*/
static __attribute__((unused)) int16_t libcrux_secrets_int_as_i16_f5(int16_t self) {
  return libcrux_secrets_int_public_integers_classify_27_39(
      libcrux_secrets_int_public_integers_declassify_d8_39(self));
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.default_d3
with const generics
- SIZE= 3168
*/
static __attribute__((unused)) Eurydice_arr_a8 libcrux_ml_kem_types_default_d3_0e(void) {
  return (KRML_CLITERAL(Eurydice_arr_a8){.data = {0U}});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_to_shared_211(
    const Eurydice_arr_d1 *a, size_t r) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = r;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 3168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d46(
    const Eurydice_arr_a8 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_51
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_d1 libcrux_ml_kem_types_from_51_d9(Eurydice_arr_d1 value) {
  return value;
}

/**
 Create a new [`MlKemKeyPair`] from the secret and public key.
*/
/**
This function found in impl
{libcrux_ml_kem::types::MlKemKeyPair<PRIVATE_KEY_SIZE, PUBLIC_KEY_SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_17
with const generics
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair libcrux_ml_kem_types_from_17_70(
    Eurydice_arr_a8 sk, Eurydice_arr_d1 pk) {
  return (KRML_CLITERAL(libcrux_ml_kem_mlkem1024_MlKem1024KeyPair){.sk = sk,
                                                                   .pk = pk});
}

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_b2
with const generics
- SIZE= 3168
*/
static __attribute__((unused)) Eurydice_arr_a8 libcrux_ml_kem_types_from_b2_0e(Eurydice_arr_a8 value) {
  return value;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1536
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_2f(
    const Eurydice_arr_df *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1536U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 3168
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d417(
    Eurydice_arr_a8 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 1536
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_2f(
    Eurydice_arr_df *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1536U;
  return lit;
}

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_19
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_d1 libcrux_ml_kem_types_from_19_d9(Eurydice_arr_d1 value) {
  return value;
}

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_e6
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) const Eurydice_arr_d1 *libcrux_ml_kem_types_as_slice_e6_d9(
    const Eurydice_arr_d1 *self) {
  return self;
}

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_a9
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) const Eurydice_arr_d1 *libcrux_ml_kem_types_as_slice_a9_d9(
    const Eurydice_arr_d1 *self) {
  return self;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f4(
    Eurydice_arr_d1 *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){
      .ptr = a->data + r, .meta = (size_t)1568U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d416(
    Eurydice_arr_d1 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 352
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d415(
    Eurydice_arr_e7 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 352
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_25(
    const Eurydice_arr_e7 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)352U;
  return lit;
}

/**
A monomorphic instance of libcrux_ml_kem.utils.prf_input_inc
with const generics
- K= 4
*/
static __attribute__((unused)) uint8_t libcrux_ml_kem_utils_prf_input_inc_23(Eurydice_arr_890 *prf_inputs,
                                              uint8_t domain_separator) {
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
                  prf_inputs->data[i0].data[32U] = domain_separator;
                  domain_separator = (uint32_t)domain_separator + 1U;);
  return domain_separator;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1600
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_720(
    const Eurydice_arr_14 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1600U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_b50(
    const Eurydice_arr_d1 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1568U;
  return lit;
}

/**
This function found in impl {core::convert::AsRef<[u8]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_ref_c1
with const generics
- SIZE= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 libcrux_ml_kem_types_as_ref_c1_d9(
    const Eurydice_arr_d1 *self) {
  return Eurydice_array_to_slice_shared_b50(self);
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1600
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f3(
    Eurydice_arr_14 *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){
      .ptr = a->data + r, .meta = (size_t)1600U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1600
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 array_to_subslice_mut_d414(
    Eurydice_arr_14 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 1600
*/
static __attribute__((unused)) Eurydice_arr_14 libcrux_ml_kem_utils_into_padded_array_49(
    Eurydice_borrow_slice_u8 slice) {
  Eurydice_arr_14 out = {.data = {0U}};
  Eurydice_slice_copy(array_to_subslice_mut_d414(
                          &out, (KRML_CLITERAL(core_ops_range_Range_87){
                                    .start = (size_t)0U, .end = slice.meta})),
                      slice, uint8_t);
  return out;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f2(
    const Eurydice_arr_d1 *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r,
                                                  .meta = (size_t)1568U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d45(
    const Eurydice_arr_d1 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 3168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_68(
    const Eurydice_arr_a8 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)3168U;
  return lit;
}

/**
 Unpack an incoming private key into it's different parts.

 We have this here in types to extract into a common core for C.
*/
/**
A monomorphic instance of libcrux_ml_kem.types.unpack_private_key
with const generics
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8_x4 libcrux_ml_kem_types_unpack_private_key_e3(
    Eurydice_borrow_slice_u8 private_key) {
  Eurydice_borrow_slice_u8_x2 uu____0 = Eurydice_slice_split_at(
      private_key, (size_t)1536U, uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 ind_cpa_secret_key = uu____0.fst;
  Eurydice_borrow_slice_u8 secret_key0 = uu____0.snd;
  Eurydice_borrow_slice_u8_x2 uu____1 = Eurydice_slice_split_at(
      secret_key0, (size_t)1568U, uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 ind_cpa_public_key = uu____1.fst;
  Eurydice_borrow_slice_u8 secret_key = uu____1.snd;
  Eurydice_borrow_slice_u8_x2 uu____2 = Eurydice_slice_split_at(
      secret_key, LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE, uint8_t,
      Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 ind_cpa_public_key_hash = uu____2.fst;
  Eurydice_borrow_slice_u8 implicit_rejection_value = uu____2.snd;
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8_x4){
      .fst = ind_cpa_secret_key,
      .snd = ind_cpa_public_key,
      .thd = ind_cpa_public_key_hash,
      .f3 = implicit_rejection_value});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d44(
    Eurydice_arr_ec *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 32
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_utils_into_padded_array_ce(
    Eurydice_borrow_slice_u8 slice) {
  Eurydice_arr_ec out = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d44(
                          &out, (KRML_CLITERAL(core_ops_range_Range_87){
                                    .start = (size_t)0U, .end = slice.meta})),
                      slice, uint8_t);
  return out;
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.default_d3
with const generics
- SIZE= 2400
*/
static __attribute__((unused)) Eurydice_arr_7d libcrux_ml_kem_types_default_d3_79(void) {
  return (KRML_CLITERAL(Eurydice_arr_7d){.data = {0U}});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f1(
    const Eurydice_arr_5f *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r,
                                                  .meta = (size_t)1184U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_to_shared_210(
    const Eurydice_arr_5f *a, size_t r) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = r;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 2400
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d44(
    const Eurydice_arr_7d *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_51
with const generics
- SIZE= 1184
*/
static __attribute__((unused)) Eurydice_arr_5f libcrux_ml_kem_types_from_51_3d(Eurydice_arr_5f value) {
  return value;
}

/**
 Create a new [`MlKemKeyPair`] from the secret and public key.
*/
/**
This function found in impl
{libcrux_ml_kem::types::MlKemKeyPair<PRIVATE_KEY_SIZE, PUBLIC_KEY_SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_17
with const generics
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem768_MlKem768KeyPair libcrux_ml_kem_types_from_17_bc(
    Eurydice_arr_7d sk, Eurydice_arr_5f pk) {
  return (KRML_CLITERAL(libcrux_ml_kem_mlkem768_MlKem768KeyPair){.sk = sk,
                                                                 .pk = pk});
}

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemPrivateKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_b2
with const generics
- SIZE= 2400
*/
static __attribute__((unused)) Eurydice_arr_7d libcrux_ml_kem_types_from_b2_79(Eurydice_arr_7d value) {
  return value;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1152
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_f4(
    const Eurydice_arr_0e *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1152U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 2400
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d413(
    Eurydice_arr_7d *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 1152
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_f4(
    Eurydice_arr_0e *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1152U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f2(
    Eurydice_arr_5f *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){
      .ptr = a->data + r, .meta = (size_t)1184U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d412(
    Eurydice_arr_5f *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 24
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_ed(
    const Eurydice_arr_94 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)24U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 384
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d411(
    Eurydice_arr_b2 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 384
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_a9(
    const Eurydice_arr_b2 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)384U;
  return lit;
}

/**
This function found in impl {core::result::Result<T, E>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of core.result.unwrap_26
with types Eurydice_arr uint8_t[[$32size_t]], core_array_TryFromSliceError

*/
static __attribute__((unused)) Eurydice_arr_ec core_result_unwrap_26_39(core_result_Result_07 self) {
  if (self.tag == core_result_Ok) {
    return self.val.case_Ok;
  } else {
    KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                      "unwrap not Ok");
    KRML_HOST_EXIT(255U);
  }
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f0(
    const Eurydice_arr_c7 *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r,
                                                  .meta = (size_t)64U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d43(
    const Eurydice_arr_c7 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
This function found in impl {core::convert::From<[u8; SIZE]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.from_19
with const generics
- SIZE= 1088
*/
static __attribute__((unused)) Eurydice_arr_2b libcrux_ml_kem_types_from_19_52(Eurydice_arr_2b value) {
  return value;
}

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemPublicKey<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_e6
with const generics
- SIZE= 1184
*/
static __attribute__((unused)) const Eurydice_arr_5f *libcrux_ml_kem_types_as_slice_e6_3d(
    const Eurydice_arr_5f *self) {
  return self;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_ff(
    const Eurydice_arr_5f *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1184U;
  return lit;
}

/**
 A reference to the raw byte slice.
*/
/**
This function found in impl {libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_slice_a9
with const generics
- SIZE= 1088
*/
static __attribute__((unused)) const Eurydice_arr_2b *libcrux_ml_kem_types_as_slice_a9_52(
    const Eurydice_arr_2b *self) {
  return self;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f1(
    Eurydice_arr_2b *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){
      .ptr = a->data + r, .meta = (size_t)1088U - r});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 10
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_30(
    const Eurydice_arr_6d *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)10U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d42(
    const Eurydice_arr_ec *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d410(
    Eurydice_arr_2b *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 22
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_98(
    const Eurydice_arr_80 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)22U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 20
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_8f(
    const Eurydice_arr_fc *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)20U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 320
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d49(
    Eurydice_arr_b0 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 320
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_56(
    const Eurydice_arr_b0 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)320U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 128
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_78(
    const Eurydice_arr_89 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)128U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types int16_t
with const generics
- N= 256
*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_array_to_slice_shared_99(
    const Eurydice_arr_04 *a) {
  Eurydice_borrow_slice_i16 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)256U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 33
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_b5(
    const Eurydice_arr_fa *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)33U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 128
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_78(
    Eurydice_arr_89 *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)128U;
  return lit;
}

/**
A monomorphic instance of libcrux_ml_kem.utils.prf_input_inc
with const generics
- K= 3
*/
static __attribute__((unused)) uint8_t libcrux_ml_kem_utils_prf_input_inc_78(Eurydice_arr_800 *prf_inputs,
                                              uint8_t domain_separator) {
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
                  prf_inputs->data[i0].data[32U] = domain_separator;
                  domain_separator = (uint32_t)domain_separator + 1U;);
  return domain_separator;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 33
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d48(
    Eurydice_arr_fa *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 33
*/
static __attribute__((unused)) Eurydice_arr_fa libcrux_ml_kem_utils_into_padded_array_29(
    Eurydice_borrow_slice_u8 slice) {
  Eurydice_arr_fa out = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d48(
                          &out, (KRML_CLITERAL(core_ops_range_Range_87){
                                    .start = (size_t)0U, .end = slice.meta})),
                      slice, uint8_t);
  return out;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 34
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 array_to_subslice_mut_d47(
    Eurydice_arr_31 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 34
*/
static __attribute__((unused)) Eurydice_arr_31 libcrux_ml_kem_utils_into_padded_array_de(
    Eurydice_borrow_slice_u8 slice) {
  Eurydice_arr_31 out = {.data = {0U}};
  Eurydice_slice_copy(array_to_subslice_mut_d47(
                          &out, (KRML_CLITERAL(core_ops_range_Range_87){
                                    .start = (size_t)0U, .end = slice.meta})),
                      slice, uint8_t);
  return out;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t
with const generics
- N= 272
*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_array_to_subslice_shared_e70(
    const Eurydice_arr_5b *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_i16){.ptr = a->data + r.start,
                                                   .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d41(
    const Eurydice_arr_c5 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_2c(
    Eurydice_arr_c5 *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)168U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t
with const generics
- N= 272
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_i16 Eurydice_array_to_subslice_mut_e7(
    Eurydice_arr_5b *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_i16){
      .ptr = a->data + r.start, .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 504
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d40(
    const Eurydice_arr_79 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 504
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_48(
    Eurydice_arr_79 *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)504U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 34
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_e9(
    const Eurydice_arr_31 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)34U;
  return lit;
}

/**
A monomorphic instance of Eurydice.slice_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_slice_subslice_from_shared_6d(
    Eurydice_borrow_slice_u8 s, size_t r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = s.ptr + r,
                                                  .meta = s.meta - r});
}

/**
A monomorphic instance of Eurydice.slice_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_slice_subslice_to_shared_72(
    Eurydice_borrow_slice_u8 s, size_t r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = s.ptr, .meta = r});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1120
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_81(
    const Eurydice_arr_af *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1120U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_06(
    const Eurydice_arr_2b *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)1088U;
  return lit;
}

/**
This function found in impl {core::convert::AsRef<[u8]> for
libcrux_ml_kem::types::MlKemCiphertext<SIZE>}
*/
/**
A monomorphic instance of libcrux_ml_kem.types.as_ref_c1
with const generics
- SIZE= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 libcrux_ml_kem_types_as_ref_c1_52(
    const Eurydice_arr_2b *self) {
  return Eurydice_array_to_slice_shared_06(self);
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1120
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f0(
    Eurydice_arr_af *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){
      .ptr = a->data + r, .meta = (size_t)1120U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1120
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 array_to_subslice_mut_d46(
    Eurydice_arr_af *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 1120
*/
static __attribute__((unused)) Eurydice_arr_af libcrux_ml_kem_utils_into_padded_array_66(
    Eurydice_borrow_slice_u8 slice) {
  Eurydice_arr_af out = {.data = {0U}};
  Eurydice_slice_copy(array_to_subslice_mut_d46(
                          &out, (KRML_CLITERAL(core_ops_range_Range_87){
                                    .start = (size_t)0U, .end = slice.meta})),
                      slice, uint8_t);
  return out;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_17(
    const Eurydice_arr_c7 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)64U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_mut
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_from_mut_5f(
    Eurydice_arr_c7 *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r,
                                                      .meta = (size_t)64U - r});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_01(
    const Eurydice_arr_ec *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)32U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 array_to_subslice_mut_d45(
    Eurydice_arr_c7 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
 Pad the `slice` with `0`s at the end.
*/
/**
A monomorphic instance of libcrux_ml_kem.utils.into_padded_array
with const generics
- LEN= 64
*/
static __attribute__((unused)) Eurydice_arr_c7 libcrux_ml_kem_utils_into_padded_array_c9(
    Eurydice_borrow_slice_u8 slice) {
  Eurydice_arr_c7 out = {.data = {0U}};
  Eurydice_slice_copy(array_to_subslice_mut_d45(
                          &out, (KRML_CLITERAL(core_ops_range_Range_87){
                                    .start = (size_t)0U, .end = slice.meta})),
                      slice, uint8_t);
  return out;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 2
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_82(
    const Eurydice_array_u8x2 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)2U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_from_shared
with types uint8_t, core_ops_range_RangeFrom size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_from_shared_5f(
    const Eurydice_arr_2b *a, size_t r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r,
                                                  .meta = (size_t)1088U - r});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 1088
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_shared_d4(
    const Eurydice_arr_2b *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = a->data + r.start,
                                                  .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 2400
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_51(
    const Eurydice_arr_7d *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)2400U;
  return lit;
}

/**
 Unpack an incoming private key into it's different parts.

 We have this here in types to extract into a common core for C.
*/
/**
A monomorphic instance of libcrux_ml_kem.types.unpack_private_key
with const generics
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8_x4 libcrux_ml_kem_types_unpack_private_key_64(
    Eurydice_borrow_slice_u8 private_key) {
  Eurydice_borrow_slice_u8_x2 uu____0 = Eurydice_slice_split_at(
      private_key, (size_t)1152U, uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 ind_cpa_secret_key = uu____0.fst;
  Eurydice_borrow_slice_u8 secret_key0 = uu____0.snd;
  Eurydice_borrow_slice_u8_x2 uu____1 = Eurydice_slice_split_at(
      secret_key0, (size_t)1184U, uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 ind_cpa_public_key = uu____1.fst;
  Eurydice_borrow_slice_u8 secret_key = uu____1.snd;
  Eurydice_borrow_slice_u8_x2 uu____2 = Eurydice_slice_split_at(
      secret_key, LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE, uint8_t,
      Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 ind_cpa_public_key_hash = uu____2.fst;
  Eurydice_borrow_slice_u8 implicit_rejection_value = uu____2.snd;
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8_x4){
      .fst = ind_cpa_secret_key,
      .snd = ind_cpa_public_key,
      .thd = ind_cpa_public_key_hash,
      .f3 = implicit_rejection_value});
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$24size_t]]

*/
static __attribute__((unused)) Eurydice_arr_94 libcrux_secrets_int_public_integers_declassify_d8_40(
    Eurydice_arr_94 self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$22size_t]]

*/
static __attribute__((unused)) Eurydice_arr_80 libcrux_secrets_int_public_integers_declassify_d8_0b(
    Eurydice_arr_80 self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$20size_t]]

*/
static __attribute__((unused)) Eurydice_arr_fc libcrux_secrets_int_public_integers_declassify_d8_2b(
    Eurydice_arr_fc self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$10size_t]]

*/
static __attribute__((unused)) Eurydice_arr_6d libcrux_secrets_int_public_integers_declassify_d8_37(
    Eurydice_arr_6d self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$8size_t]]

*/
static __attribute__((unused)) Eurydice_array_u8x8 libcrux_secrets_int_public_integers_declassify_d8_52(
    Eurydice_array_u8x8 self) {
  return self;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_shared
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t
with const generics
- N= 16
*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_array_to_subslice_shared_e7(
    const Eurydice_arr_d6 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_i16){.ptr = a->data + r.start,
                                                   .meta = r.end - r.start});
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr uint8_t[[$2size_t]]

*/
static __attribute__((unused)) Eurydice_array_u8x2 libcrux_secrets_int_public_integers_declassify_d8_75(
    Eurydice_array_u8x2 self) {
  return self;
}

/**
 Classify a mutable slice (identity)
 We define a separate function for this because hax has limited support for
 &mut-returning functions
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_mut_slice
with types Eurydice_dst_ref_mut uint8_t size_t

*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8
libcrux_secrets_int_public_integers_classify_mut_slice_75(
    Eurydice_mut_borrow_slice_u8 x) {
  return x;
}

/**
This function found in impl {libcrux_secrets::traits::ClassifyRef<&'a ([T])> for
&'a ([T])}
*/
/**
A monomorphic instance of libcrux_secrets.int.classify_public.classify_ref_6d
with types uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 libcrux_secrets_int_classify_public_classify_ref_6d_90(
    Eurydice_borrow_slice_u8 self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::Declassify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.declassify_d8
with types Eurydice_arr int16_t[[$16size_t]]

*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_secrets_int_public_integers_declassify_d8_4b(
    Eurydice_arr_d6 self) {
  return self;
}

/**
This function found in impl {libcrux_secrets::traits::ClassifyRef<&'a ([T])> for
&'a ([T])}
*/
/**
A monomorphic instance of libcrux_secrets.int.classify_public.classify_ref_6d
with types int16_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_i16
libcrux_secrets_int_classify_public_classify_ref_6d_39(
    Eurydice_borrow_slice_i16 self) {
  return self;
}

/**
A monomorphic instance of Eurydice.slice_subslice_shared
with types int16_t, core_ops_range_Range size_t, Eurydice_derefed_slice int16_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_i16 Eurydice_slice_subslice_shared_a6(
    Eurydice_borrow_slice_i16 s, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_i16){.ptr = s.ptr + r.start,
                                                   .meta = r.end - r.start});
}

/**
This function found in impl {core::result::Result<T, E>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of core.result.unwrap_26
with types Eurydice_arr int16_t[[$16size_t]], core_array_TryFromSliceError

*/
static __attribute__((unused)) Eurydice_arr_d6 core_result_unwrap_26_d3(core_result_Result_ec self) {
  if (self.tag == core_result_Ok) {
    return self.val.case_Ok;
  } else {
    KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                      "unwrap not Ok");
    KRML_HOST_EXIT(255U);
  }
}

/**
This function found in impl {libcrux_secrets::traits::Classify<T> for T}
*/
/**
A monomorphic instance of libcrux_secrets.int.public_integers.classify_27
with types Eurydice_arr int16_t[[$16size_t]]

*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_secrets_int_public_integers_classify_27_4b(
    Eurydice_arr_d6 self) {
  return self;
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 64
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_17(
    Eurydice_arr_c7 *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)64U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 48
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_9f(
    Eurydice_arr_65 *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)48U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 32
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_01(
    Eurydice_arr_ec *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)32U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_mut
with types uint8_t
with const generics
- N= 28
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_slice_mut_5e(
    Eurydice_arr_a2 *a) {
  Eurydice_mut_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)28U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 104
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_72(
    const Eurydice_arr_c4 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)104U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 104
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d43(
    Eurydice_arr_c4 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 144
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_38(
    const Eurydice_arr_f4 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)144U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 144
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d42(
    Eurydice_arr_f4 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_2c(
    const Eurydice_arr_c5 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)168U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 168
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d41(
    Eurydice_arr_c5 *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 136
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_58(
    const Eurydice_arr_ff *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)136U;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 136
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d40(
    Eurydice_arr_ff *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_to_shared
with types uint8_t, core_ops_range_RangeTo size_t, Eurydice_derefed_slice
uint8_t with const generics
- N= 8
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_subslice_to_shared_21(
    const Eurydice_array_u8x8 *a, size_t r) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = r;
  return lit;
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 8
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_6e(
    const Eurydice_array_u8x8 *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)8U;
  return lit;
}

/**
A monomorphic instance of Eurydice.slice_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t

*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_slice_subslice_mut_c8(
    Eurydice_mut_borrow_slice_u8 s, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = s.ptr + r.start,
                                                      .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_slice_shared
with types uint8_t
with const generics
- N= 72
*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_array_to_slice_shared_e2(
    const Eurydice_arr_ab *a) {
  Eurydice_borrow_slice_u8 lit;
  lit.ptr = a->data;
  lit.meta = (size_t)72U;
  return lit;
}

/**
This function found in impl {core::result::Result<T, E>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of core.result.unwrap_26
with types Eurydice_arr uint8_t[[$8size_t]], core_array_TryFromSliceError

*/
static __attribute__((unused)) Eurydice_array_u8x8 core_result_unwrap_26_e0(core_result_Result_8e self) {
  if (self.tag == core_result_Ok) {
    return self.val.case_Ok;
  } else {
    KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                      "unwrap not Ok");
    KRML_HOST_EXIT(255U);
  }
}

/**
A monomorphic instance of Eurydice.slice_subslice_shared
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t

*/
static __attribute__((unused)) Eurydice_borrow_slice_u8 Eurydice_slice_subslice_shared_c8(
    Eurydice_borrow_slice_u8 s, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_borrow_slice_u8){.ptr = s.ptr + r.start,
                                                  .meta = r.end - r.start});
}

/**
A monomorphic instance of Eurydice.array_to_subslice_mut
with types uint8_t, core_ops_range_Range size_t, Eurydice_derefed_slice uint8_t
with const generics
- N= 72
*/
static __attribute__((unused)) Eurydice_mut_borrow_slice_u8 Eurydice_array_to_subslice_mut_d4(
    Eurydice_arr_ab *a, core_ops_range_Range_87 r) {
  return (KRML_CLITERAL(Eurydice_mut_borrow_slice_u8){.ptr = a->data + r.start,
                                                      .meta = r.end - r.start});
}


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_sha3_portable.c */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */



/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_zero_d2(void) {
  return 0ULL;
}

static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__veor5q_u64(
    uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e) {
  return (((a ^ b) ^ c) ^ d) ^ e;
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_xor5_d2(
    uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e) {
  return libcrux_sha3_simd_portable__veor5q_u64(a, b, c, d, e);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 1
- RIGHT= 63
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_76(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)1);
}

static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vrax1q_u64(uint64_t a,
                                                                uint64_t b) {
  return a ^ libcrux_sha3_simd_portable_rotate_left_76(b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vrax1q_u64(a, b);
}

static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vbcaxq_u64(uint64_t a,
                                                                uint64_t b,
                                                                uint64_t c) {
  return a ^ (b & ~c);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_and_not_xor_d2(uint64_t a,
                                                                   uint64_t b,
                                                                   uint64_t c) {
  return libcrux_sha3_simd_portable__vbcaxq_u64(a, b, c);
}

static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__veorq_n_u64(uint64_t a,
                                                                 uint64_t c) {
  return a ^ c;
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_constant_d2(uint64_t a, uint64_t c) {
  return libcrux_sha3_simd_portable__veorq_n_u64(a, c);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_xor_d2(uint64_t a,
                                                           uint64_t b) {
  return a ^ b;
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.new_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_7c libcrux_sha3_generic_keccak_new_80_71(void) {
  Eurydice_arr_7c lit;
  uint64_t repeat_expression[25U];
  for (size_t i = (size_t)0U; i < (size_t)25U; i++) {
    repeat_expression[i] = libcrux_sha3_simd_portable_zero_d2();
  }
  memcpy(lit.data, repeat_expression, (size_t)25U * sizeof(uint64_t));
  return lit;
}

/**
A monomorphic instance of libcrux_sha3.traits.get_ij
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE const uint64_t *libcrux_sha3_traits_get_ij_71(
    const Eurydice_arr_7c *arr, size_t i, size_t j) {
  return &arr->data[(size_t)5U * j + i];
}

/**
A monomorphic instance of libcrux_sha3.traits.set_ij
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_traits_set_ij_71(Eurydice_arr_7c *arr,
                                                   size_t i, size_t j,
                                                   uint64_t value) {
  arr->data[(size_t)5U * j + i] = value;
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 72
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_block_c6(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start) {
  Eurydice_arr_7c state_flat = {.data = {0U}};
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U, .end = (size_t)72U / (size_t)8U}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      for (size_t i = (size_t)0U; i < (size_t)72U / (size_t)8U; i++) {
        size_t i0 = i;
        libcrux_sha3_traits_set_ij_71(
            state, i0 / (size_t)5U, i0 % (size_t)5U,
            libcrux_sha3_traits_get_ij_71(state, i0 / (size_t)5U,
                                          i0 % (size_t)5U)[0U] ^
                state_flat.data[i0]);
      }
      return;
    }
    size_t i = uu____0.f0;
    size_t offset = start + (size_t)8U * i;
    Eurydice_array_u8x8 arr;
    memcpy(arr.data,
           Eurydice_slice_subslice_shared_c8(
               blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = offset, .end = offset + (size_t)8U}))
               .ptr,
           (size_t)8U * sizeof(uint8_t));
    Eurydice_array_u8x8 uu____1 =
        core_result_unwrap_26_e0((KRML_CLITERAL(core_result_Result_8e){
            .tag = core_result_Ok, .val = {.case_Ok = arr}}));
    state_flat.data[i] = core_num__u64__from_le_bytes(uu____1);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 72
- DELIMITER= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_last_dc(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start,
    size_t len) {
  Eurydice_arr_ab buffer = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d4(
                          &buffer, (KRML_CLITERAL(core_ops_range_Range_87){
                                       .start = (size_t)0U, .end = len})),
                      Eurydice_slice_subslice_shared_c8(
                          blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                                      .start = start, .end = start + len})),
                      uint8_t);
  buffer.data[len] = 6U;
  size_t uu____0 = (size_t)72U - (size_t)1U;
  buffer.data[uu____0] = (uint32_t)buffer.data[uu____0] | 128U;
  libcrux_sha3_simd_portable_load_block_c6(
      state, Eurydice_array_to_slice_shared_e2(&buffer), (size_t)0U);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 72
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_dc(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len) {
  libcrux_sha3_simd_portable_load_last_dc(self, input->data[0U], start, len);
}

/**
This function found in impl {core::ops::index::Index<(usize, usize), T> for
libcrux_sha3::generic_keccak::KeccakState<T, N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.index_c2
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) const uint64_t *libcrux_sha3_generic_keccak_index_c2_71(
    const Eurydice_arr_7c *self, size_t_x2 index) {
  return libcrux_sha3_traits_get_ij_71(self, index.fst, index.snd);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.theta_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_84
libcrux_sha3_generic_keccak_theta_80_71(Eurydice_arr_7c *self) {
  Eurydice_arr_84 c = {
      .data = {libcrux_sha3_simd_portable_xor5_d2(
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                                       .snd = (size_t)0U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                                       .snd = (size_t)0U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                                       .snd = (size_t)0U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                                       .snd = (size_t)0U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){
                                 .fst = (size_t)4U, .snd = (size_t)0U}))[0U]),
               libcrux_sha3_simd_portable_xor5_d2(
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                                       .snd = (size_t)1U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                                       .snd = (size_t)1U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                                       .snd = (size_t)1U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                                       .snd = (size_t)1U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){
                                 .fst = (size_t)4U, .snd = (size_t)1U}))[0U]),
               libcrux_sha3_simd_portable_xor5_d2(
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                                       .snd = (size_t)2U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                                       .snd = (size_t)2U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                                       .snd = (size_t)2U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                                       .snd = (size_t)2U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){
                                 .fst = (size_t)4U, .snd = (size_t)2U}))[0U]),
               libcrux_sha3_simd_portable_xor5_d2(
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                                       .snd = (size_t)3U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                                       .snd = (size_t)3U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                                       .snd = (size_t)3U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                                       .snd = (size_t)3U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){
                                 .fst = (size_t)4U, .snd = (size_t)3U}))[0U]),
               libcrux_sha3_simd_portable_xor5_d2(
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                                       .snd = (size_t)4U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                                       .snd = (size_t)4U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                                       .snd = (size_t)4U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                                       .snd = (size_t)4U}))[0U],
                   libcrux_sha3_generic_keccak_index_c2_71(
                       self, (KRML_CLITERAL(size_t_x2){
                                 .fst = (size_t)4U, .snd = (size_t)4U}))[0U])}};
  return (KRML_CLITERAL(Eurydice_arr_84){
      .data = {libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(
                   c.data[((size_t)0U + (size_t)4U) % (size_t)5U],
                   c.data[((size_t)0U + (size_t)1U) % (size_t)5U]),
               libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(
                   c.data[((size_t)1U + (size_t)4U) % (size_t)5U],
                   c.data[((size_t)1U + (size_t)1U) % (size_t)5U]),
               libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(
                   c.data[((size_t)2U + (size_t)4U) % (size_t)5U],
                   c.data[((size_t)2U + (size_t)1U) % (size_t)5U]),
               libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(
                   c.data[((size_t)3U + (size_t)4U) % (size_t)5U],
                   c.data[((size_t)3U + (size_t)1U) % (size_t)5U]),
               libcrux_sha3_simd_portable_rotate_left1_and_xor_d2(
                   c.data[((size_t)4U + (size_t)4U) % (size_t)5U],
                   c.data[((size_t)4U + (size_t)1U) % (size_t)5U])}});
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.set_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) void libcrux_sha3_generic_keccak_set_80_71(Eurydice_arr_7c *self, size_t i,
                                           size_t j, uint64_t v) {
  libcrux_sha3_traits_set_ij_71(self, i, j, v);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 36
- RIGHT= 28
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_02(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)36);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 36
- RIGHT= 28
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_02(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_02(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 36
- RIGHT= 28
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_02(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_02(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 3
- RIGHT= 61
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_ac(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)3);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 3
- RIGHT= 61
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_ac(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_ac(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 3
- RIGHT= 61
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_ac(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_ac(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 41
- RIGHT= 23
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_rotate_left_020(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)41);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 41
- RIGHT= 23
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_020(uint64_t a,
                                                                   uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_020(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 41
- RIGHT= 23
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_020(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_020(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 18
- RIGHT= 46
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_a9(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)18);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 18
- RIGHT= 46
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_a9(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_a9(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 18
- RIGHT= 46
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_a9(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_a9(a, b);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_0_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_rho_0_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_84 t) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)0U,
      libcrux_sha3_simd_portable_xor_d2(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                              .snd = (size_t)0U}))[0U],
          t.data[0U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)0U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_02(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                              .snd = (size_t)0U}))[0U],
          t.data[0U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)0U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_ac(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                              .snd = (size_t)0U}))[0U],
          t.data[0U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)0U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_020(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                              .snd = (size_t)0U}))[0U],
          t.data[0U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)0U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_a9(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                              .snd = (size_t)0U}))[0U],
          t.data[0U]));
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 1
- RIGHT= 63
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_76(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_76(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 1
- RIGHT= 63
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_76(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_76(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 44
- RIGHT= 20
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_58(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)44);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 44
- RIGHT= 20
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_58(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_58(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 44
- RIGHT= 20
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_58(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_58(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 10
- RIGHT= 54
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_e0(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)10);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 10
- RIGHT= 54
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_e0(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_e0(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 10
- RIGHT= 54
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_e0(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_e0(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 45
- RIGHT= 19
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_63(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)45);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 45
- RIGHT= 19
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_63(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_63(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 45
- RIGHT= 19
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_63(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_63(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 2
- RIGHT= 62
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_6a(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)2);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 2
- RIGHT= 62
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_6a(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_6a(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 2
- RIGHT= 62
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_6a(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_6a(a, b);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_1_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_rho_1_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_84 t) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)1U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_76(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                              .snd = (size_t)1U}))[0U],
          t.data[1U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)1U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_58(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                              .snd = (size_t)1U}))[0U],
          t.data[1U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)1U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_e0(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                              .snd = (size_t)1U}))[0U],
          t.data[1U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)1U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_63(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                              .snd = (size_t)1U}))[0U],
          t.data[1U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)1U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_6a(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                              .snd = (size_t)1U}))[0U],
          t.data[1U]));
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 62
- RIGHT= 2
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_ab(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)62);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 62
- RIGHT= 2
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_ab(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_ab(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 62
- RIGHT= 2
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_ab(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_ab(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 6
- RIGHT= 58
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_5b(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)6);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 6
- RIGHT= 58
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_5b(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_5b(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 6
- RIGHT= 58
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_5b(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_5b(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 43
- RIGHT= 21
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_6f(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)43);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 43
- RIGHT= 21
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_6f(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_6f(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 43
- RIGHT= 21
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_6f(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_6f(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 15
- RIGHT= 49
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_62(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)15);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 15
- RIGHT= 49
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_62(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_62(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 15
- RIGHT= 49
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_62(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_62(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 61
- RIGHT= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_23(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)61);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 61
- RIGHT= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_23(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_23(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 61
- RIGHT= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_23(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_23(a, b);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_2_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_rho_2_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_84 t) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)2U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_ab(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                              .snd = (size_t)2U}))[0U],
          t.data[2U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)2U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_5b(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                              .snd = (size_t)2U}))[0U],
          t.data[2U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)2U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_6f(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                              .snd = (size_t)2U}))[0U],
          t.data[2U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)2U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_62(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                              .snd = (size_t)2U}))[0U],
          t.data[2U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)2U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_23(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                              .snd = (size_t)2U}))[0U],
          t.data[2U]));
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 28
- RIGHT= 36
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_37(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)28);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 28
- RIGHT= 36
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_37(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_37(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 28
- RIGHT= 36
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_37(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_37(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 55
- RIGHT= 9
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_bb(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)55);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 55
- RIGHT= 9
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_bb(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_bb(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 55
- RIGHT= 9
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_bb(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_bb(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 25
- RIGHT= 39
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_b9(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)25);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 25
- RIGHT= 39
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_b9(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_b9(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 25
- RIGHT= 39
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_b9(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_b9(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 21
- RIGHT= 43
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_54(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)21);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 21
- RIGHT= 43
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_54(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_54(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 21
- RIGHT= 43
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_54(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_54(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 56
- RIGHT= 8
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_4c(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)56);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 56
- RIGHT= 8
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_4c(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_4c(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 56
- RIGHT= 8
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_4c(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_4c(a, b);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_3_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_rho_3_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_84 t) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)3U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_37(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                              .snd = (size_t)3U}))[0U],
          t.data[3U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)3U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_bb(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                              .snd = (size_t)3U}))[0U],
          t.data[3U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)3U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_b9(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                              .snd = (size_t)3U}))[0U],
          t.data[3U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)3U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_54(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                              .snd = (size_t)3U}))[0U],
          t.data[3U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)3U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_4c(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                              .snd = (size_t)3U}))[0U],
          t.data[3U]));
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 27
- RIGHT= 37
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_ce(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)27);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 27
- RIGHT= 37
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_ce(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_ce(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 27
- RIGHT= 37
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_ce(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_ce(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 20
- RIGHT= 44
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_77(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)20);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 20
- RIGHT= 44
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_77(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_77(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 20
- RIGHT= 44
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_77(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_77(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 39
- RIGHT= 25
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_25(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)39);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 39
- RIGHT= 25
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_25(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_25(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 39
- RIGHT= 25
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_25(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_25(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 8
- RIGHT= 56
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_af(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)8);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 8
- RIGHT= 56
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_af(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_af(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 8
- RIGHT= 56
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_af(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_af(a, b);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.rotate_left
with const generics
- LEFT= 14
- RIGHT= 50
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable_rotate_left_fd(uint64_t x) {
  return core_num__u64__rotate_left(x, (uint32_t)14);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable._vxarq_u64
with const generics
- LEFT= 14
- RIGHT= 50
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t libcrux_sha3_simd_portable__vxarq_u64_fd(uint64_t a,
                                                                  uint64_t b) {
  return libcrux_sha3_simd_portable_rotate_left_fd(a ^ b);
}

/**
This function found in impl {libcrux_sha3::traits::KeccakItem<1usize> for u64}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.xor_and_rotate_d2
with const generics
- LEFT= 14
- RIGHT= 50
*/
static __attribute__((unused)) KRML_MUSTINLINE uint64_t
libcrux_sha3_simd_portable_xor_and_rotate_d2_fd(uint64_t a, uint64_t b) {
  return libcrux_sha3_simd_portable__vxarq_u64_fd(a, b);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_4_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_rho_4_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_84 t) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)4U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_ce(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                              .snd = (size_t)4U}))[0U],
          t.data[4U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)4U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_77(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                              .snd = (size_t)4U}))[0U],
          t.data[4U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)4U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_25(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                              .snd = (size_t)4U}))[0U],
          t.data[4U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)4U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_af(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                              .snd = (size_t)4U}))[0U],
          t.data[4U]));
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)4U,
      libcrux_sha3_simd_portable_xor_and_rotate_d2_fd(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                              .snd = (size_t)4U}))[0U],
          t.data[4U]));
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.rho_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_rho_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_84 t) {
  libcrux_sha3_generic_keccak_rho_0_80_71(self, t);
  libcrux_sha3_generic_keccak_rho_1_80_71(self, t);
  libcrux_sha3_generic_keccak_rho_2_80_71(self, t);
  libcrux_sha3_generic_keccak_rho_3_80_71(self, t);
  libcrux_sha3_generic_keccak_rho_4_80_71(self, t);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_0_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_pi_0_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_7c old) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)0U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                          .snd = (size_t)3U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)0U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                          .snd = (size_t)1U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)0U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                          .snd = (size_t)4U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)0U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                          .snd = (size_t)2U}))[0U]);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_1_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_pi_1_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_7c old) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)1U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                          .snd = (size_t)1U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)1U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                          .snd = (size_t)4U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)1U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                          .snd = (size_t)2U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)1U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                          .snd = (size_t)0U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)1U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)1U,
                                          .snd = (size_t)3U}))[0U]);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_2_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_pi_2_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_7c old) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)2U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                          .snd = (size_t)2U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)2U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                          .snd = (size_t)0U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)2U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                          .snd = (size_t)3U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)2U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                          .snd = (size_t)1U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)2U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)2U,
                                          .snd = (size_t)4U}))[0U]);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_3_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_pi_3_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_7c old) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)3U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                          .snd = (size_t)3U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)3U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                          .snd = (size_t)1U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)3U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                          .snd = (size_t)4U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)3U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                          .snd = (size_t)2U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)3U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)3U,
                                          .snd = (size_t)0U}))[0U]);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_4_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_pi_4_80_71(
    Eurydice_arr_7c *self, Eurydice_arr_7c old) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)4U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                          .snd = (size_t)4U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)1U, (size_t)4U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                          .snd = (size_t)2U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)2U, (size_t)4U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                          .snd = (size_t)0U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)3U, (size_t)4U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                          .snd = (size_t)3U}))[0U]);
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)4U, (size_t)4U,
      libcrux_sha3_generic_keccak_index_c2_71(
          &old, (KRML_CLITERAL(size_t_x2){.fst = (size_t)4U,
                                          .snd = (size_t)1U}))[0U]);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.pi_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_pi_80_71(
    Eurydice_arr_7c *self) {
  Eurydice_arr_7c old = self[0U];
  libcrux_sha3_generic_keccak_pi_0_80_71(self, old);
  libcrux_sha3_generic_keccak_pi_1_80_71(self, old);
  libcrux_sha3_generic_keccak_pi_2_80_71(self, old);
  libcrux_sha3_generic_keccak_pi_3_80_71(self, old);
  libcrux_sha3_generic_keccak_pi_4_80_71(self, old);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.chi_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_chi_80_71(
    Eurydice_arr_7c *self) {
  Eurydice_arr_7c old = self[0U];
  KRML_MAYBE_FOR5(
      i0, (size_t)0U, (size_t)5U, (size_t)1U, size_t i1 = i0; KRML_MAYBE_FOR5(
          i, (size_t)0U, (size_t)5U, (size_t)1U, size_t j = i;
          libcrux_sha3_generic_keccak_set_80_71(
              self, i1, j,
              libcrux_sha3_simd_portable_and_not_xor_d2(
                  libcrux_sha3_generic_keccak_index_c2_71(
                      self,
                      (KRML_CLITERAL(size_t_x2){.fst = i1, .snd = j}))[0U],
                  libcrux_sha3_generic_keccak_index_c2_71(
                      &old, (KRML_CLITERAL(size_t_x2){
                                .fst = i1,
                                .snd = (j + (size_t)2U) % (size_t)5U}))[0U],
                  libcrux_sha3_generic_keccak_index_c2_71(
                      &old,
                      (KRML_CLITERAL(size_t_x2){
                          .fst = i1,
                          .snd = (j + (size_t)1U) % (size_t)5U}))[0U]));););
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.iota_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_iota_80_71(
    Eurydice_arr_7c *self, size_t i) {
  libcrux_sha3_generic_keccak_set_80_71(
      self, (size_t)0U, (size_t)0U,
      libcrux_sha3_simd_portable_xor_constant_d2(
          libcrux_sha3_generic_keccak_index_c2_71(
              self, (KRML_CLITERAL(size_t_x2){.fst = (size_t)0U,
                                              .snd = (size_t)0U}))[0U],
          LIBCRUX_SHA3_GENERIC_KECCAK_CONSTANTS_ROUNDCONSTANTS.data[i]));
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.keccakf1600_80
with types uint64_t
with const generics
- N= 1
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_keccakf1600_80_71(
    Eurydice_arr_7c *self) {
  for (size_t i = (size_t)0U; i < (size_t)24U; i++) {
    size_t i0 = i;
    Eurydice_arr_84 t = libcrux_sha3_generic_keccak_theta_80_71(self);
    libcrux_sha3_generic_keccak_rho_80_71(self, t);
    libcrux_sha3_generic_keccak_pi_80_71(self);
    libcrux_sha3_generic_keccak_chi_80_71(self);
    libcrux_sha3_generic_keccak_iota_80_71(self, i0);
  }
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 72
- DELIM= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_final_80_bd(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len) {
  libcrux_sha3_simd_portable_load_last_a1_dc(self, input, start, len);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 72
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_store_block_c6(
    const Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out, size_t start,
    size_t len) {
  size_t octets = len / (size_t)8U;
  for (size_t i = (size_t)0U; i < octets; i++) {
    size_t i0 = i;
    Eurydice_array_u8x8 bytes = core_num__u64__to_le_bytes(
        libcrux_sha3_traits_get_ij_71(s, i0 / (size_t)5U, i0 % (size_t)5U)[0U]);
    size_t out_pos = start + (size_t)8U * i0;
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            out, (KRML_CLITERAL(core_ops_range_Range_87){
                     .start = out_pos, .end = out_pos + (size_t)8U})),
        Eurydice_array_to_slice_shared_6e(&bytes), uint8_t);
  }
  size_t remaining = len % (size_t)8U;
  if (remaining > (size_t)0U) {
    Eurydice_array_u8x8 bytes =
        core_num__u64__to_le_bytes(libcrux_sha3_traits_get_ij_71(
            s, octets / (size_t)5U, octets % (size_t)5U)[0U]);
    size_t out_pos = start + len - remaining;
    Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
        out, (KRML_CLITERAL(core_ops_range_Range_87){
                 .start = out_pos, .end = out_pos + remaining}));
    Eurydice_slice_copy(
        uu____0, Eurydice_array_to_subslice_to_shared_21(&bytes, remaining),
        uint8_t);
  }
}

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_c6(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len) {
  libcrux_sha3_simd_portable_store_block_c6(self, out, start, len);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 72
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_c6(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start) {
  libcrux_sha3_simd_portable_load_block_c6(self, input->data[0U], start);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 72
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_block_80_e9(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start) {
  libcrux_sha3_simd_portable_load_block_a1_c6(self, input, start);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 72
- DELIM= 6
*/
static __attribute__((unused)) inline void libcrux_sha3_generic_keccak_portable_keccak1_dc(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output) {
  Eurydice_arr_7c s = libcrux_sha3_generic_keccak_new_80_71();
  size_t input_len = input.meta;
  size_t input_blocks = input_len / (size_t)72U;
  size_t input_rem = input_len % (size_t)72U;
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = input_blocks}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_dc lvalue = {.data = {input}};
      libcrux_sha3_generic_keccak_absorb_final_80_bd(
          &s, &lvalue, input_len - input_rem, input_rem);
      size_t output_len = output.meta;
      size_t output_blocks = output_len / (size_t)72U;
      size_t output_rem = output_len % (size_t)72U;
      if (output_blocks == (size_t)0U) {
        libcrux_sha3_simd_portable_squeeze_9b_c6(&s, output, (size_t)0U,
                                                 output_len);
      } else {
        libcrux_sha3_simd_portable_squeeze_9b_c6(&s, output, (size_t)0U,
                                                 (size_t)72U);
        for (size_t i = (size_t)1U; i < output_blocks; i++) {
          size_t i0 = i;
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_c6(&s, output, i0 * (size_t)72U,
                                                   (size_t)72U);
        }
        if (output_rem != (size_t)0U) {
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_c6(
              &s, output, output_len - output_rem, output_rem);
        }
      }
      return;
    }
    size_t i = uu____0.f0;
    /* original Rust expression is not an lvalue in C */
    Eurydice_arr_dc lvalue = {.data = {input}};
    libcrux_sha3_generic_keccak_absorb_block_80_e9(&s, &lvalue,
                                                   i * (size_t)72U);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
 A portable SHA3 512 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_sha512(Eurydice_mut_borrow_slice_u8 digest,
                                  Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_generic_keccak_portable_keccak1_dc(data, digest);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 136
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_block_b2(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start) {
  Eurydice_arr_7c state_flat = {.data = {0U}};
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U, .end = (size_t)136U / (size_t)8U}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      for (size_t i = (size_t)0U; i < (size_t)136U / (size_t)8U; i++) {
        size_t i0 = i;
        libcrux_sha3_traits_set_ij_71(
            state, i0 / (size_t)5U, i0 % (size_t)5U,
            libcrux_sha3_traits_get_ij_71(state, i0 / (size_t)5U,
                                          i0 % (size_t)5U)[0U] ^
                state_flat.data[i0]);
      }
      return;
    }
    size_t i = uu____0.f0;
    size_t offset = start + (size_t)8U * i;
    Eurydice_array_u8x8 arr;
    memcpy(arr.data,
           Eurydice_slice_subslice_shared_c8(
               blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = offset, .end = offset + (size_t)8U}))
               .ptr,
           (size_t)8U * sizeof(uint8_t));
    Eurydice_array_u8x8 uu____1 =
        core_result_unwrap_26_e0((KRML_CLITERAL(core_result_Result_8e){
            .tag = core_result_Ok, .val = {.case_Ok = arr}}));
    state_flat.data[i] = core_num__u64__from_le_bytes(uu____1);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 136
- DELIMITER= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_last_22(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start,
    size_t len) {
  Eurydice_arr_ff buffer = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d40(
                          &buffer, (KRML_CLITERAL(core_ops_range_Range_87){
                                       .start = (size_t)0U, .end = len})),
                      Eurydice_slice_subslice_shared_c8(
                          blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                                      .start = start, .end = start + len})),
                      uint8_t);
  buffer.data[len] = 6U;
  size_t uu____0 = (size_t)136U - (size_t)1U;
  buffer.data[uu____0] = (uint32_t)buffer.data[uu____0] | 128U;
  libcrux_sha3_simd_portable_load_block_b2(
      state, Eurydice_array_to_slice_shared_58(&buffer), (size_t)0U);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 136
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_22(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len) {
  libcrux_sha3_simd_portable_load_last_22(self, input->data[0U], start, len);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 136
- DELIM= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_final_80_bd0(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len) {
  libcrux_sha3_simd_portable_load_last_a1_22(self, input, start, len);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 136
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_store_block_b2(
    const Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out, size_t start,
    size_t len) {
  size_t octets = len / (size_t)8U;
  for (size_t i = (size_t)0U; i < octets; i++) {
    size_t i0 = i;
    Eurydice_array_u8x8 bytes = core_num__u64__to_le_bytes(
        libcrux_sha3_traits_get_ij_71(s, i0 / (size_t)5U, i0 % (size_t)5U)[0U]);
    size_t out_pos = start + (size_t)8U * i0;
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            out, (KRML_CLITERAL(core_ops_range_Range_87){
                     .start = out_pos, .end = out_pos + (size_t)8U})),
        Eurydice_array_to_slice_shared_6e(&bytes), uint8_t);
  }
  size_t remaining = len % (size_t)8U;
  if (remaining > (size_t)0U) {
    Eurydice_array_u8x8 bytes =
        core_num__u64__to_le_bytes(libcrux_sha3_traits_get_ij_71(
            s, octets / (size_t)5U, octets % (size_t)5U)[0U]);
    size_t out_pos = start + len - remaining;
    Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
        out, (KRML_CLITERAL(core_ops_range_Range_87){
                 .start = out_pos, .end = out_pos + remaining}));
    Eurydice_slice_copy(
        uu____0, Eurydice_array_to_subslice_to_shared_21(&bytes, remaining),
        uint8_t);
  }
}

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_b2(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len) {
  libcrux_sha3_simd_portable_store_block_b2(self, out, start, len);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 136
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_b2(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start) {
  libcrux_sha3_simd_portable_load_block_b2(self, input->data[0U], start);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 136
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_block_80_e90(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start) {
  libcrux_sha3_simd_portable_load_block_a1_b2(self, input, start);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 136
- DELIM= 6
*/
static __attribute__((unused)) inline void libcrux_sha3_generic_keccak_portable_keccak1_22(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output) {
  Eurydice_arr_7c s = libcrux_sha3_generic_keccak_new_80_71();
  size_t input_len = input.meta;
  size_t input_blocks = input_len / (size_t)136U;
  size_t input_rem = input_len % (size_t)136U;
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = input_blocks}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_dc lvalue = {.data = {input}};
      libcrux_sha3_generic_keccak_absorb_final_80_bd0(
          &s, &lvalue, input_len - input_rem, input_rem);
      size_t output_len = output.meta;
      size_t output_blocks = output_len / (size_t)136U;
      size_t output_rem = output_len % (size_t)136U;
      if (output_blocks == (size_t)0U) {
        libcrux_sha3_simd_portable_squeeze_9b_b2(&s, output, (size_t)0U,
                                                 output_len);
      } else {
        libcrux_sha3_simd_portable_squeeze_9b_b2(&s, output, (size_t)0U,
                                                 (size_t)136U);
        for (size_t i = (size_t)1U; i < output_blocks; i++) {
          size_t i0 = i;
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_b2(
              &s, output, i0 * (size_t)136U, (size_t)136U);
        }
        if (output_rem != (size_t)0U) {
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_b2(
              &s, output, output_len - output_rem, output_rem);
        }
      }
      return;
    }
    size_t i = uu____0.f0;
    /* original Rust expression is not an lvalue in C */
    Eurydice_arr_dc lvalue = {.data = {input}};
    libcrux_sha3_generic_keccak_absorb_block_80_e90(&s, &lvalue,
                                                    i * (size_t)136U);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
 A portable SHA3 256 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_sha256(Eurydice_mut_borrow_slice_u8 digest,
                                  Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_generic_keccak_portable_keccak1_22(data, digest);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 136
- DELIMITER= 31
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_last_220(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start,
    size_t len) {
  Eurydice_arr_ff buffer = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d40(
                          &buffer, (KRML_CLITERAL(core_ops_range_Range_87){
                                       .start = (size_t)0U, .end = len})),
                      Eurydice_slice_subslice_shared_c8(
                          blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                                      .start = start, .end = start + len})),
                      uint8_t);
  buffer.data[len] = 31U;
  size_t uu____0 = (size_t)136U - (size_t)1U;
  buffer.data[uu____0] = (uint32_t)buffer.data[uu____0] | 128U;
  libcrux_sha3_simd_portable_load_block_b2(
      state, Eurydice_array_to_slice_shared_58(&buffer), (size_t)0U);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 136
- DELIMITER= 31
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_220(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start, size_t len) {
  libcrux_sha3_simd_portable_load_last_220(self, input->data[0U], start, len);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 136
- DELIM= 31
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_final_80_bd1(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len) {
  libcrux_sha3_simd_portable_load_last_a1_220(self, input, start, len);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 136
- DELIM= 31
*/
static __attribute__((unused)) inline void libcrux_sha3_generic_keccak_portable_keccak1_220(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output) {
  Eurydice_arr_7c s = libcrux_sha3_generic_keccak_new_80_71();
  size_t input_len = input.meta;
  size_t input_blocks = input_len / (size_t)136U;
  size_t input_rem = input_len % (size_t)136U;
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = input_blocks}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_dc lvalue = {.data = {input}};
      libcrux_sha3_generic_keccak_absorb_final_80_bd1(
          &s, &lvalue, input_len - input_rem, input_rem);
      size_t output_len = output.meta;
      size_t output_blocks = output_len / (size_t)136U;
      size_t output_rem = output_len % (size_t)136U;
      if (output_blocks == (size_t)0U) {
        libcrux_sha3_simd_portable_squeeze_9b_b2(&s, output, (size_t)0U,
                                                 output_len);
      } else {
        libcrux_sha3_simd_portable_squeeze_9b_b2(&s, output, (size_t)0U,
                                                 (size_t)136U);
        for (size_t i = (size_t)1U; i < output_blocks; i++) {
          size_t i0 = i;
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_b2(
              &s, output, i0 * (size_t)136U, (size_t)136U);
        }
        if (output_rem != (size_t)0U) {
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_b2(
              &s, output, output_len - output_rem, output_rem);
        }
      }
      return;
    }
    size_t i = uu____0.f0;
    /* original Rust expression is not an lvalue in C */
    Eurydice_arr_dc lvalue = {.data = {input}};
    libcrux_sha3_generic_keccak_absorb_block_80_e90(&s, &lvalue,
                                                    i * (size_t)136U);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
 A portable SHAKE256 implementation.
*/
static __attribute__((unused)) void libcrux_sha3_portable_shake256(Eurydice_mut_borrow_slice_u8 digest,
                                    Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_generic_keccak_portable_keccak1_220(data, digest);
}

/**
 Create a new SHAKE-128 state object.
*/
static __attribute__((unused)) Eurydice_arr_7c libcrux_sha3_portable_incremental_shake128_init(void) {
  return libcrux_sha3_generic_keccak_new_80_71();
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_block_60(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start) {
  Eurydice_arr_7c state_flat = {.data = {0U}};
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U, .end = (size_t)168U / (size_t)8U}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      for (size_t i = (size_t)0U; i < (size_t)168U / (size_t)8U; i++) {
        size_t i0 = i;
        libcrux_sha3_traits_set_ij_71(
            state, i0 / (size_t)5U, i0 % (size_t)5U,
            libcrux_sha3_traits_get_ij_71(state, i0 / (size_t)5U,
                                          i0 % (size_t)5U)[0U] ^
                state_flat.data[i0]);
      }
      return;
    }
    size_t i = uu____0.f0;
    size_t offset = start + (size_t)8U * i;
    Eurydice_array_u8x8 arr;
    memcpy(arr.data,
           Eurydice_slice_subslice_shared_c8(
               blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = offset, .end = offset + (size_t)8U}))
               .ptr,
           (size_t)8U * sizeof(uint8_t));
    Eurydice_array_u8x8 uu____1 =
        core_result_unwrap_26_e0((KRML_CLITERAL(core_result_Result_8e){
            .tag = core_result_Ok, .val = {.case_Ok = arr}}));
    state_flat.data[i] = core_num__u64__from_le_bytes(uu____1);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 168
- DELIMITER= 31
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_last_37(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start,
    size_t len) {
  Eurydice_arr_c5 buffer = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d41(
                          &buffer, (KRML_CLITERAL(core_ops_range_Range_87){
                                       .start = (size_t)0U, .end = len})),
                      Eurydice_slice_subslice_shared_c8(
                          blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                                      .start = start, .end = start + len})),
                      uint8_t);
  buffer.data[len] = 31U;
  size_t uu____0 = (size_t)168U - (size_t)1U;
  buffer.data[uu____0] = (uint32_t)buffer.data[uu____0] | 128U;
  libcrux_sha3_simd_portable_load_block_60(
      state, Eurydice_array_to_slice_shared_2c(&buffer), (size_t)0U);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 168
- DELIMITER= 31
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_37(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len) {
  libcrux_sha3_simd_portable_load_last_37(self, input->data[0U], start, len);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 168
- DELIM= 31
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_final_80_bd2(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len) {
  libcrux_sha3_simd_portable_load_last_a1_37(self, input, start, len);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
 Absorb
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_absorb_final(
    Eurydice_arr_7c *s, Eurydice_borrow_slice_u8 data0) {
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_dc lvalue = {.data = {data0}};
  libcrux_sha3_generic_keccak_absorb_final_80_bd2(s, &lvalue, (size_t)0U,
                                                  data0.meta);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_store_block_60(
    const Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out, size_t start,
    size_t len) {
  size_t octets = len / (size_t)8U;
  for (size_t i = (size_t)0U; i < octets; i++) {
    size_t i0 = i;
    Eurydice_array_u8x8 bytes = core_num__u64__to_le_bytes(
        libcrux_sha3_traits_get_ij_71(s, i0 / (size_t)5U, i0 % (size_t)5U)[0U]);
    size_t out_pos = start + (size_t)8U * i0;
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            out, (KRML_CLITERAL(core_ops_range_Range_87){
                     .start = out_pos, .end = out_pos + (size_t)8U})),
        Eurydice_array_to_slice_shared_6e(&bytes), uint8_t);
  }
  size_t remaining = len % (size_t)8U;
  if (remaining > (size_t)0U) {
    Eurydice_array_u8x8 bytes =
        core_num__u64__to_le_bytes(libcrux_sha3_traits_get_ij_71(
            s, octets / (size_t)5U, octets % (size_t)5U)[0U]);
    size_t out_pos = start + len - remaining;
    Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
        out, (KRML_CLITERAL(core_ops_range_Range_87){
                 .start = out_pos, .end = out_pos + remaining}));
    Eurydice_slice_copy(
        uu____0, Eurydice_array_to_subslice_to_shared_21(&bytes, remaining),
        uint8_t);
  }
}

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_60(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len) {
  libcrux_sha3_simd_portable_store_block_60(self, out, start, len);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_first_three_blocks_b4 with const
generics
- RATE= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_generic_keccak_portable_squeeze_first_three_blocks_b4_60(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out) {
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)0U, (size_t)168U);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)168U,
                                           (size_t)168U);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)2U * (size_t)168U,
                                           (size_t)168U);
}

/**
 Squeeze three blocks
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_squeeze_first_three_blocks(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out0) {
  libcrux_sha3_generic_keccak_portable_squeeze_first_three_blocks_b4_60(s,
                                                                        out0);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_next_block_b4 with const generics
- RATE= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_generic_keccak_portable_squeeze_next_block_b4_60(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out, size_t start) {
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, start, (size_t)168U);
}

/**
 Squeeze another block
*/
static __attribute__((unused)) void libcrux_sha3_portable_incremental_shake128_squeeze_next_block(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out0) {
  libcrux_sha3_generic_keccak_portable_squeeze_next_block_b4_60(s, out0,
                                                                (size_t)0U);
}

/**
 Returns the output size of a digest.
*/
static __attribute__((unused)) size_t libcrux_sha3_digest_size(libcrux_sha3_Algorithm mode) {
  switch (mode) {
    case libcrux_sha3_Algorithm_Sha224: {
      break;
    }
    case libcrux_sha3_Algorithm_Sha256: {
      return LIBCRUX_SHA3_SHA3_256_DIGEST_SIZE;
    }
    case libcrux_sha3_Algorithm_Sha384: {
      return LIBCRUX_SHA3_SHA3_384_DIGEST_SIZE;
    }
    case libcrux_sha3_Algorithm_Sha512: {
      return LIBCRUX_SHA3_SHA3_512_DIGEST_SIZE;
    }
    default: {
      KRML_HOST_EPRINTF("KaRaMeL incomplete match at %s:%d\n", __FILE__,
                        __LINE__);
      KRML_HOST_EXIT(253U);
    }
  }
  return LIBCRUX_SHA3_SHA3_224_DIGEST_SIZE;
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 144
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_block_9e(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start) {
  Eurydice_arr_7c state_flat = {.data = {0U}};
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U, .end = (size_t)144U / (size_t)8U}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      for (size_t i = (size_t)0U; i < (size_t)144U / (size_t)8U; i++) {
        size_t i0 = i;
        libcrux_sha3_traits_set_ij_71(
            state, i0 / (size_t)5U, i0 % (size_t)5U,
            libcrux_sha3_traits_get_ij_71(state, i0 / (size_t)5U,
                                          i0 % (size_t)5U)[0U] ^
                state_flat.data[i0]);
      }
      return;
    }
    size_t i = uu____0.f0;
    size_t offset = start + (size_t)8U * i;
    Eurydice_array_u8x8 arr;
    memcpy(arr.data,
           Eurydice_slice_subslice_shared_c8(
               blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = offset, .end = offset + (size_t)8U}))
               .ptr,
           (size_t)8U * sizeof(uint8_t));
    Eurydice_array_u8x8 uu____1 =
        core_result_unwrap_26_e0((KRML_CLITERAL(core_result_Result_8e){
            .tag = core_result_Ok, .val = {.case_Ok = arr}}));
    state_flat.data[i] = core_num__u64__from_le_bytes(uu____1);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 144
- DELIMITER= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_last_3a(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start,
    size_t len) {
  Eurydice_arr_f4 buffer = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d42(
                          &buffer, (KRML_CLITERAL(core_ops_range_Range_87){
                                       .start = (size_t)0U, .end = len})),
                      Eurydice_slice_subslice_shared_c8(
                          blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                                      .start = start, .end = start + len})),
                      uint8_t);
  buffer.data[len] = 6U;
  size_t uu____0 = (size_t)144U - (size_t)1U;
  buffer.data[uu____0] = (uint32_t)buffer.data[uu____0] | 128U;
  libcrux_sha3_simd_portable_load_block_9e(
      state, Eurydice_array_to_slice_shared_38(&buffer), (size_t)0U);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 144
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_3a(Eurydice_arr_7c *self,
                                                const Eurydice_arr_dc *input,
                                                size_t start, size_t len) {
  libcrux_sha3_simd_portable_load_last_3a(self, input->data[0U], start, len);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 144
- DELIM= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_final_80_bd3(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len) {
  libcrux_sha3_simd_portable_load_last_a1_3a(self, input, start, len);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 144
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_store_block_9e(
    const Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out, size_t start,
    size_t len) {
  size_t octets = len / (size_t)8U;
  for (size_t i = (size_t)0U; i < octets; i++) {
    size_t i0 = i;
    Eurydice_array_u8x8 bytes = core_num__u64__to_le_bytes(
        libcrux_sha3_traits_get_ij_71(s, i0 / (size_t)5U, i0 % (size_t)5U)[0U]);
    size_t out_pos = start + (size_t)8U * i0;
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            out, (KRML_CLITERAL(core_ops_range_Range_87){
                     .start = out_pos, .end = out_pos + (size_t)8U})),
        Eurydice_array_to_slice_shared_6e(&bytes), uint8_t);
  }
  size_t remaining = len % (size_t)8U;
  if (remaining > (size_t)0U) {
    Eurydice_array_u8x8 bytes =
        core_num__u64__to_le_bytes(libcrux_sha3_traits_get_ij_71(
            s, octets / (size_t)5U, octets % (size_t)5U)[0U]);
    size_t out_pos = start + len - remaining;
    Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
        out, (KRML_CLITERAL(core_ops_range_Range_87){
                 .start = out_pos, .end = out_pos + remaining}));
    Eurydice_slice_copy(
        uu____0, Eurydice_array_to_subslice_to_shared_21(&bytes, remaining),
        uint8_t);
  }
}

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_9e(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len) {
  libcrux_sha3_simd_portable_store_block_9e(self, out, start, len);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 144
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_9e(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start) {
  libcrux_sha3_simd_portable_load_block_9e(self, input->data[0U], start);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 144
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_block_80_e91(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start) {
  libcrux_sha3_simd_portable_load_block_a1_9e(self, input, start);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 144
- DELIM= 6
*/
static __attribute__((unused)) inline void libcrux_sha3_generic_keccak_portable_keccak1_3a(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output) {
  Eurydice_arr_7c s = libcrux_sha3_generic_keccak_new_80_71();
  size_t input_len = input.meta;
  size_t input_blocks = input_len / (size_t)144U;
  size_t input_rem = input_len % (size_t)144U;
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = input_blocks}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_dc lvalue = {.data = {input}};
      libcrux_sha3_generic_keccak_absorb_final_80_bd3(
          &s, &lvalue, input_len - input_rem, input_rem);
      size_t output_len = output.meta;
      size_t output_blocks = output_len / (size_t)144U;
      size_t output_rem = output_len % (size_t)144U;
      if (output_blocks == (size_t)0U) {
        libcrux_sha3_simd_portable_squeeze_9b_9e(&s, output, (size_t)0U,
                                                 output_len);
      } else {
        libcrux_sha3_simd_portable_squeeze_9b_9e(&s, output, (size_t)0U,
                                                 (size_t)144U);
        for (size_t i = (size_t)1U; i < output_blocks; i++) {
          size_t i0 = i;
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_9e(
              &s, output, i0 * (size_t)144U, (size_t)144U);
        }
        if (output_rem != (size_t)0U) {
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_9e(
              &s, output, output_len - output_rem, output_rem);
        }
      }
      return;
    }
    size_t i = uu____0.f0;
    /* original Rust expression is not an lvalue in C */
    Eurydice_arr_dc lvalue = {.data = {input}};
    libcrux_sha3_generic_keccak_absorb_block_80_e91(&s, &lvalue,
                                                    i * (size_t)144U);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
 A portable SHA3 224 implementation.
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_portable_sha224(
    Eurydice_mut_borrow_slice_u8 digest, Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_generic_keccak_portable_keccak1_3a(data, digest);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block
with const generics
- RATE= 104
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_block_53(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start) {
  Eurydice_arr_7c state_flat = {.data = {0U}};
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U, .end = (size_t)104U / (size_t)8U}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      for (size_t i = (size_t)0U; i < (size_t)104U / (size_t)8U; i++) {
        size_t i0 = i;
        libcrux_sha3_traits_set_ij_71(
            state, i0 / (size_t)5U, i0 % (size_t)5U,
            libcrux_sha3_traits_get_ij_71(state, i0 / (size_t)5U,
                                          i0 % (size_t)5U)[0U] ^
                state_flat.data[i0]);
      }
      return;
    }
    size_t i = uu____0.f0;
    size_t offset = start + (size_t)8U * i;
    Eurydice_array_u8x8 arr;
    memcpy(arr.data,
           Eurydice_slice_subslice_shared_c8(
               blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = offset, .end = offset + (size_t)8U}))
               .ptr,
           (size_t)8U * sizeof(uint8_t));
    Eurydice_array_u8x8 uu____1 =
        core_result_unwrap_26_e0((KRML_CLITERAL(core_result_Result_8e){
            .tag = core_result_Ok, .val = {.case_Ok = arr}}));
    state_flat.data[i] = core_num__u64__from_le_bytes(uu____1);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last
with const generics
- RATE= 104
- DELIMITER= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_load_last_dc0(
    Eurydice_arr_7c *state, Eurydice_borrow_slice_u8 blocks, size_t start,
    size_t len) {
  Eurydice_arr_c4 buffer = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d43(
                          &buffer, (KRML_CLITERAL(core_ops_range_Range_87){
                                       .start = (size_t)0U, .end = len})),
                      Eurydice_slice_subslice_shared_c8(
                          blocks, (KRML_CLITERAL(core_ops_range_Range_87){
                                      .start = start, .end = start + len})),
                      uint8_t);
  buffer.data[len] = 6U;
  size_t uu____0 = (size_t)104U - (size_t)1U;
  buffer.data[uu____0] = (uint32_t)buffer.data[uu____0] | 128U;
  libcrux_sha3_simd_portable_load_block_53(
      state, Eurydice_array_to_slice_shared_72(&buffer), (size_t)0U);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_last_a1
with const generics
- RATE= 104
- DELIMITER= 6
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_last_a1_dc0(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start, size_t len) {
  libcrux_sha3_simd_portable_load_last_dc0(self, input->data[0U], start, len);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_final_80
with types uint64_t
with const generics
- N= 1
- RATE= 104
- DELIM= 6
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_final_80_bd4(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start,
    size_t len) {
  libcrux_sha3_simd_portable_load_last_a1_dc0(self, input, start, len);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.simd.portable.store_block
with const generics
- RATE= 104
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_simd_portable_store_block_53(
    const Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out, size_t start,
    size_t len) {
  size_t octets = len / (size_t)8U;
  for (size_t i = (size_t)0U; i < octets; i++) {
    size_t i0 = i;
    Eurydice_array_u8x8 bytes = core_num__u64__to_le_bytes(
        libcrux_sha3_traits_get_ij_71(s, i0 / (size_t)5U, i0 % (size_t)5U)[0U]);
    size_t out_pos = start + (size_t)8U * i0;
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            out, (KRML_CLITERAL(core_ops_range_Range_87){
                     .start = out_pos, .end = out_pos + (size_t)8U})),
        Eurydice_array_to_slice_shared_6e(&bytes), uint8_t);
  }
  size_t remaining = len % (size_t)8U;
  if (remaining > (size_t)0U) {
    Eurydice_array_u8x8 bytes =
        core_num__u64__to_le_bytes(libcrux_sha3_traits_get_ij_71(
            s, octets / (size_t)5U, octets % (size_t)5U)[0U]);
    size_t out_pos = start + len - remaining;
    Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
        out, (KRML_CLITERAL(core_ops_range_Range_87){
                 .start = out_pos, .end = out_pos + remaining}));
    Eurydice_slice_copy(
        uu____0, Eurydice_array_to_subslice_to_shared_21(&bytes, remaining),
        uint8_t);
  }
}

/**
This function found in impl {libcrux_sha3::traits::Squeeze<u64> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.squeeze_9b
with const generics
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_squeeze_9b_53(const Eurydice_arr_7c *self,
                                              Eurydice_mut_borrow_slice_u8 out,
                                              size_t start, size_t len) {
  libcrux_sha3_simd_portable_store_block_53(self, out, start, len);
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 104
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_53(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start) {
  libcrux_sha3_simd_portable_load_block_53(self, input->data[0U], start);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 104
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_block_80_e92(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start) {
  libcrux_sha3_simd_portable_load_block_a1_53(self, input, start);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 104
- DELIM= 6
*/
static __attribute__((unused)) inline void libcrux_sha3_generic_keccak_portable_keccak1_dc0(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output) {
  Eurydice_arr_7c s = libcrux_sha3_generic_keccak_new_80_71();
  size_t input_len = input.meta;
  size_t input_blocks = input_len / (size_t)104U;
  size_t input_rem = input_len % (size_t)104U;
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = input_blocks}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_dc lvalue = {.data = {input}};
      libcrux_sha3_generic_keccak_absorb_final_80_bd4(
          &s, &lvalue, input_len - input_rem, input_rem);
      size_t output_len = output.meta;
      size_t output_blocks = output_len / (size_t)104U;
      size_t output_rem = output_len % (size_t)104U;
      if (output_blocks == (size_t)0U) {
        libcrux_sha3_simd_portable_squeeze_9b_53(&s, output, (size_t)0U,
                                                 output_len);
      } else {
        libcrux_sha3_simd_portable_squeeze_9b_53(&s, output, (size_t)0U,
                                                 (size_t)104U);
        for (size_t i = (size_t)1U; i < output_blocks; i++) {
          size_t i0 = i;
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_53(
              &s, output, i0 * (size_t)104U, (size_t)104U);
        }
        if (output_rem != (size_t)0U) {
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_53(
              &s, output, output_len - output_rem, output_rem);
        }
      }
      return;
    }
    size_t i = uu____0.f0;
    /* original Rust expression is not an lvalue in C */
    Eurydice_arr_dc lvalue = {.data = {input}};
    libcrux_sha3_generic_keccak_absorb_block_80_e92(&s, &lvalue,
                                                    i * (size_t)104U);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
 A portable SHA3 384 implementation.
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_portable_sha384(
    Eurydice_mut_borrow_slice_u8 digest, Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_generic_keccak_portable_keccak1_dc0(data, digest);
}

/**
 SHA3 224

 Preconditions:
 - `digest.len() == 28`
*/
static __attribute__((unused)) void libcrux_sha3_sha224_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload) {
  libcrux_sha3_portable_sha224(digest, payload);
}

/**
 SHA3 224
*/
static __attribute__((unused)) Eurydice_arr_a2 libcrux_sha3_sha224(Eurydice_borrow_slice_u8 data) {
  Eurydice_arr_a2 out = {.data = {0U}};
  libcrux_sha3_sha224_ema(Eurydice_array_to_slice_mut_5e(&out), data);
  return out;
}

/**
 SHA3 256
*/
static __attribute__((unused)) void libcrux_sha3_sha256_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload) {
  libcrux_sha3_portable_sha256(digest, payload);
}

/**
 SHA3 256
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_sha3_sha256(Eurydice_borrow_slice_u8 data) {
  Eurydice_arr_ec out = {.data = {0U}};
  libcrux_sha3_sha256_ema(Eurydice_array_to_slice_mut_01(&out), data);
  return out;
}

/**
 SHA3 384
*/
static __attribute__((unused)) void libcrux_sha3_sha384_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload) {
  libcrux_sha3_portable_sha384(digest, payload);
}

/**
 SHA3 384
*/
static __attribute__((unused)) Eurydice_arr_65 libcrux_sha3_sha384(Eurydice_borrow_slice_u8 data) {
  Eurydice_arr_65 out = {.data = {0U}};
  libcrux_sha3_sha384_ema(Eurydice_array_to_slice_mut_9f(&out), data);
  return out;
}

/**
 SHA3 512
*/
static __attribute__((unused)) void libcrux_sha3_sha512_ema(Eurydice_mut_borrow_slice_u8 digest,
                             Eurydice_borrow_slice_u8 payload) {
  libcrux_sha3_portable_sha512(digest, payload);
}

/**
 SHA3 512
*/
static __attribute__((unused)) Eurydice_arr_c7 libcrux_sha3_sha512(Eurydice_borrow_slice_u8 data) {
  Eurydice_arr_c7 out = {.data = {0U}};
  libcrux_sha3_sha512_ema(Eurydice_array_to_slice_mut_17(&out), data);
  return out;
}

/**
This function found in impl {libcrux_sha3::traits::Absorb<1usize> for
libcrux_sha3::generic_keccak::KeccakState<u64, 1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of libcrux_sha3.simd.portable.load_block_a1
with const generics
- RATE= 168
*/
static __attribute__((unused)) void libcrux_sha3_simd_portable_load_block_a1_60(Eurydice_arr_7c *self,
                                                 const Eurydice_arr_dc *input,
                                                 size_t start) {
  libcrux_sha3_simd_portable_load_block_60(self, input->data[0U], start);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<T,
N>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_sha3.generic_keccak.absorb_block_80
with types uint64_t
with const generics
- N= 1
- RATE= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_generic_keccak_absorb_block_80_e93(
    Eurydice_arr_7c *self, const Eurydice_arr_dc *input, size_t start) {
  libcrux_sha3_simd_portable_load_block_a1_60(self, input, start);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
}

/**
A monomorphic instance of libcrux_sha3.generic_keccak.portable.keccak1
with const generics
- RATE= 168
- DELIM= 31
*/
static __attribute__((unused)) inline void libcrux_sha3_generic_keccak_portable_keccak1_37(
    Eurydice_borrow_slice_u8 input, Eurydice_mut_borrow_slice_u8 output) {
  Eurydice_arr_7c s = libcrux_sha3_generic_keccak_new_80_71();
  size_t input_len = input.meta;
  size_t input_blocks = input_len / (size_t)168U;
  size_t input_rem = input_len % (size_t)168U;
  core_ops_range_Range_87 iter =
      core_iter_traits_collect__core__iter__traits__collect__IntoIterator_Clause1_Item__I__for_I__into_iter(
          (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = input_blocks}),
          core_ops_range_Range_87, size_t, core_ops_range_Range_87);
  while (true) {
    core_option_Option_87 uu____0 =
        core_iter_range__core__iter__traits__iterator__Iterator_A__for_core__ops__range__Range_A__TraitClause_0___next(
            &iter, size_t, core_option_Option_87);
    if (uu____0.tag == core_option_None) {
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_dc lvalue = {.data = {input}};
      libcrux_sha3_generic_keccak_absorb_final_80_bd2(
          &s, &lvalue, input_len - input_rem, input_rem);
      size_t output_len = output.meta;
      size_t output_blocks = output_len / (size_t)168U;
      size_t output_rem = output_len % (size_t)168U;
      if (output_blocks == (size_t)0U) {
        libcrux_sha3_simd_portable_squeeze_9b_60(&s, output, (size_t)0U,
                                                 output_len);
      } else {
        libcrux_sha3_simd_portable_squeeze_9b_60(&s, output, (size_t)0U,
                                                 (size_t)168U);
        for (size_t i = (size_t)1U; i < output_blocks; i++) {
          size_t i0 = i;
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_60(
              &s, output, i0 * (size_t)168U, (size_t)168U);
        }
        if (output_rem != (size_t)0U) {
          libcrux_sha3_generic_keccak_keccakf1600_80_71(&s);
          libcrux_sha3_simd_portable_squeeze_9b_60(
              &s, output, output_len - output_rem, output_rem);
        }
      }
      return;
    }
    size_t i = uu____0.f0;
    /* original Rust expression is not an lvalue in C */
    Eurydice_arr_dc lvalue = {.data = {input}};
    libcrux_sha3_generic_keccak_absorb_block_80_e93(&s, &lvalue,
                                                    i * (size_t)168U);
  }
  KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__,
                    "panic!");
  KRML_HOST_EXIT(255U);
}

/**
 A portable SHAKE128 implementation.
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_portable_shake128(
    Eurydice_mut_borrow_slice_u8 digest, Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_generic_keccak_portable_keccak1_37(data, digest);
}

/**
 SHAKE 128

 Writes `out.len()` bytes.
*/
static __attribute__((unused)) void libcrux_sha3_shake128_ema(Eurydice_mut_borrow_slice_u8 out,
                               Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_portable_shake128(out, data);
}

/**
 SHAKE 256

 Writes `out.len()` bytes.
*/
static __attribute__((unused)) void libcrux_sha3_shake256_ema(Eurydice_mut_borrow_slice_u8 out,
                               Eurydice_borrow_slice_u8 data) {
  libcrux_sha3_portable_shake256(out, data);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_first_five_blocks_b4 with const
generics
- RATE= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_generic_keccak_portable_squeeze_first_five_blocks_b4_60(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out) {
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)0U, (size_t)168U);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)168U,
                                           (size_t)168U);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)2U * (size_t)168U,
                                           (size_t)168U);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)3U * (size_t)168U,
                                           (size_t)168U);
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_60(self, out, (size_t)4U * (size_t)168U,
                                           (size_t)168U);
}

/**
 Squeeze five blocks
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_portable_incremental_shake128_squeeze_first_five_blocks(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out0) {
  libcrux_sha3_generic_keccak_portable_squeeze_first_five_blocks_b4_60(s, out0);
}

/**
 Absorb some data for SHAKE-256 for the last time
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_sha3_portable_incremental_shake256_absorb_final(
    Eurydice_arr_7c *s, Eurydice_borrow_slice_u8 data) {
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_dc lvalue = {.data = {data}};
  libcrux_sha3_generic_keccak_absorb_final_80_bd1(s, &lvalue, (size_t)0U,
                                                  data.meta);
}

/**
 Create a new SHAKE-256 state object.
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_7c
libcrux_sha3_portable_incremental_shake256_init(void) {
  return libcrux_sha3_generic_keccak_new_80_71();
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_first_block_b4 with const generics
- RATE= 136
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_generic_keccak_portable_squeeze_first_block_b4_b2(
    const Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out) {
  libcrux_sha3_simd_portable_squeeze_9b_b2(self, out, (size_t)0U, (size_t)136U);
}

/**
 Squeeze the first SHAKE-256 block
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_portable_incremental_shake256_squeeze_first_block(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out) {
  libcrux_sha3_generic_keccak_portable_squeeze_first_block_b4_b2(&s[0U], out);
}

/**
This function found in impl {libcrux_sha3::generic_keccak::KeccakState<u64,
1usize>[core::marker::Sized<u64>,
libcrux_sha3::simd::portable::{libcrux_sha3::traits::KeccakItem<1usize> for
u64}]}
*/
/**
A monomorphic instance of
libcrux_sha3.generic_keccak.portable.squeeze_next_block_b4 with const generics
- RATE= 136
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_generic_keccak_portable_squeeze_next_block_b4_b2(
    Eurydice_arr_7c *self, Eurydice_mut_borrow_slice_u8 out, size_t start) {
  libcrux_sha3_generic_keccak_keccakf1600_80_71(self);
  libcrux_sha3_simd_portable_squeeze_9b_b2(self, out, start, (size_t)136U);
}

/**
 Squeeze the next SHAKE-256 block
*/
static __attribute__((unused)) KRML_MUSTINLINE void
libcrux_sha3_portable_incremental_shake256_squeeze_next_block(
    Eurydice_arr_7c *s, Eurydice_mut_borrow_slice_u8 out) {
  libcrux_sha3_generic_keccak_portable_squeeze_next_block_b4_b2(s, out,
                                                                (size_t)0U);
}


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_mlkem_portable.c */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */



static __attribute__((unused)) inline Eurydice_arr_c7 libcrux_ml_kem_hash_functions_portable_G(
    Eurydice_borrow_slice_u8 input) {
  Eurydice_arr_c7 digest = {.data = {0U}};
  libcrux_sha3_portable_sha512(Eurydice_array_to_slice_mut_17(&digest), input);
  return digest;
}

static __attribute__((unused)) inline Eurydice_arr_ec libcrux_ml_kem_hash_functions_portable_H(
    Eurydice_borrow_slice_u8 input) {
  Eurydice_arr_ec digest = {.data = {0U}};
  libcrux_sha3_portable_sha256(Eurydice_array_to_slice_mut_01(&digest), input);
  return digest;
}

#define ZETAS_TIMES_MONTGOMERY_R                                              \
  ((KRML_CLITERAL(Eurydice_arr_34){                                           \
      .data = {-1044, -758,  -359,  -1517, 1493,  1422,  287,   202,   -171,  \
               622,   1577,  182,   962,   -1202, -1474, 1468,  573,   -1325, \
               264,   383,   -829,  1458,  -1602, -130,  -681,  1017,  732,   \
               608,   -1542, 411,   -205,  -1571, 1223,  652,   -552,  1015,  \
               -1293, 1491,  -282,  -1544, 516,   -8,    -320,  -666,  -1618, \
               -1162, 126,   1469,  -853,  -90,   -271,  830,   107,   -1421, \
               -247,  -951,  -398,  961,   -1508, -725,  448,   -1065, 677,   \
               -1275, -1103, 430,   555,   843,   -1251, 871,   1550,  105,   \
               422,   587,   177,   -235,  -291,  -460,  1574,  1653,  -246,  \
               778,   1159,  -147,  -777,  1483,  -602,  1119,  -1590, 644,   \
               -872,  349,   418,   329,   -156,  -75,   817,   1097,  603,   \
               610,   1322,  -1285, -1465, 384,   -1215, -136,  1218,  -1335, \
               -874,  220,   -1187, -1659, -1185, -1530, -1278, 794,   -1510, \
               -854,  -870,  478,   -108,  -308,  996,   991,   958,   -1460, \
               1522,  1628}}))

static __attribute__((unused)) KRML_MUSTINLINE int16_t zeta(size_t i) {
  return ZETAS_TIMES_MONTGOMERY_R.data[i];
}

#define VECTORS_IN_RING_ELEMENT ((size_t)16U)

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_vector_type_zero(void) {
  return libcrux_secrets_int_public_integers_classify_27_4b(
      (KRML_CLITERAL(Eurydice_arr_d6){.data = {0U}}));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ZERO_b8(void) {
  return libcrux_ml_kem_vector_portable_vector_type_zero();
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_vector_type_from_i16_array(
    Eurydice_borrow_slice_i16 array) {
  Eurydice_arr_d6 arr;
  memcpy(arr.data,
         Eurydice_slice_subslice_shared_a6(
             array, (KRML_CLITERAL(core_ops_range_Range_87){
                        .start = (size_t)0U, .end = (size_t)16U}))
             .ptr,
         (size_t)16U * sizeof(int16_t));
  return core_result_unwrap_26_d3((KRML_CLITERAL(core_result_Result_ec){
      .tag = core_result_Ok, .val = {.case_Ok = arr}}));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_from_i16_array_b8(
    Eurydice_borrow_slice_i16 array) {
  return libcrux_ml_kem_vector_portable_vector_type_from_i16_array(
      libcrux_secrets_int_classify_public_classify_ref_6d_39(array));
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_vector_type_to_i16_array(Eurydice_arr_d6 x) {
  return x;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_to_i16_array_b8(
    Eurydice_arr_d6 x) {
  return libcrux_secrets_int_public_integers_declassify_d8_4b(
      libcrux_ml_kem_vector_portable_vector_type_to_i16_array(x));
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_vector_type_from_bytes(
    Eurydice_borrow_slice_u8 array) {
  Eurydice_arr_d6 elements;
  int16_t repeat_expression[16U];
  KRML_MAYBE_FOR16(i, (size_t)0U, (size_t)16U, (size_t)1U,
                   repeat_expression[i] = libcrux_secrets_int_I16(0););
  memcpy(elements.data, repeat_expression, (size_t)16U * sizeof(int16_t));
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    elements.data[i0] =
        (int16_t)((uint32_t)libcrux_secrets_int_as_i16_59(
                      array.ptr[(size_t)2U * i0 + (size_t)1U])
                  << 8U) |
        libcrux_secrets_int_as_i16_59(array.ptr[(size_t)2U * i0]);
  }
  return elements;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_from_bytes_b8(
    Eurydice_borrow_slice_u8 array) {
  return libcrux_ml_kem_vector_portable_vector_type_from_bytes(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(array));
}

static __attribute__((unused)) KRML_MUSTINLINE void libcrux_ml_kem_vector_portable_vector_type_to_bytes(
    Eurydice_arr_d6 x, Eurydice_mut_borrow_slice_u8 bytes) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    bytes.ptr[(size_t)2U * i0 + (size_t)1U] =
        libcrux_secrets_int_as_u8_f5(x.data[i0] >> 8U);
    bytes.ptr[(size_t)2U * i0] = libcrux_secrets_int_as_u8_f5(x.data[i0]);
  }
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) void libcrux_ml_kem_vector_portable_to_bytes_b8(
    Eurydice_arr_d6 x, Eurydice_mut_borrow_slice_u8 bytes) {
  libcrux_ml_kem_vector_portable_vector_type_to_bytes(
      x, libcrux_secrets_int_public_integers_classify_mut_slice_75(bytes));
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_add(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    size_t uu____0 = i0;
    lhs.data[uu____0] += rhs->data[i0];
  }
  return lhs;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_add_b8(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs) {
  return libcrux_ml_kem_vector_portable_arithmetic_add(lhs, rhs);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 libcrux_ml_kem_vector_portable_arithmetic_sub(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    size_t uu____0 = i0;
    lhs.data[uu____0] -= rhs->data[i0];
  }
  return lhs;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_sub_b8(
    Eurydice_arr_d6 lhs, const Eurydice_arr_d6 *rhs) {
  return libcrux_ml_kem_vector_portable_arithmetic_sub(lhs, rhs);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_multiply_by_constant(
    Eurydice_arr_d6 vec, int16_t c) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    size_t uu____0 = i0;
    vec.data[uu____0] *= c;
  }
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_multiply_by_constant_b8(
    Eurydice_arr_d6 vec, int16_t c) {
  return libcrux_ml_kem_vector_portable_arithmetic_multiply_by_constant(vec, c);
}

/**
 Note: This function is not secret independent
 Only use with public values.
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_cond_subtract_3329(
    Eurydice_arr_d6 vec) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    if (libcrux_secrets_int_public_integers_declassify_d8_39(vec.data[i0]) >=
        3329) {
      size_t uu____0 = i0;
      vec.data[uu____0] -= 3329;
    }
  }
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_cond_subtract_3329_b8(
    Eurydice_arr_d6 v) {
  return libcrux_ml_kem_vector_portable_arithmetic_cond_subtract_3329(v);
}

/**
 Signed Barrett Reduction

 Given an input `value`, `barrett_reduce` outputs a representative `result`
 such that:

 - result ≡ value (mod FIELD_MODULUS)
 - the absolute value of `result` is bound as follows:

 `|result| ≤ FIELD_MODULUS / 2 · (|value|/BARRETT_R + 1)

 Note: The input bound is 28296 to prevent overflow in the multiplication of
 quotient by FIELD_MODULUS

*/
static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce_element(
    int16_t value) {
  int32_t t = libcrux_secrets_int_as_i32_f5(value) *
                  LIBCRUX_ML_KEM_VECTOR_PORTABLE_ARITHMETIC_BARRETT_MULTIPLIER +
              (LIBCRUX_ML_KEM_VECTOR_TRAITS_BARRETT_R >> 1U);
  int16_t quotient = libcrux_secrets_int_as_i16_36(
      t >> (uint32_t)LIBCRUX_ML_KEM_VECTOR_TRAITS_BARRETT_SHIFT);
  return value - quotient * LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS;
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce(Eurydice_arr_d6 vec) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int16_t vi =
        libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce_element(
            vec.data[i0]);
    vec.data[i0] = vi;
  }
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_barrett_reduce_b8(
    Eurydice_arr_d6 vector) {
  return libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce(vector);
}

/**
 Signed Montgomery Reduction

 Given an input `value`, `montgomery_reduce` outputs a representative `o`
 such that:

 - o ≡ value · MONTGOMERY_R^(-1) (mod FIELD_MODULUS)
 - the absolute value of `o` is bound as follows:

 `|result| ≤ ceil(|value| / MONTGOMERY_R) + 1665

 In particular, if `|value| ≤ FIELD_MODULUS-1 * FIELD_MODULUS-1`, then `|o| <=
 FIELD_MODULUS-1`. And, if `|value| ≤ pow2 16 * FIELD_MODULUS-1`, then `|o| <=
 FIELD_MODULUS + 1664

*/
static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_arithmetic_montgomery_reduce_element(
    int32_t value) {
  int32_t k =
      libcrux_secrets_int_as_i32_f5(libcrux_secrets_int_as_i16_36(value)) *
      libcrux_secrets_int_as_i32_b8(
          libcrux_secrets_int_public_integers_classify_27_df(
              LIBCRUX_ML_KEM_VECTOR_TRAITS_INVERSE_OF_MODULUS_MOD_MONTGOMERY_R));
  int32_t k_times_modulus =
      libcrux_secrets_int_as_i32_f5(libcrux_secrets_int_as_i16_36(k)) *
      libcrux_secrets_int_as_i32_f5(
          libcrux_secrets_int_public_integers_classify_27_39(
              LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS));
  int16_t c = libcrux_secrets_int_as_i16_36(
      k_times_modulus >>
      (uint32_t)LIBCRUX_ML_KEM_VECTOR_PORTABLE_ARITHMETIC_MONTGOMERY_SHIFT);
  int16_t value_high = libcrux_secrets_int_as_i16_36(
      value >>
      (uint32_t)LIBCRUX_ML_KEM_VECTOR_PORTABLE_ARITHMETIC_MONTGOMERY_SHIFT);
  return value_high - c;
}

/**
 If `fe` is some field element 'x' of the Kyber field and `fer` is congruent to
 `y · MONTGOMERY_R`, this procedure outputs a value that is congruent to
 `x · y`, as follows:

    `fe · fer ≡ x · y · MONTGOMERY_R (mod FIELD_MODULUS)`

 `montgomery_reduce` takes the value `x · y · MONTGOMERY_R` and outputs a
 representative `x · y · MONTGOMERY_R * MONTGOMERY_R^{-1} ≡ x · y (mod
 FIELD_MODULUS)`.
*/
static __attribute__((unused)) KRML_MUSTINLINE int16_t
libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_fe_by_fer(
    int16_t fe, int16_t fer) {
  int32_t product =
      libcrux_secrets_int_as_i32_f5(fe) * libcrux_secrets_int_as_i32_f5(fer);
  return libcrux_ml_kem_vector_portable_arithmetic_montgomery_reduce_element(
      product);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_by_constant(
    Eurydice_arr_d6 vec, int16_t c) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    vec.data[i0] =
        libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_fe_by_fer(
            vec.data[i0], c);
  }
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6
libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
    Eurydice_arr_d6 vector, int16_t constant) {
  return libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_by_constant(
      vector, libcrux_secrets_int_public_integers_classify_27_39(constant));
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_bitwise_and_with_constant(
    Eurydice_arr_d6 vec, int16_t c) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    size_t uu____0 = i0;
    vec.data[uu____0] &= c;
  }
  return vec;
}

/**
A monomorphic instance of libcrux_ml_kem.vector.portable.arithmetic.shift_right
with const generics
- SHIFT_BY= 15
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 shift_right_ef(Eurydice_arr_d6 vec) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    vec.data[i0] >>= (uint32_t)15;
  }
  return vec;
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_arithmetic_to_unsigned_representative(
    Eurydice_arr_d6 a) {
  Eurydice_arr_d6 t = shift_right_ef(a);
  Eurydice_arr_d6 fm =
      libcrux_ml_kem_vector_portable_arithmetic_bitwise_and_with_constant(
          t, LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS);
  return libcrux_ml_kem_vector_portable_arithmetic_add(a, &fm);
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_to_unsigned_representative_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_arithmetic_to_unsigned_representative(
      a);
}

/**
 The `compress_*` functions implement the `Compress` function specified in the
 NIST FIPS 203 standard (Page 18, Expression 4.5), which is defined as:

 ```plaintext
 Compress_d: ℤq -> ℤ_{2ᵈ}
 Compress_d(x) = ⌈(2ᵈ/q)·x⌋
 ```

 Since `⌈x⌋ = ⌊x + 1/2⌋` we have:

 ```plaintext
 Compress_d(x) = ⌊(2ᵈ/q)·x + 1/2⌋
               = ⌊(2^{d+1}·x + q) / 2q⌋
 ```

 For further information about the function implementations, consult the
 `implementation_notes.pdf` document in this directory.

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
static __attribute__((unused)) uint8_t libcrux_ml_kem_vector_portable_compress_compress_message_coefficient(
    uint16_t fe) {
  int16_t shifted = libcrux_secrets_int_public_integers_classify_27_39(1664) -
                    libcrux_secrets_int_as_i16_ca(fe);
  int16_t mask = shifted >> 15U;
  int16_t shifted_to_positive = mask ^ shifted;
  int16_t shifted_positive_in_range = shifted_to_positive - 832;
  int16_t r0 = shifted_positive_in_range >> 15U;
  int16_t r1 = r0 & 1;
  return libcrux_secrets_int_as_u8_f5(r1);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_compress_compress_1(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    a.data[i0] = libcrux_secrets_int_as_i16_59(
        libcrux_ml_kem_vector_portable_compress_compress_message_coefficient(
            libcrux_secrets_int_as_u16_f5(a.data[i0])));
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_compress_1_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_compress_compress_1(a);
}

static __attribute__((unused)) KRML_MUSTINLINE uint32_t
libcrux_ml_kem_vector_portable_arithmetic_get_n_least_significant_bits(
    uint8_t n, uint32_t value) {
  return value & ((1U << (uint32_t)n) - 1U);
}

static __attribute__((unused)) int16_t libcrux_ml_kem_vector_portable_compress_compress_ciphertext_coefficient(
    uint8_t coefficient_bits, uint16_t fe) {
  uint64_t compressed = libcrux_secrets_int_as_u64_ca(fe)
                        << (uint32_t)coefficient_bits;
  compressed += 1664ULL;
  compressed *= 10321340ULL;
  compressed >>= 35U;
  return libcrux_secrets_int_as_i16_b8(
      libcrux_ml_kem_vector_portable_arithmetic_get_n_least_significant_bits(
          coefficient_bits, libcrux_secrets_int_as_u32_a3(compressed)));
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_compress_decompress_1(Eurydice_arr_d6 a) {
  Eurydice_arr_d6 z = libcrux_ml_kem_vector_portable_vector_type_zero();
  Eurydice_arr_d6 s = libcrux_ml_kem_vector_portable_arithmetic_sub(z, &a);
  Eurydice_arr_d6 res =
      libcrux_ml_kem_vector_portable_arithmetic_bitwise_and_with_constant(s,
                                                                          1665);
  return res;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_decompress_1_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_compress_decompress_1(a);
}

static __attribute__((unused)) KRML_MUSTINLINE void libcrux_ml_kem_vector_portable_ntt_ntt_step(
    Eurydice_arr_d6 *vec, int16_t zeta, size_t i, size_t j) {
  int16_t t =
      libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_fe_by_fer(
          vec->data[j],
          libcrux_secrets_int_public_integers_classify_27_39(zeta));
  int16_t a_minus_t = vec->data[i] - t;
  int16_t a_plus_t = vec->data[i] + t;
  vec->data[j] = a_minus_t;
  vec->data[i] = a_plus_t;
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_ntt_ntt_layer_1_step(Eurydice_arr_d6 vec,
                                                    int16_t zeta0,
                                                    int16_t zeta1,
                                                    int16_t zeta2,
                                                    int16_t zeta3) {
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta0, (size_t)0U,
                                              (size_t)2U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta0, (size_t)1U,
                                              (size_t)3U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta1, (size_t)4U,
                                              (size_t)6U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta1, (size_t)5U,
                                              (size_t)7U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta2, (size_t)8U,
                                              (size_t)10U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta2, (size_t)9U,
                                              (size_t)11U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta3, (size_t)12U,
                                              (size_t)14U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta3, (size_t)13U,
                                              (size_t)15U);
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_layer_1_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1, int16_t zeta2,
    int16_t zeta3) {
  return libcrux_ml_kem_vector_portable_ntt_ntt_layer_1_step(a, zeta0, zeta1,
                                                             zeta2, zeta3);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_ntt_ntt_layer_2_step(Eurydice_arr_d6 vec,
                                                    int16_t zeta0,
                                                    int16_t zeta1) {
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta0, (size_t)0U,
                                              (size_t)4U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta0, (size_t)1U,
                                              (size_t)5U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta0, (size_t)2U,
                                              (size_t)6U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta0, (size_t)3U,
                                              (size_t)7U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta1, (size_t)8U,
                                              (size_t)12U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta1, (size_t)9U,
                                              (size_t)13U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta1, (size_t)10U,
                                              (size_t)14U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta1, (size_t)11U,
                                              (size_t)15U);
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_layer_2_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1) {
  return libcrux_ml_kem_vector_portable_ntt_ntt_layer_2_step(a, zeta0, zeta1);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_ntt_ntt_layer_3_step(Eurydice_arr_d6 vec,
                                                    int16_t zeta) {
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)0U,
                                              (size_t)8U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)1U,
                                              (size_t)9U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)2U,
                                              (size_t)10U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)3U,
                                              (size_t)11U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)4U,
                                              (size_t)12U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)5U,
                                              (size_t)13U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)6U,
                                              (size_t)14U);
  libcrux_ml_kem_vector_portable_ntt_ntt_step(&vec, zeta, (size_t)7U,
                                              (size_t)15U);
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_layer_3_step_b8(
    Eurydice_arr_d6 a, int16_t zeta) {
  return libcrux_ml_kem_vector_portable_ntt_ntt_layer_3_step(a, zeta);
}

static __attribute__((unused)) KRML_MUSTINLINE void libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(
    Eurydice_arr_d6 *vec, int16_t zeta, size_t i, size_t j) {
  int16_t a_minus_b = vec->data[j] - vec->data[i];
  int16_t a_plus_b = vec->data[j] + vec->data[i];
  int16_t o0 = libcrux_ml_kem_vector_portable_arithmetic_barrett_reduce_element(
      a_plus_b);
  int16_t o1 =
      libcrux_ml_kem_vector_portable_arithmetic_montgomery_multiply_fe_by_fer(
          a_minus_b, libcrux_secrets_int_public_integers_classify_27_39(zeta));
  vec->data[i] = o0;
  vec->data[j] = o1;
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_1_step(Eurydice_arr_d6 vec,
                                                        int16_t zeta0,
                                                        int16_t zeta1,
                                                        int16_t zeta2,
                                                        int16_t zeta3) {
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta0, (size_t)0U,
                                                  (size_t)2U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta0, (size_t)1U,
                                                  (size_t)3U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta1, (size_t)4U,
                                                  (size_t)6U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta1, (size_t)5U,
                                                  (size_t)7U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta2, (size_t)8U,
                                                  (size_t)10U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta2, (size_t)9U,
                                                  (size_t)11U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta3, (size_t)12U,
                                                  (size_t)14U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta3, (size_t)13U,
                                                  (size_t)15U);
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_inv_ntt_layer_1_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1, int16_t zeta2,
    int16_t zeta3) {
  return libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_1_step(
      a, zeta0, zeta1, zeta2, zeta3);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_2_step(Eurydice_arr_d6 vec,
                                                        int16_t zeta0,
                                                        int16_t zeta1) {
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta0, (size_t)0U,
                                                  (size_t)4U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta0, (size_t)1U,
                                                  (size_t)5U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta0, (size_t)2U,
                                                  (size_t)6U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta0, (size_t)3U,
                                                  (size_t)7U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta1, (size_t)8U,
                                                  (size_t)12U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta1, (size_t)9U,
                                                  (size_t)13U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta1, (size_t)10U,
                                                  (size_t)14U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta1, (size_t)11U,
                                                  (size_t)15U);
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_inv_ntt_layer_2_step_b8(
    Eurydice_arr_d6 a, int16_t zeta0, int16_t zeta1) {
  return libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_2_step(a, zeta0,
                                                                 zeta1);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_3_step(Eurydice_arr_d6 vec,
                                                        int16_t zeta) {
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)0U,
                                                  (size_t)8U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)1U,
                                                  (size_t)9U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)2U,
                                                  (size_t)10U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)3U,
                                                  (size_t)11U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)4U,
                                                  (size_t)12U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)5U,
                                                  (size_t)13U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)6U,
                                                  (size_t)14U);
  libcrux_ml_kem_vector_portable_ntt_inv_ntt_step(&vec, zeta, (size_t)7U,
                                                  (size_t)15U);
  return vec;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_inv_ntt_layer_3_step_b8(
    Eurydice_arr_d6 a, int16_t zeta) {
  return libcrux_ml_kem_vector_portable_ntt_inv_ntt_layer_3_step(a, zeta);
}

/**
 Compute the product of two Kyber binomials with respect to the
 modulus `X² - zeta`.

 This function almost implements <strong>Algorithm 11</strong> of the
 NIST FIPS 203 standard, which is reproduced below:

 ```plaintext
 Input:  a₀, a₁, b₀, b₁ ∈ ℤq.
 Input: γ ∈ ℤq.
 Output: c₀, c₁ ∈ ℤq.

 c₀ ← a₀·b₀ + a₁·b₁·γ
 c₁ ← a₀·b₁ + a₁·b₀
 return c₀, c₁
 ```
 We say "almost" because the coefficients output by this function are in
 the Montgomery domain (unlike in the specification).

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
static __attribute__((unused)) KRML_MUSTINLINE void libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
    const Eurydice_arr_d6 *a, const Eurydice_arr_d6 *b, int16_t zeta, size_t i,
    Eurydice_arr_d6 *out) {
  int16_t ai = a->data[(size_t)2U * i];
  int16_t bi = b->data[(size_t)2U * i];
  int16_t aj = a->data[(size_t)2U * i + (size_t)1U];
  int16_t bj = b->data[(size_t)2U * i + (size_t)1U];
  int32_t ai_bi =
      libcrux_secrets_int_as_i32_f5(ai) * libcrux_secrets_int_as_i32_f5(bi);
  int32_t aj_bj_ =
      libcrux_secrets_int_as_i32_f5(aj) * libcrux_secrets_int_as_i32_f5(bj);
  int16_t aj_bj =
      libcrux_ml_kem_vector_portable_arithmetic_montgomery_reduce_element(
          aj_bj_);
  int32_t aj_bj_zeta = libcrux_secrets_int_as_i32_f5(aj_bj) *
                       libcrux_secrets_int_as_i32_f5(zeta);
  int32_t ai_bi_aj_bj = ai_bi + aj_bj_zeta;
  int16_t o0 =
      libcrux_ml_kem_vector_portable_arithmetic_montgomery_reduce_element(
          ai_bi_aj_bj);
  int32_t ai_bj =
      libcrux_secrets_int_as_i32_f5(ai) * libcrux_secrets_int_as_i32_f5(bj);
  int32_t aj_bi =
      libcrux_secrets_int_as_i32_f5(aj) * libcrux_secrets_int_as_i32_f5(bi);
  int32_t ai_bj_aj_bi = ai_bj + aj_bi;
  int16_t o1 =
      libcrux_ml_kem_vector_portable_arithmetic_montgomery_reduce_element(
          ai_bj_aj_bi);
  out->data[(size_t)2U * i] = o0;
  out->data[(size_t)2U * i + (size_t)1U] = o1;
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_ntt_multiply(
    const Eurydice_arr_d6 *lhs, const Eurydice_arr_d6 *rhs, int16_t zeta0,
    int16_t zeta1, int16_t zeta2, int16_t zeta3) {
  int16_t nzeta0 = -zeta0;
  int16_t nzeta1 = -zeta1;
  int16_t nzeta2 = -zeta2;
  int16_t nzeta3 = -zeta3;
  Eurydice_arr_d6 out = libcrux_ml_kem_vector_portable_vector_type_zero();
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(zeta0),
      (size_t)0U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(nzeta0),
      (size_t)1U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(zeta1),
      (size_t)2U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(nzeta1),
      (size_t)3U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(zeta2),
      (size_t)4U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(nzeta2),
      (size_t)5U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(zeta3),
      (size_t)6U, &out);
  libcrux_ml_kem_vector_portable_ntt_ntt_multiply_binomials(
      lhs, rhs, libcrux_secrets_int_public_integers_classify_27_39(nzeta3),
      (size_t)7U, &out);
  return out;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_ntt_multiply_b8(
    const Eurydice_arr_d6 *lhs, const Eurydice_arr_d6 *rhs, int16_t zeta0,
    int16_t zeta1, int16_t zeta2, int16_t zeta3) {
  return libcrux_ml_kem_vector_portable_ntt_ntt_multiply(lhs, rhs, zeta0, zeta1,
                                                         zeta2, zeta3);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_array_u8x2
libcrux_ml_kem_vector_portable_serialize_serialize_1(Eurydice_arr_d6 v) {
  uint8_t result0 =
      (((((((uint32_t)libcrux_secrets_int_as_u8_f5(v.data[0U]) |
            (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[1U]) << 1U) |
           (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[2U]) << 2U) |
          (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[3U]) << 3U) |
         (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[4U]) << 4U) |
        (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[5U]) << 5U) |
       (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[6U]) << 6U) |
      (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[7U]) << 7U;
  uint8_t result1 =
      (((((((uint32_t)libcrux_secrets_int_as_u8_f5(v.data[8U]) |
            (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[9U]) << 1U) |
           (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[10U]) << 2U) |
          (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[11U]) << 3U) |
         (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[12U]) << 4U) |
        (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[13U]) << 5U) |
       (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[14U]) << 6U) |
      (uint32_t)libcrux_secrets_int_as_u8_f5(v.data[15U]) << 7U;
  return (KRML_CLITERAL(Eurydice_array_u8x2){.data = {result0, result1}});
}

static __attribute__((unused)) Eurydice_array_u8x2 libcrux_ml_kem_vector_portable_serialize_1(
    Eurydice_arr_d6 a) {
  return libcrux_secrets_int_public_integers_declassify_d8_75(
      libcrux_ml_kem_vector_portable_serialize_serialize_1(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_array_u8x2 libcrux_ml_kem_vector_portable_serialize_1_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_serialize_1(a);
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_serialize_deserialize_1(
    Eurydice_borrow_slice_u8 v) {
  int16_t result0 = libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] & 1U);
  int16_t result1 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 1U & 1U);
  int16_t result2 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 2U & 1U);
  int16_t result3 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 3U & 1U);
  int16_t result4 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 4U & 1U);
  int16_t result5 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 5U & 1U);
  int16_t result6 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 6U & 1U);
  int16_t result7 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[0U] >> 7U & 1U);
  int16_t result8 = libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] & 1U);
  int16_t result9 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 1U & 1U);
  int16_t result10 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 2U & 1U);
  int16_t result11 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 3U & 1U);
  int16_t result12 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 4U & 1U);
  int16_t result13 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 5U & 1U);
  int16_t result14 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 6U & 1U);
  int16_t result15 =
      libcrux_secrets_int_as_i16_59((uint32_t)v.ptr[1U] >> 7U & 1U);
  return (KRML_CLITERAL(Eurydice_arr_d6){
      .data = {result0, result1, result2, result3, result4, result5, result6,
               result7, result8, result9, result10, result11, result12,
               result13, result14, result15}});
}

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_1(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_serialize_deserialize_1(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_1_b8(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_deserialize_1(a);
}

static __attribute__((unused)) KRML_MUSTINLINE uint8_t_x4
libcrux_ml_kem_vector_portable_serialize_serialize_4_int(
    Eurydice_borrow_slice_i16 v) {
  uint8_t result0 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[1U]) << 4U |
                    (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[0U]);
  uint8_t result1 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[3U]) << 4U |
                    (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[2U]);
  uint8_t result2 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[5U]) << 4U |
                    (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[4U]);
  uint8_t result3 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[7U]) << 4U |
                    (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[6U]);
  return (KRML_CLITERAL(uint8_t_x4){
      .fst = result0, .snd = result1, .thd = result2, .f3 = result3});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_array_u8x8
libcrux_ml_kem_vector_portable_serialize_serialize_4(Eurydice_arr_d6 v) {
  uint8_t_x4 result0_3 =
      libcrux_ml_kem_vector_portable_serialize_serialize_4_int(
          Eurydice_array_to_subslice_shared_e7(
              &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                          .end = (size_t)8U})));
  uint8_t_x4 result4_7 =
      libcrux_ml_kem_vector_portable_serialize_serialize_4_int(
          Eurydice_array_to_subslice_shared_e7(
              &v, (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = (size_t)8U, .end = (size_t)16U})));
  return (KRML_CLITERAL(Eurydice_array_u8x8){
      .data = {result0_3.fst, result0_3.snd, result0_3.thd, result0_3.f3,
               result4_7.fst, result4_7.snd, result4_7.thd, result4_7.f3}});
}

static __attribute__((unused)) Eurydice_array_u8x8 libcrux_ml_kem_vector_portable_serialize_4(
    Eurydice_arr_d6 a) {
  return libcrux_secrets_int_public_integers_declassify_d8_52(
      libcrux_ml_kem_vector_portable_serialize_serialize_4(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_array_u8x8 libcrux_ml_kem_vector_portable_serialize_4_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_serialize_4(a);
}

static __attribute__((unused)) KRML_MUSTINLINE int16_t_x8
libcrux_ml_kem_vector_portable_serialize_deserialize_4_int(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t v0 = libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[0U] & 15U);
  int16_t v1 =
      libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[0U] >> 4U & 15U);
  int16_t v2 = libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[1U] & 15U);
  int16_t v3 =
      libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[1U] >> 4U & 15U);
  int16_t v4 = libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[2U] & 15U);
  int16_t v5 =
      libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[2U] >> 4U & 15U);
  int16_t v6 = libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[3U] & 15U);
  int16_t v7 =
      libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[3U] >> 4U & 15U);
  return (KRML_CLITERAL(int16_t_x8){.fst = v0,
                                    .snd = v1,
                                    .thd = v2,
                                    .f3 = v3,
                                    .f4 = v4,
                                    .f5 = v5,
                                    .f6 = v6,
                                    .f7 = v7});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_serialize_deserialize_4(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t_x8 v0_7 = libcrux_ml_kem_vector_portable_serialize_deserialize_4_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                         .end = (size_t)4U})));
  int16_t_x8 v8_15 = libcrux_ml_kem_vector_portable_serialize_deserialize_4_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)4U,
                                                         .end = (size_t)8U})));
  return (KRML_CLITERAL(Eurydice_arr_d6){
      .data = {v0_7.fst, v0_7.snd, v0_7.thd, v0_7.f3, v0_7.f4, v0_7.f5, v0_7.f6,
               v0_7.f7, v8_15.fst, v8_15.snd, v8_15.thd, v8_15.f3, v8_15.f4,
               v8_15.f5, v8_15.f6, v8_15.f7}});
}

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_4(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_serialize_deserialize_4(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_4_b8(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_deserialize_4(a);
}

static __attribute__((unused)) KRML_MUSTINLINE uint8_t_x5
libcrux_ml_kem_vector_portable_serialize_serialize_5_int(
    Eurydice_borrow_slice_i16 v) {
  uint8_t r0 = libcrux_secrets_int_as_u8_f5(
      v.ptr[0U] | (int16_t)((uint32_t)v.ptr[1U] << 5U));
  uint8_t r1 = libcrux_secrets_int_as_u8_f5(
      (v.ptr[1U] >> 3U | (int16_t)((uint32_t)v.ptr[2U] << 2U)) |
      (int16_t)((uint32_t)v.ptr[3U] << 7U));
  uint8_t r2 = libcrux_secrets_int_as_u8_f5(
      v.ptr[3U] >> 1U | (int16_t)((uint32_t)v.ptr[4U] << 4U));
  uint8_t r3 = libcrux_secrets_int_as_u8_f5(
      (v.ptr[4U] >> 4U | (int16_t)((uint32_t)v.ptr[5U] << 1U)) |
      (int16_t)((uint32_t)v.ptr[6U] << 6U));
  uint8_t r4 = libcrux_secrets_int_as_u8_f5(
      v.ptr[6U] >> 2U | (int16_t)((uint32_t)v.ptr[7U] << 3U));
  return (KRML_CLITERAL(uint8_t_x5){
      .fst = r0, .snd = r1, .thd = r2, .f3 = r3, .f4 = r4});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_6d
libcrux_ml_kem_vector_portable_serialize_serialize_5(Eurydice_arr_d6 v) {
  uint8_t_x5 r0_4 = libcrux_ml_kem_vector_portable_serialize_serialize_5_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                      .end = (size_t)8U})));
  uint8_t_x5 r5_9 = libcrux_ml_kem_vector_portable_serialize_serialize_5_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)8U,
                                                      .end = (size_t)16U})));
  return (KRML_CLITERAL(Eurydice_arr_6d){
      .data = {r0_4.fst, r0_4.snd, r0_4.thd, r0_4.f3, r0_4.f4, r5_9.fst,
               r5_9.snd, r5_9.thd, r5_9.f3, r5_9.f4}});
}

static __attribute__((unused)) Eurydice_arr_6d libcrux_ml_kem_vector_portable_serialize_5(Eurydice_arr_d6 a) {
  return libcrux_secrets_int_public_integers_declassify_d8_37(
      libcrux_ml_kem_vector_portable_serialize_serialize_5(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_6d libcrux_ml_kem_vector_portable_serialize_5_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_serialize_5(a);
}

static __attribute__((unused)) KRML_MUSTINLINE int16_t_x8
libcrux_ml_kem_vector_portable_serialize_deserialize_5_int(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t v0 = libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[0U] & 31U);
  int16_t v1 = libcrux_secrets_int_as_i16_59(
      ((uint32_t)bytes.ptr[1U] & 3U) << 3U | (uint32_t)bytes.ptr[0U] >> 5U);
  int16_t v2 =
      libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[1U] >> 2U & 31U);
  int16_t v3 = libcrux_secrets_int_as_i16_59(
      ((uint32_t)bytes.ptr[2U] & 15U) << 1U | (uint32_t)bytes.ptr[1U] >> 7U);
  int16_t v4 = libcrux_secrets_int_as_i16_59(
      ((uint32_t)bytes.ptr[3U] & 1U) << 4U | (uint32_t)bytes.ptr[2U] >> 4U);
  int16_t v5 =
      libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[3U] >> 1U & 31U);
  int16_t v6 = libcrux_secrets_int_as_i16_59(
      ((uint32_t)bytes.ptr[4U] & 7U) << 2U | (uint32_t)bytes.ptr[3U] >> 6U);
  int16_t v7 = libcrux_secrets_int_as_i16_59((uint32_t)bytes.ptr[4U] >> 3U);
  return (KRML_CLITERAL(int16_t_x8){.fst = v0,
                                    .snd = v1,
                                    .thd = v2,
                                    .f3 = v3,
                                    .f4 = v4,
                                    .f5 = v5,
                                    .f6 = v6,
                                    .f7 = v7});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_serialize_deserialize_5(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t_x8 v0_7 = libcrux_ml_kem_vector_portable_serialize_deserialize_5_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                         .end = (size_t)5U})));
  int16_t_x8 v8_15 = libcrux_ml_kem_vector_portable_serialize_deserialize_5_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)5U,
                                                         .end = (size_t)10U})));
  return (KRML_CLITERAL(Eurydice_arr_d6){
      .data = {v0_7.fst, v0_7.snd, v0_7.thd, v0_7.f3, v0_7.f4, v0_7.f5, v0_7.f6,
               v0_7.f7, v8_15.fst, v8_15.snd, v8_15.thd, v8_15.f3, v8_15.f4,
               v8_15.f5, v8_15.f6, v8_15.f7}});
}

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_5(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_serialize_deserialize_5(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_5_b8(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_deserialize_5(a);
}

static __attribute__((unused)) KRML_MUSTINLINE uint8_t_x5
libcrux_ml_kem_vector_portable_serialize_serialize_10_int(
    Eurydice_borrow_slice_i16 v) {
  uint8_t r0 = libcrux_secrets_int_as_u8_f5(v.ptr[0U] & 255);
  uint8_t r1 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[1U] & 63) << 2U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[0U] >> 8U & 3);
  uint8_t r2 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[2U] & 15) << 4U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[1U] >> 6U & 15);
  uint8_t r3 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[3U] & 3) << 6U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[2U] >> 4U & 63);
  uint8_t r4 = libcrux_secrets_int_as_u8_f5(v.ptr[3U] >> 2U & 255);
  return (KRML_CLITERAL(uint8_t_x5){
      .fst = r0, .snd = r1, .thd = r2, .f3 = r3, .f4 = r4});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_fc
libcrux_ml_kem_vector_portable_serialize_serialize_10(Eurydice_arr_d6 v) {
  uint8_t_x5 r0_4 = libcrux_ml_kem_vector_portable_serialize_serialize_10_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                      .end = (size_t)4U})));
  uint8_t_x5 r5_9 = libcrux_ml_kem_vector_portable_serialize_serialize_10_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)4U,
                                                      .end = (size_t)8U})));
  uint8_t_x5 r10_14 = libcrux_ml_kem_vector_portable_serialize_serialize_10_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)8U,
                                                      .end = (size_t)12U})));
  uint8_t_x5 r15_19 = libcrux_ml_kem_vector_portable_serialize_serialize_10_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)12U,
                                                      .end = (size_t)16U})));
  return (KRML_CLITERAL(Eurydice_arr_fc){
      .data = {r0_4.fst,   r0_4.snd,   r0_4.thd,   r0_4.f3,   r0_4.f4,
               r5_9.fst,   r5_9.snd,   r5_9.thd,   r5_9.f3,   r5_9.f4,
               r10_14.fst, r10_14.snd, r10_14.thd, r10_14.f3, r10_14.f4,
               r15_19.fst, r15_19.snd, r15_19.thd, r15_19.f3, r15_19.f4}});
}

static __attribute__((unused)) Eurydice_arr_fc libcrux_ml_kem_vector_portable_serialize_10(Eurydice_arr_d6 a) {
  return libcrux_secrets_int_public_integers_declassify_d8_2b(
      libcrux_ml_kem_vector_portable_serialize_serialize_10(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_fc libcrux_ml_kem_vector_portable_serialize_10_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_serialize_10(a);
}

static __attribute__((unused)) KRML_MUSTINLINE int16_t_x8
libcrux_ml_kem_vector_portable_serialize_deserialize_10_int(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t r0 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[1U]) & 3)
                << 8U) |
      (libcrux_secrets_int_as_i16_59(bytes.ptr[0U]) & 255));
  int16_t r1 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[2U]) & 15)
                << 6U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[1U]) >> 2U);
  int16_t r2 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[3U]) & 63)
                << 4U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[2U]) >> 4U);
  int16_t r3 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)libcrux_secrets_int_as_i16_59(bytes.ptr[4U]) << 2U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[3U]) >> 6U);
  int16_t r4 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[6U]) & 3)
                << 8U) |
      (libcrux_secrets_int_as_i16_59(bytes.ptr[5U]) & 255));
  int16_t r5 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[7U]) & 15)
                << 6U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[6U]) >> 2U);
  int16_t r6 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[8U]) & 63)
                << 4U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[7U]) >> 4U);
  int16_t r7 = libcrux_secrets_int_as_i16_f5(
      (int16_t)((uint32_t)libcrux_secrets_int_as_i16_59(bytes.ptr[9U]) << 2U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[8U]) >> 6U);
  return (KRML_CLITERAL(int16_t_x8){.fst = r0,
                                    .snd = r1,
                                    .thd = r2,
                                    .f3 = r3,
                                    .f4 = r4,
                                    .f5 = r5,
                                    .f6 = r6,
                                    .f7 = r7});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_serialize_deserialize_10(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t_x8 v0_7 = libcrux_ml_kem_vector_portable_serialize_deserialize_10_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                         .end = (size_t)10U})));
  int16_t_x8 v8_15 =
      libcrux_ml_kem_vector_portable_serialize_deserialize_10_int(
          Eurydice_slice_subslice_shared_c8(
              bytes, (KRML_CLITERAL(core_ops_range_Range_87){
                         .start = (size_t)10U, .end = (size_t)20U})));
  return (KRML_CLITERAL(Eurydice_arr_d6){
      .data = {v0_7.fst, v0_7.snd, v0_7.thd, v0_7.f3, v0_7.f4, v0_7.f5, v0_7.f6,
               v0_7.f7, v8_15.fst, v8_15.snd, v8_15.thd, v8_15.f3, v8_15.f4,
               v8_15.f5, v8_15.f6, v8_15.f7}});
}

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_10(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_serialize_deserialize_10(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_10_b8(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_deserialize_10(a);
}

static __attribute__((unused)) KRML_MUSTINLINE uint8_t_x11
libcrux_ml_kem_vector_portable_serialize_serialize_11_int(
    Eurydice_borrow_slice_i16 v) {
  uint8_t r0 = libcrux_secrets_int_as_u8_f5(v.ptr[0U]);
  uint8_t r1 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[1U] & 31) << 3U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[0U] >> 8U);
  uint8_t r2 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[2U] & 3) << 6U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[1U] >> 5U);
  uint8_t r3 = libcrux_secrets_int_as_u8_f5(v.ptr[2U] >> 2U & 255);
  uint8_t r4 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[3U] & 127) << 1U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[2U] >> 10U);
  uint8_t r5 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[4U] & 15) << 4U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[3U] >> 7U);
  uint8_t r6 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[5U] & 1) << 7U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[4U] >> 4U);
  uint8_t r7 = libcrux_secrets_int_as_u8_f5(v.ptr[5U] >> 1U & 255);
  uint8_t r8 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[6U] & 63) << 2U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[5U] >> 9U);
  uint8_t r9 = (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[7U] & 7) << 5U |
               (uint32_t)libcrux_secrets_int_as_u8_f5(v.ptr[6U] >> 6U);
  uint8_t r10 = libcrux_secrets_int_as_u8_f5(v.ptr[7U] >> 3U);
  return (KRML_CLITERAL(uint8_t_x11){.fst = r0,
                                     .snd = r1,
                                     .thd = r2,
                                     .f3 = r3,
                                     .f4 = r4,
                                     .f5 = r5,
                                     .f6 = r6,
                                     .f7 = r7,
                                     .f8 = r8,
                                     .f9 = r9,
                                     .f10 = r10});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_80
libcrux_ml_kem_vector_portable_serialize_serialize_11(Eurydice_arr_d6 v) {
  uint8_t_x11 r0_10 = libcrux_ml_kem_vector_portable_serialize_serialize_11_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                      .end = (size_t)8U})));
  uint8_t_x11 r11_21 =
      libcrux_ml_kem_vector_portable_serialize_serialize_11_int(
          Eurydice_array_to_subslice_shared_e7(
              &v, (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = (size_t)8U, .end = (size_t)16U})));
  return (KRML_CLITERAL(Eurydice_arr_80){
      .data = {r0_10.fst, r0_10.snd,  r0_10.thd,  r0_10.f3,   r0_10.f4,
               r0_10.f5,  r0_10.f6,   r0_10.f7,   r0_10.f8,   r0_10.f9,
               r0_10.f10, r11_21.fst, r11_21.snd, r11_21.thd, r11_21.f3,
               r11_21.f4, r11_21.f5,  r11_21.f6,  r11_21.f7,  r11_21.f8,
               r11_21.f9, r11_21.f10}});
}

static __attribute__((unused)) Eurydice_arr_80 libcrux_ml_kem_vector_portable_serialize_11(Eurydice_arr_d6 a) {
  return libcrux_secrets_int_public_integers_declassify_d8_0b(
      libcrux_ml_kem_vector_portable_serialize_serialize_11(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_80 libcrux_ml_kem_vector_portable_serialize_11_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_serialize_11(a);
}

static __attribute__((unused)) KRML_MUSTINLINE int16_t_x8
libcrux_ml_kem_vector_portable_serialize_deserialize_11_int(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t r0 =
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[1U]) & 7)
                << 8U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[0U]);
  int16_t r1 =
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[2U]) & 63)
                << 5U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[1U]) >> 3U;
  int16_t r2 =
      ((int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[4U]) & 1)
                 << 10U) |
       (int16_t)((uint32_t)libcrux_secrets_int_as_i16_59(bytes.ptr[3U])
                 << 2U)) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[2U]) >> 6U;
  int16_t r3 =
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[5U]) & 15)
                << 7U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[4U]) >> 1U;
  int16_t r4 =
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[6U]) & 127)
                << 4U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[5U]) >> 4U;
  int16_t r5 =
      ((int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[8U]) & 3)
                 << 9U) |
       (int16_t)((uint32_t)libcrux_secrets_int_as_i16_59(bytes.ptr[7U])
                 << 1U)) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[6U]) >> 7U;
  int16_t r6 =
      (int16_t)((uint32_t)(libcrux_secrets_int_as_i16_59(bytes.ptr[9U]) & 31)
                << 6U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[8U]) >> 2U;
  int16_t r7 =
      (int16_t)((uint32_t)libcrux_secrets_int_as_i16_59(bytes.ptr[10U]) << 3U) |
      libcrux_secrets_int_as_i16_59(bytes.ptr[9U]) >> 5U;
  return (KRML_CLITERAL(int16_t_x8){.fst = r0,
                                    .snd = r1,
                                    .thd = r2,
                                    .f3 = r3,
                                    .f4 = r4,
                                    .f5 = r5,
                                    .f6 = r6,
                                    .f7 = r7});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_serialize_deserialize_11(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t_x8 v0_7 = libcrux_ml_kem_vector_portable_serialize_deserialize_11_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                         .end = (size_t)11U})));
  int16_t_x8 v8_15 =
      libcrux_ml_kem_vector_portable_serialize_deserialize_11_int(
          Eurydice_slice_subslice_shared_c8(
              bytes, (KRML_CLITERAL(core_ops_range_Range_87){
                         .start = (size_t)11U, .end = (size_t)22U})));
  return (KRML_CLITERAL(Eurydice_arr_d6){
      .data = {v0_7.fst, v0_7.snd, v0_7.thd, v0_7.f3, v0_7.f4, v0_7.f5, v0_7.f6,
               v0_7.f7, v8_15.fst, v8_15.snd, v8_15.thd, v8_15.f3, v8_15.f4,
               v8_15.f5, v8_15.f6, v8_15.f7}});
}

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_11(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_serialize_deserialize_11(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_11_b8(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_deserialize_11(a);
}

static __attribute__((unused)) KRML_MUSTINLINE uint8_t_x3
libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
    Eurydice_borrow_slice_i16 v) {
  uint8_t r0 = libcrux_secrets_int_as_u8_f5(v.ptr[0U] & 255);
  uint8_t r1 = libcrux_secrets_int_as_u8_f5(
      v.ptr[0U] >> 8U | (int16_t)((uint32_t)(v.ptr[1U] & 15) << 4U));
  uint8_t r2 = libcrux_secrets_int_as_u8_f5(v.ptr[1U] >> 4U & 255);
  return (KRML_CLITERAL(uint8_t_x3){.fst = r0, .snd = r1, .thd = r2});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_94
libcrux_ml_kem_vector_portable_serialize_serialize_12(Eurydice_arr_d6 v) {
  uint8_t_x3 r0_2 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                      .end = (size_t)2U})));
  uint8_t_x3 r3_5 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)2U,
                                                      .end = (size_t)4U})));
  uint8_t_x3 r6_8 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)4U,
                                                      .end = (size_t)6U})));
  uint8_t_x3 r9_11 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)6U,
                                                      .end = (size_t)8U})));
  uint8_t_x3 r12_14 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)8U,
                                                      .end = (size_t)10U})));
  uint8_t_x3 r15_17 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)10U,
                                                      .end = (size_t)12U})));
  uint8_t_x3 r18_20 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)12U,
                                                      .end = (size_t)14U})));
  uint8_t_x3 r21_23 = libcrux_ml_kem_vector_portable_serialize_serialize_12_int(
      Eurydice_array_to_subslice_shared_e7(
          &v, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)14U,
                                                      .end = (size_t)16U})));
  return (KRML_CLITERAL(Eurydice_arr_94){
      .data = {r0_2.fst,   r0_2.snd,   r0_2.thd,   r3_5.fst,   r3_5.snd,
               r3_5.thd,   r6_8.fst,   r6_8.snd,   r6_8.thd,   r9_11.fst,
               r9_11.snd,  r9_11.thd,  r12_14.fst, r12_14.snd, r12_14.thd,
               r15_17.fst, r15_17.snd, r15_17.thd, r18_20.fst, r18_20.snd,
               r18_20.thd, r21_23.fst, r21_23.snd, r21_23.thd}});
}

static __attribute__((unused)) Eurydice_arr_94 libcrux_ml_kem_vector_portable_serialize_12(Eurydice_arr_d6 a) {
  return libcrux_secrets_int_public_integers_declassify_d8_40(
      libcrux_ml_kem_vector_portable_serialize_serialize_12(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_94 libcrux_ml_kem_vector_portable_serialize_12_b8(
    Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_serialize_12(a);
}

static __attribute__((unused)) KRML_MUSTINLINE int16_t_x2
libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t byte0 = libcrux_secrets_int_as_i16_59(bytes.ptr[0U]);
  int16_t byte1 = libcrux_secrets_int_as_i16_59(bytes.ptr[1U]);
  int16_t byte2 = libcrux_secrets_int_as_i16_59(bytes.ptr[2U]);
  int16_t r0 = (int16_t)((uint32_t)(byte1 & 15) << 8U) | (byte0 & 255);
  int16_t r1 = (int16_t)((uint32_t)byte2 << 4U) | (byte1 >> 4U & 15);
  return (KRML_CLITERAL(int16_t_x2){.fst = r0, .snd = r1});
}

static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
libcrux_ml_kem_vector_portable_serialize_deserialize_12(
    Eurydice_borrow_slice_u8 bytes) {
  int16_t_x2 v0_1 = libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                         .end = (size_t)3U})));
  int16_t_x2 v2_3 = libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)3U,
                                                         .end = (size_t)6U})));
  int16_t_x2 v4_5 = libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)6U,
                                                         .end = (size_t)9U})));
  int16_t_x2 v6_7 = libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)9U,
                                                         .end = (size_t)12U})));
  int16_t_x2 v8_9 = libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
      Eurydice_slice_subslice_shared_c8(
          bytes, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)12U,
                                                         .end = (size_t)15U})));
  int16_t_x2 v10_11 =
      libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
          Eurydice_slice_subslice_shared_c8(
              bytes, (KRML_CLITERAL(core_ops_range_Range_87){
                         .start = (size_t)15U, .end = (size_t)18U})));
  int16_t_x2 v12_13 =
      libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
          Eurydice_slice_subslice_shared_c8(
              bytes, (KRML_CLITERAL(core_ops_range_Range_87){
                         .start = (size_t)18U, .end = (size_t)21U})));
  int16_t_x2 v14_15 =
      libcrux_ml_kem_vector_portable_serialize_deserialize_12_int(
          Eurydice_slice_subslice_shared_c8(
              bytes, (KRML_CLITERAL(core_ops_range_Range_87){
                         .start = (size_t)21U, .end = (size_t)24U})));
  return (KRML_CLITERAL(Eurydice_arr_d6){
      .data = {v0_1.fst, v0_1.snd, v2_3.fst, v2_3.snd, v4_5.fst, v4_5.snd,
               v6_7.fst, v6_7.snd, v8_9.fst, v8_9.snd, v10_11.fst, v10_11.snd,
               v12_13.fst, v12_13.snd, v14_15.fst, v14_15.snd}});
}

static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_12(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_serialize_deserialize_12(
      libcrux_secrets_int_classify_public_classify_ref_6d_90(a));
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) Eurydice_arr_d6 libcrux_ml_kem_vector_portable_deserialize_12_b8(
    Eurydice_borrow_slice_u8 a) {
  return libcrux_ml_kem_vector_portable_deserialize_12(a);
}

static __attribute__((unused)) KRML_MUSTINLINE size_t libcrux_ml_kem_vector_portable_sampling_rej_sample(
    Eurydice_borrow_slice_u8 a, Eurydice_mut_borrow_slice_i16 result) {
  size_t sampled = (size_t)0U;
  for (size_t i = (size_t)0U; i < a.meta / (size_t)3U; i++) {
    size_t i0 = i;
    int16_t b1 = (int16_t)(uint32_t)a.ptr[i0 * (size_t)3U + (size_t)0U];
    int16_t b2 = (int16_t)(uint32_t)a.ptr[i0 * (size_t)3U + (size_t)1U];
    int16_t b3 = (int16_t)(uint32_t)a.ptr[i0 * (size_t)3U + (size_t)2U];
    int16_t d1 = (int16_t)((uint32_t)(b2 & 15) << 8U) | b1;
    int16_t d2 = (int16_t)((uint32_t)b3 << 4U) | b2 >> 4U;
    if (d1 < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS) {
      if (sampled < (size_t)16U) {
        result.ptr[sampled] = d1;
        sampled++;
      }
    }
    if (d2 < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS) {
      if (sampled < (size_t)16U) {
        result.ptr[sampled] = d2;
        sampled++;
      }
    }
  }
  return sampled;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) size_t libcrux_ml_kem_vector_portable_rej_sample_b8(
    Eurydice_borrow_slice_u8 a, Eurydice_mut_borrow_slice_i16 out) {
  return libcrux_ml_kem_vector_portable_sampling_rej_sample(a, out);
}

/**
This function found in impl {core::clone::Clone for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
static __attribute__((unused)) inline Eurydice_arr_d6 libcrux_ml_kem_vector_portable_vector_type_clone_9c(
    const Eurydice_arr_d6 *self) {
  return self[0U];
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.ZERO_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) Eurydice_arr_9e ZERO_d6_ea(void) {
  Eurydice_arr_9e lit;
  Eurydice_arr_d6 repeat_expression[16U];
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U,
      repeat_expression[i] = libcrux_ml_kem_vector_portable_ZERO_b8(););
  memcpy(lit.data, repeat_expression, (size_t)16U * sizeof(Eurydice_arr_d6));
  return lit;
}

/**
 Only use with public values.

 This MUST NOT be used with secret inputs, like its caller
 `deserialize_ring_elements_reduced`.
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_to_reduced_ring_element with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_to_reduced_ring_element_ea(Eurydice_borrow_slice_u8 serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  for (size_t i = (size_t)0U; i < serialized.meta / (size_t)24U; i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 bytes = Eurydice_slice_subslice_shared_c8(
        serialized,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * (size_t)24U, .end = i0 * (size_t)24U + (size_t)24U}));
    Eurydice_arr_d6 coefficient =
        libcrux_ml_kem_vector_portable_deserialize_12_b8(bytes);
    Eurydice_arr_d6 uu____0 =
        libcrux_ml_kem_vector_portable_cond_subtract_3329_b8(coefficient);
    re.data[i0] = uu____0;
  }
  return re;
}

/**
 See [deserialize_ring_elements_reduced_out].
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_ring_elements_reduced with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void deserialize_ring_elements_reduced_ee(
    Eurydice_borrow_slice_u8 public_key, Eurydice_arr_d20 *deserialized_pk) {
  for (size_t i = (size_t)0U;
       i < public_key.meta / LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT;
       i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 ring_element = Eurydice_slice_subslice_shared_c8(
        public_key,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT,
            .end = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT +
                   LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT}));
    Eurydice_arr_9e uu____0 =
        deserialize_to_reduced_ring_element_ea(ring_element);
    deserialized_pk->data[i0] = uu____0;
  }
}

/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_init_absorb_final with const
generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_4a shake128_init_absorb_final_23(
    const Eurydice_arr_56 *input) {
  Eurydice_arr_4a shake128_state;
  Eurydice_arr_7c repeat_expression[4U];
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  repeat_expression[i] =
                      libcrux_sha3_portable_incremental_shake128_init(););
  memcpy(shake128_state.data, repeat_expression,
         (size_t)4U * sizeof(Eurydice_arr_7c));
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
                  libcrux_sha3_portable_incremental_shake128_absorb_final(
                      &shake128_state.data[i0],
                      Eurydice_array_to_slice_shared_e9(&input->data[i0])););
  return shake128_state;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_init_absorb_final_4a with const
generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_4a shake128_init_absorb_final_4a_23(
    const Eurydice_arr_56 *input) {
  return shake128_init_absorb_final_23(input);
}

/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_first_three_blocks with
const generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_7c0 shake128_squeeze_first_three_blocks_23(
    Eurydice_arr_4a *st) {
  Eurydice_arr_7c0 out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      libcrux_sha3_portable_incremental_shake128_squeeze_first_three_blocks(
          &st->data[i0], Eurydice_array_to_slice_mut_48(&out.data[i0])););
  return out;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_first_three_blocks_4a
with const generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_7c0 shake128_squeeze_first_three_blocks_4a_23(
    Eurydice_arr_4a *self) {
  return shake128_squeeze_first_three_blocks_23(self);
}

/**
 If `bytes` contains a set of uniformly random bytes, this function
 uniformly samples a ring element `â` that is treated as being the NTT
 representation of the corresponding polynomial `a`.

 Since rejection sampling is used, it is possible the supplied bytes are
 not enough to sample the element, in which case an `Err` is returned and the
 caller must try again with a fresh set of bytes.

 This function <strong>partially</strong> implements <strong>Algorithm
 6</strong> of the NIST FIPS 203 standard, We say "partially" because this
 implementation only accepts a finite set of bytes as input and returns an error
 if the set is not enough; Algorithm 6 of the FIPS 203 standard on the other
 hand samples from an infinite stream of bytes until the ring element is filled.
 Algorithm 6 is reproduced below:

 ```plaintext
 Input: byte stream B ∈ 𝔹*.
 Output: array â ∈ ℤ₂₅₆.

 i ← 0
 j ← 0
 while j < 256 do
     d₁ ← B[i] + 256·(B[i+1] mod 16)
     d₂ ← ⌊B[i+1]/16⌋ + 16·B[i+2]
     if d₁ < q then
         â[j] ← d₁
         j ← j + 1
     end if
     if d₂ < q and j < 256 then
         â[j] ← d₂
         j ← j + 1
     end if
     i ← i + 3
 end while
 return â
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of
libcrux_ml_kem.sampling.sample_from_uniform_distribution_next with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- N= 504
*/
static __attribute__((unused)) KRML_MUSTINLINE bool sample_from_uniform_distribution_next_1c(
    const Eurydice_arr_7c0 *randomness, Eurydice_arr_cc *sampled_coefficients,
    Eurydice_arr_24 *out) {
  KRML_MAYBE_FOR4(
      i0, (size_t)0U, (size_t)4U, (size_t)1U, size_t i1 = i0;
      for (size_t i = (size_t)0U; i < (size_t)504U / (size_t)24U; i++) {
        size_t r = i;
        if (sampled_coefficients->data[i1] <
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
          size_t sampled = libcrux_ml_kem_vector_portable_rej_sample_b8(
              Eurydice_array_to_subslice_shared_d40(
                  &randomness->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = r * (size_t)24U,
                      .end = r * (size_t)24U + (size_t)24U})),
              Eurydice_array_to_subslice_mut_e7(
                  &out->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = sampled_coefficients->data[i1],
                      .end = sampled_coefficients->data[i1] + (size_t)16U})));
          size_t uu____0 = i1;
          sampled_coefficients->data[uu____0] += sampled;
        }
      });
  bool done = true;
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      if (sampled_coefficients->data[i0] >=
          LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
        sampled_coefficients->data[i0] =
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT;
      } else { done = false; });
  return done;
}

/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_next_block with const
generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_9c shake128_squeeze_next_block_23(
    Eurydice_arr_4a *st) {
  Eurydice_arr_9c out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      libcrux_sha3_portable_incremental_shake128_squeeze_next_block(
          &st->data[i0], Eurydice_array_to_slice_mut_2c(&out.data[i0])););
  return out;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_next_block_4a with const
generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_9c shake128_squeeze_next_block_4a_23(
    Eurydice_arr_4a *self) {
  return shake128_squeeze_next_block_23(self);
}

/**
 If `bytes` contains a set of uniformly random bytes, this function
 uniformly samples a ring element `â` that is treated as being the NTT
 representation of the corresponding polynomial `a`.

 Since rejection sampling is used, it is possible the supplied bytes are
 not enough to sample the element, in which case an `Err` is returned and the
 caller must try again with a fresh set of bytes.

 This function <strong>partially</strong> implements <strong>Algorithm
 6</strong> of the NIST FIPS 203 standard, We say "partially" because this
 implementation only accepts a finite set of bytes as input and returns an error
 if the set is not enough; Algorithm 6 of the FIPS 203 standard on the other
 hand samples from an infinite stream of bytes until the ring element is filled.
 Algorithm 6 is reproduced below:

 ```plaintext
 Input: byte stream B ∈ 𝔹*.
 Output: array â ∈ ℤ₂₅₆.

 i ← 0
 j ← 0
 while j < 256 do
     d₁ ← B[i] + 256·(B[i+1] mod 16)
     d₂ ← ⌊B[i+1]/16⌋ + 16·B[i+2]
     if d₁ < q then
         â[j] ← d₁
         j ← j + 1
     end if
     if d₂ < q and j < 256 then
         â[j] ← d₂
         j ← j + 1
     end if
     i ← i + 3
 end while
 return â
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of
libcrux_ml_kem.sampling.sample_from_uniform_distribution_next with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- N= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE bool sample_from_uniform_distribution_next_1c0(
    const Eurydice_arr_9c *randomness, Eurydice_arr_cc *sampled_coefficients,
    Eurydice_arr_24 *out) {
  KRML_MAYBE_FOR4(
      i0, (size_t)0U, (size_t)4U, (size_t)1U, size_t i1 = i0;
      for (size_t i = (size_t)0U; i < (size_t)168U / (size_t)24U; i++) {
        size_t r = i;
        if (sampled_coefficients->data[i1] <
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
          size_t sampled = libcrux_ml_kem_vector_portable_rej_sample_b8(
              Eurydice_array_to_subslice_shared_d41(
                  &randomness->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = r * (size_t)24U,
                      .end = r * (size_t)24U + (size_t)24U})),
              Eurydice_array_to_subslice_mut_e7(
                  &out->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = sampled_coefficients->data[i1],
                      .end = sampled_coefficients->data[i1] + (size_t)16U})));
          size_t uu____0 = i1;
          sampled_coefficients->data[uu____0] += sampled;
        }
      });
  bool done = true;
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      if (sampled_coefficients->data[i0] >=
          LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
        sampled_coefficients->data[i0] =
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT;
      } else { done = false; });
  return done;
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.ZERO
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) Eurydice_arr_9e ZERO_ea(void) {
  Eurydice_arr_9e lit;
  Eurydice_arr_d6 repeat_expression[16U];
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U,
      repeat_expression[i] = libcrux_ml_kem_vector_portable_ZERO_b8(););
  memcpy(lit.data, repeat_expression, (size_t)16U * sizeof(Eurydice_arr_d6));
  return lit;
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.from_i16_array
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
from_i16_array_ea(Eurydice_borrow_slice_i16 a) {
  Eurydice_arr_9e result = ZERO_ea();
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 uu____0 = libcrux_ml_kem_vector_portable_from_i16_array_b8(
        Eurydice_slice_subslice_shared_a6(
            a, (KRML_CLITERAL(core_ops_range_Range_87){
                   .start = i0 * (size_t)16U,
                   .end = (i0 + (size_t)1U) * (size_t)16U})));
    result.data[i0] = uu____0;
  }
  return result;
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.from_i16_array_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
from_i16_array_d6_ea(Eurydice_borrow_slice_i16 a) {
  return from_i16_array_ea(a);
}

/**
This function found in impl {core::ops::function::FnMut<([i16; 272usize]),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]> for libcrux_ml_kem::sampling::sample_from_xof::closure<Vector,
Hasher, K>[TraitClause@0, TraitClause@1, TraitClause@2, TraitClause@3]}
*/
/**
A monomorphic instance of libcrux_ml_kem.sampling.sample_from_xof.call_mut_0a
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_0a_910(Eurydice_arr_5b tupled_args) {
  Eurydice_arr_5b s = tupled_args;
  return from_i16_array_d6_ea(Eurydice_array_to_subslice_shared_e70(
      &s, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = (size_t)256U})));
}

/**
A monomorphic instance of libcrux_ml_kem.sampling.sample_from_xof
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d20
sample_from_xof_910(const Eurydice_arr_56 *seeds) {
  Eurydice_arr_cc sampled_coefficients = {.data = {0U}};
  Eurydice_arr_24 out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  Eurydice_arr_4a xof_state = shake128_init_absorb_final_4a_23(seeds);
  Eurydice_arr_7c0 randomness0 =
      shake128_squeeze_first_three_blocks_4a_23(&xof_state);
  bool done = sample_from_uniform_distribution_next_1c(
      &randomness0, &sampled_coefficients, &out);
  while (true) {
    if (done) {
      break;
    } else {
      Eurydice_arr_9c randomness =
          shake128_squeeze_next_block_4a_23(&xof_state);
      done = sample_from_uniform_distribution_next_1c0(
          &randomness, &sampled_coefficients, &out);
    }
  }
  Eurydice_arr_d20 arr_mapped_str;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  arr_mapped_str.data[i] = call_mut_0a_910(out.data[i]););
  return arr_mapped_str;
}

/**
A monomorphic instance of libcrux_ml_kem.matrix.sample_matrix_A
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void sample_matrix_A_910(Eurydice_arr_60 *A_transpose,
                                                const Eurydice_arr_31 *seed,
                                                bool transpose) {
  KRML_MAYBE_FOR4(
      i0, (size_t)0U, (size_t)4U, (size_t)1U, size_t i1 = i0;
      Eurydice_arr_56 seeds; Eurydice_arr_31 repeat_expression[4U];
      KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                      repeat_expression[i] =
                          core_array__core__clone__Clone_for__T__N___clone(
                              (size_t)34U, seed, uint8_t, Eurydice_arr_31););
      memcpy(seeds.data, repeat_expression,
             (size_t)4U * sizeof(Eurydice_arr_31));
      KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t j = i;
                      seeds.data[j].data[32U] = (uint8_t)i1;
                      seeds.data[j].data[33U] = (uint8_t)j;);
      Eurydice_arr_d20 sampled = sample_from_xof_910(&seeds); KRML_MAYBE_FOR4(
          i, (size_t)0U, (size_t)4U, (size_t)1U, size_t j = i;
          Eurydice_arr_9e sample = sampled.data[j]; if (transpose) {
            A_transpose->data[j].data[i1] = sample;
          } else { A_transpose->data[i1].data[j] = sample; }););
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.H_4a
with const generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_ec H_4a_23(Eurydice_borrow_slice_u8 input) {
  return libcrux_ml_kem_hash_functions_portable_H(input);
}

/**
 Generate an unpacked key from a serialized key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.unpack_public_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- T_AS_NTT_ENCODED_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_unpack_public_key_29(
    const Eurydice_arr_d1 *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
        *unpacked_public_key) {
  Eurydice_borrow_slice_u8 uu____0 =
      Eurydice_array_to_subslice_to_shared_211(public_key, (size_t)1536U);
  deserialize_ring_elements_reduced_ee(
      uu____0, &unpacked_public_key->ind_cpa_public_key.t_as_ntt);
  unpacked_public_key->ind_cpa_public_key.seed_for_A =
      libcrux_ml_kem_utils_into_padded_array_ce(
          Eurydice_array_to_subslice_from_shared_5f2(public_key,
                                                     (size_t)1536U));
  Eurydice_arr_60 *uu____2 = &unpacked_public_key->ind_cpa_public_key.A;
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_31 lvalue = libcrux_ml_kem_utils_into_padded_array_de(
      Eurydice_array_to_subslice_from_shared_5f2(public_key, (size_t)1536U));
  sample_matrix_A_910(uu____2, &lvalue, false);
  Eurydice_arr_ec uu____3 = H_4a_23(Eurydice_array_to_slice_shared_b50(
      libcrux_ml_kem_types_as_slice_e6_d9(public_key)));
  unpacked_public_key->public_key_hash = uu____3;
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.to_unsigned_field_modulus
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
to_unsigned_field_modulus_ea(Eurydice_arr_d6 a) {
  return libcrux_ml_kem_vector_portable_to_unsigned_representative_b8(a);
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.serialize_uncompressed_ring_element with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_b2
serialize_uncompressed_ring_element_ea(const Eurydice_arr_9e *re) {
  Eurydice_arr_b2 serialized = {.data = {0U}};
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficient = to_unsigned_field_modulus_ea(re->data[i0]);
    Eurydice_arr_94 bytes =
        libcrux_ml_kem_vector_portable_serialize_12_b8(coefficient);
    Eurydice_slice_copy(
        Eurydice_array_to_subslice_mut_d411(
            &serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                             .start = (size_t)24U * i0,
                             .end = (size_t)24U * i0 + (size_t)24U})),
        Eurydice_array_to_slice_shared_ed(&bytes), uint8_t);
  }
  return serialized;
}

/**
 Call [`serialize_uncompressed_ring_element`] for each ring element.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_vector
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void serialize_vector_ee(
    const Eurydice_arr_d20 *key, Eurydice_mut_borrow_slice_u8 out) {
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e re = key->data[i0];
      Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
          out,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT,
              .end = (i0 + (size_t)1U) *
                     LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT}));
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_b2 lvalue = serialize_uncompressed_ring_element_ea(&re);
      Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_a9(&lvalue),
                          uint8_t););
}

/**
 Concatenate `t` and `ρ` into the public key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_public_key_mut
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE void serialize_public_key_mut_1c(
    const Eurydice_arr_d20 *t_as_ntt, Eurydice_borrow_slice_u8 seed_for_a,
    Eurydice_arr_d1 *serialized) {
  serialize_vector_ee(
      t_as_ntt,
      Eurydice_array_to_subslice_mut_d416(
          serialized,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end = libcrux_ml_kem_constants_ranked_bytes_per_ring_element(
                  (size_t)4U)})));
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_from_mut_5f4(
          serialized,
          libcrux_ml_kem_constants_ranked_bytes_per_ring_element((size_t)4U)),
      seed_for_a, uint8_t);
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.serialized_mut_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_1c(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *self,
    Eurydice_arr_d1 *serialized) {
  serialize_public_key_mut_1c(
      &self->ind_cpa_public_key.t_as_ntt,
      Eurydice_array_to_slice_shared_01(&self->ind_cpa_public_key.seed_for_A),
      serialized);
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_mut_11_1c(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self,
    Eurydice_arr_d1 *serialized) {
  libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_1c(&self->public_key,
                                                       serialized);
}

/**
 Concatenate `t` and `ρ` into the public key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d1 serialize_public_key_1c(
    const Eurydice_arr_d20 *t_as_ntt, Eurydice_borrow_slice_u8 seed_for_a) {
  Eurydice_arr_d1 public_key_serialized = {.data = {0U}};
  serialize_public_key_mut_1c(t_as_ntt, seed_for_a, &public_key_serialized);
  return public_key_serialized;
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.serialized_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d1 serialized_dd_1c(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *self) {
  return libcrux_ml_kem_types_from_51_d9(serialize_public_key_1c(
      &self->ind_cpa_public_key.t_as_ntt,
      Eurydice_array_to_slice_shared_01(&self->ind_cpa_public_key.seed_for_A)));
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_d1 libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_11_1c(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self) {
  return serialized_dd_1c(&self->public_key);
}

/**
 Serialize the secret key from the unpacked key pair generation.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_unpacked_secret_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PRIVATE_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) libcrux_ml_kem_utils_extraction_helper_Keypair1024
serialize_unpacked_secret_key_1c(
    const libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94
        *public_key,
    const Eurydice_arr_d20 *private_key) {
  Eurydice_arr_d1 public_key_serialized = serialize_public_key_1c(
      &public_key->t_as_ntt,
      Eurydice_array_to_slice_shared_01(&public_key->seed_for_A));
  Eurydice_arr_df secret_key_serialized = {.data = {0U}};
  serialize_vector_ee(private_key,
                      Eurydice_array_to_slice_mut_2f(&secret_key_serialized));
  return (KRML_CLITERAL(libcrux_ml_kem_utils_extraction_helper_Keypair1024){
      .fst = secret_key_serialized, .snd = public_key_serialized});
}

/**
 Serialize the secret key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.serialize_kem_secret_key_mut
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- SERIALIZED_KEY_LEN= 3168
*/
static __attribute__((unused)) KRML_MUSTINLINE void serialize_kem_secret_key_mut_4c(
    Eurydice_borrow_slice_u8 private_key, Eurydice_borrow_slice_u8 public_key,
    Eurydice_borrow_slice_u8 implicit_rejection_value,
    Eurydice_arr_a8 *serialized) {
  size_t pointer = (size_t)0U;
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d417(
                          serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                                          .start = pointer,
                                          .end = pointer + private_key.meta})),
                      private_key, uint8_t);
  pointer += private_key.meta;
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_mut_d417(
          serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                          .start = pointer, .end = pointer + public_key.meta})),
      public_key, uint8_t);
  pointer += public_key.meta;
  Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_array_to_subslice_mut_d417(
      serialized,
      (KRML_CLITERAL(core_ops_range_Range_87){
          .start = pointer,
          .end = pointer + LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE}));
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_ec lvalue = H_4a_23(public_key);
  Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_01(&lvalue),
                      uint8_t);
  pointer += LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE;
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_mut_d417(
          serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                          .start = pointer,
                          .end = pointer + implicit_rejection_value.meta})),
      implicit_rejection_value, uint8_t);
}

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_2e(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self,
    Eurydice_arr_a8 *serialized) {
  libcrux_ml_kem_utils_extraction_helper_Keypair1024 uu____0 =
      serialize_unpacked_secret_key_1c(&self->public_key.ind_cpa_public_key,
                                       &self->private_key.ind_cpa_private_key);
  Eurydice_arr_df ind_cpa_private_key = uu____0.fst;
  Eurydice_arr_d1 ind_cpa_public_key = uu____0.snd;
  serialize_kem_secret_key_mut_4c(
      Eurydice_array_to_slice_shared_2f(&ind_cpa_private_key),
      Eurydice_array_to_slice_shared_b50(&ind_cpa_public_key),
      Eurydice_array_to_slice_shared_01(
          &self->private_key.implicit_rejection_value),
      serialized);
}

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) Eurydice_arr_a8 libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_11_2e(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *self) {
  Eurydice_arr_a8 sk = libcrux_ml_kem_types_default_d3_0e();
  libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_2e(self, &sk);
  return sk;
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_to_uncompressed_ring_element with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_to_uncompressed_ring_element_ea(
    Eurydice_borrow_slice_u8 serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  for (size_t i = (size_t)0U; i < serialized.meta / (size_t)24U; i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 bytes = Eurydice_slice_subslice_shared_c8(
        serialized,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * (size_t)24U, .end = i0 * (size_t)24U + (size_t)24U}));
    Eurydice_arr_d6 uu____0 =
        libcrux_ml_kem_vector_portable_deserialize_12_b8(bytes);
    re.data[i0] = uu____0;
  }
  return re;
}

/**
 Call [`deserialize_to_uncompressed_ring_element`] for each ring element.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.deserialize_vector
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void deserialize_vector_ee(
    Eurydice_borrow_slice_u8 secret_key, Eurydice_arr_d20 *secret_as_ntt) {
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e uu____0 = deserialize_to_uncompressed_ring_element_ea(
          Eurydice_slice_subslice_shared_c8(
              secret_key,
              (KRML_CLITERAL(core_ops_range_Range_87){
                  .start = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT,
                  .end = (i0 + (size_t)1U) *
                         LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT})));
      secret_as_ntt->data[i0] = uu____0;);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.build_unpacked_public_key_mut
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- T_AS_NTT_ENCODED_SIZE= 1536
*/
static __attribute__((unused)) KRML_MUSTINLINE void build_unpacked_public_key_mut_050(
    Eurydice_borrow_slice_u8 public_key,
    libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94
        *unpacked_public_key) {
  deserialize_ring_elements_reduced_ee(
      Eurydice_slice_subslice_to_shared_72(public_key, (size_t)1536U),
      &unpacked_public_key->t_as_ntt);
  Eurydice_borrow_slice_u8 seed =
      Eurydice_slice_subslice_from_shared_6d(public_key, (size_t)1536U);
  Eurydice_arr_60 *uu____0 = &unpacked_public_key->A;
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_31 lvalue = libcrux_ml_kem_utils_into_padded_array_de(seed);
  sample_matrix_A_910(uu____0, &lvalue, false);
}

/**
 Take a serialized private key and generate an unpacked key pair from it.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.keys_from_private_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_keys_from_private_key_38(
    const Eurydice_arr_a8 *private_key,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair) {
  Eurydice_borrow_slice_u8_x4 uu____0 =
      libcrux_ml_kem_types_unpack_private_key_e3(
          Eurydice_array_to_slice_shared_68(private_key));
  Eurydice_borrow_slice_u8 ind_cpa_secret_key = uu____0.fst;
  Eurydice_borrow_slice_u8 ind_cpa_public_key = uu____0.snd;
  Eurydice_borrow_slice_u8 ind_cpa_public_key_hash = uu____0.thd;
  Eurydice_borrow_slice_u8 implicit_rejection_value = uu____0.f3;
  deserialize_vector_ee(ind_cpa_secret_key,
                        &key_pair->private_key.ind_cpa_private_key);
  build_unpacked_public_key_mut_050(ind_cpa_public_key,
                                    &key_pair->public_key.ind_cpa_public_key);
  Eurydice_slice_copy(
      Eurydice_array_to_slice_mut_01(&key_pair->public_key.public_key_hash),
      ind_cpa_public_key_hash, uint8_t);
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(
                          &key_pair->private_key.implicit_rejection_value),
                      implicit_rejection_value, uint8_t);
  Eurydice_slice_copy(
      Eurydice_array_to_slice_mut_01(
          &key_pair->public_key.ind_cpa_public_key.seed_for_A),
      Eurydice_slice_subslice_from_shared_6d(ind_cpa_public_key, (size_t)1536U),
      uint8_t);
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cpa::unpacked::IndCpaPrivateKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.unpacked.default_70
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_d20 default_70_ee(void) {
  Eurydice_arr_d20 lit;
  Eurydice_arr_9e repeat_expression[4U];
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  repeat_expression[i] = ZERO_d6_ea(););
  memcpy(lit.data, repeat_expression, (size_t)4U * sizeof(Eurydice_arr_9e));
  return lit;
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cpa::unpacked::IndCpaPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.unpacked.default_8b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94 default_8b_ee(
    void) {
  Eurydice_arr_d20 uu____0;
  Eurydice_arr_9e repeat_expression0[4U];
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  repeat_expression0[i] = ZERO_d6_ea(););
  memcpy(uu____0.data, repeat_expression0,
         (size_t)4U * sizeof(Eurydice_arr_9e));
  Eurydice_arr_ec uu____1 = {.data = {0U}};
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94 lit0;
  lit0.t_as_ntt = uu____0;
  lit0.seed_for_A = uu____1;
  Eurydice_arr_d20 repeat_expression1[4U];
  KRML_MAYBE_FOR4(
      i0, (size_t)0U, (size_t)4U, (size_t)1U, Eurydice_arr_d20 lit;
      Eurydice_arr_9e repeat_expression[4U];
      KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                      repeat_expression[i] = ZERO_d6_ea(););
      memcpy(lit.data, repeat_expression, (size_t)4U * sizeof(Eurydice_arr_9e));
      repeat_expression1[i0] = lit;);
  memcpy(lit0.A.data, repeat_expression1,
         (size_t)4U * sizeof(Eurydice_arr_d20));
  return lit0;
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_30
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
libcrux_ml_kem_ind_cca_unpacked_default_30_ee(void) {
  return (
      KRML_CLITERAL(libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94){
          .ind_cpa_public_key = default_8b_ee(),
          .public_key_hash = {.data = {0U}}});
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_7b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
libcrux_ml_kem_ind_cca_unpacked_default_7b_ee(void) {
  libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_94 uu____0 = {
      .ind_cpa_private_key = default_70_ee(),
      .implicit_rejection_value = {.data = {0U}}};
  return (KRML_CLITERAL(
      libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked){
      .private_key = uu____0,
      .public_key = libcrux_ml_kem_ind_cca_unpacked_default_30_ee()});
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.G_4a
with const generics
- K= 4
*/
static __attribute__((unused)) inline Eurydice_arr_c7 G_4a_23(Eurydice_borrow_slice_u8 input) {
  return libcrux_ml_kem_hash_functions_portable_G(input);
}

/**
This function found in impl {libcrux_ml_kem::variant::Variant for
libcrux_ml_kem::variant::MlKem}
*/
/**
A monomorphic instance of libcrux_ml_kem.variant.cpa_keygen_seed_39
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_c7
cpa_keygen_seed_39_fe(Eurydice_borrow_slice_u8 key_generation_seed) {
  Eurydice_arr_fa seed = {.data = {0U}};
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_mut_d48(
          &seed,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end =
                  LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE})),
      key_generation_seed, uint8_t);
  seed.data[LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE] =
      (uint8_t)(size_t)4U;
  return G_4a_23(Eurydice_array_to_slice_shared_b5(&seed));
}

/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRFxN
with const generics
- K= 4
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_3b PRFxN_f5(const Eurydice_arr_890 *input) {
  Eurydice_arr_3b out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
                  libcrux_sha3_portable_shake256(
                      Eurydice_array_to_slice_mut_78(&out.data[i0]),
                      Eurydice_array_to_slice_shared_b5(&input->data[i0])););
  return out;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRFxN_4a
with const generics
- K= 4
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_3b PRFxN_4a_f5(const Eurydice_arr_890 *input) {
  return PRFxN_f5(input);
}

/**
 Given a series of uniformly random bytes in `randomness`, for some number
 `eta`, the `sample_from_binomial_distribution_{eta}` functions sample a ring
 element from a binomial distribution centered at 0 that uses two sets of `eta`
 coin flips. If, for example, `eta = ETA`, each ring coefficient is a value `v`
 such such that `v ∈ {-ETA, -ETA + 1, ..., 0, ..., ETA + 1, ETA}` and:

 ```plaintext
 - If v < 0, Pr[v] = Pr[-v]
 - If v >= 0, Pr[v] = BINOMIAL_COEFFICIENT(2 * ETA; ETA - v) / 2 ^ (2 * ETA)
 ```

 The values `v < 0` are mapped to the appropriate `KyberFieldElement`.

 The expected value is:

 ```plaintext
 E[X] = (-ETA)Pr[-ETA] + (-(ETA - 1))Pr[-(ETA - 1)] + ... + (ETA - 1)Pr[ETA - 1]
 + (ETA)Pr[ETA] = 0 since Pr[-v] = Pr[v] when v < 0.
 ```

 And the variance is:

 ```plaintext
 Var(X) = E[(X - E[X])^2]
        = E[X^2]
        = sum_(v=-ETA to ETA)v^2 * (BINOMIAL_COEFFICIENT(2 * ETA; ETA - v) /
 2^(2 * ETA)) = ETA / 2
 ```

 This function implements <strong>Algorithm 7</strong> of the NIST FIPS 203
 standard, which is reproduced below:

 ```plaintext
 Input: byte array B ∈ 𝔹^{64η}.
 Output: array f ∈ ℤ₂₅₆.

 b ← BytesToBits(B)
 for (i ← 0; i < 256; i++)
     x ← ∑(j=0 to η - 1) b[2iη + j]
     y ← ∑(j=0 to η - 1) b[2iη + η + j]
     f[i] ← x−y mod q
 end for
 return f
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of
libcrux_ml_kem.sampling.sample_from_binomial_distribution_2 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
sample_from_binomial_distribution_2_ea(Eurydice_borrow_slice_u8 randomness) {
  Eurydice_arr_04 sampled_i16s = {.data = {0U}};
  for (size_t i0 = (size_t)0U; i0 < randomness.meta / (size_t)4U; i0++) {
    size_t chunk_number = i0;
    Eurydice_borrow_slice_u8 byte_chunk = Eurydice_slice_subslice_shared_c8(
        randomness, (KRML_CLITERAL(core_ops_range_Range_87){
                        .start = chunk_number * (size_t)4U,
                        .end = chunk_number * (size_t)4U + (size_t)4U}));
    uint32_t random_bits_as_u32 =
        (((uint32_t)byte_chunk.ptr[0U] | (uint32_t)byte_chunk.ptr[1U] << 8U) |
         (uint32_t)byte_chunk.ptr[2U] << 16U) |
        (uint32_t)byte_chunk.ptr[3U] << 24U;
    uint32_t even_bits = random_bits_as_u32 & 1431655765U;
    uint32_t odd_bits = random_bits_as_u32 >> 1U & 1431655765U;
    uint32_t coin_toss_outcomes = even_bits + odd_bits;
    for (uint32_t i = 0U; i < 32U / 4U; i++) {
      uint32_t outcome_set = i;
      uint32_t outcome_set0 = outcome_set * 4U;
      int16_t outcome_1 =
          (int16_t)(coin_toss_outcomes >> (uint32_t)outcome_set0 & 3U);
      int16_t outcome_2 =
          (int16_t)(coin_toss_outcomes >> (uint32_t)(outcome_set0 + 2U) & 3U);
      size_t offset = (size_t)(outcome_set0 >> 2U);
      sampled_i16s.data[(size_t)8U * chunk_number + offset] =
          outcome_1 - outcome_2;
    }
  }
  return from_i16_array_d6_ea(Eurydice_array_to_slice_shared_99(&sampled_i16s));
}

/**
A monomorphic instance of
libcrux_ml_kem.sampling.sample_from_binomial_distribution with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- ETA= 2
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
sample_from_binomial_distribution_66(Eurydice_borrow_slice_u8 randomness) {
  return sample_from_binomial_distribution_2_ea(randomness);
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_at_layer_7
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_at_layer_7_ea(Eurydice_arr_9e *re) {
  size_t step = VECTORS_IN_RING_ELEMENT / (size_t)2U;
  for (size_t i = (size_t)0U; i < step; i++) {
    size_t j = i;
    Eurydice_arr_d6 t = libcrux_ml_kem_vector_portable_multiply_by_constant_b8(
        re->data[j + step], -1600);
    re->data[j + step] = libcrux_ml_kem_vector_portable_sub_b8(re->data[j], &t);
    Eurydice_arr_d6 uu____1 =
        libcrux_ml_kem_vector_portable_add_b8(re->data[j], &t);
    re->data[j] = uu____1;
  }
}

typedef struct libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2_s {
  Eurydice_arr_d6 fst;
  Eurydice_arr_d6 snd;
} libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2;

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_layer_int_vec_step
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE
    libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2
    ntt_layer_int_vec_step_ea(Eurydice_arr_d6 a, Eurydice_arr_d6 b,
                              int16_t zeta_r) {
  Eurydice_arr_d6 t =
      libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(b,
                                                                        zeta_r);
  b = libcrux_ml_kem_vector_portable_sub_b8(a, &t);
  a = libcrux_ml_kem_vector_portable_add_b8(a, &t);
  return (KRML_CLITERAL(
      libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2){.fst = a,
                                                                    .snd = b});
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_at_layer_4_plus
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_at_layer_4_plus_ea(size_t *zeta_i,
                                                   Eurydice_arr_9e *re,
                                                   size_t layer) {
  size_t step = (size_t)1U << (uint32_t)layer;
  for (size_t i0 = (size_t)0U; i0 < (size_t)128U >> (uint32_t)layer; i0++) {
    size_t round = i0;
    zeta_i[0U]++;
    size_t offset = round * step * (size_t)2U;
    size_t offset_vec = offset / (size_t)16U;
    size_t step_vec = step / (size_t)16U;
    for (size_t i = offset_vec; i < offset_vec + step_vec; i++) {
      size_t j = i;
      libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2 uu____0 =
          ntt_layer_int_vec_step_ea(re->data[j], re->data[j + step_vec],
                                    zeta(zeta_i[0U]));
      Eurydice_arr_d6 x = uu____0.fst;
      Eurydice_arr_d6 y = uu____0.snd;
      re->data[j] = x;
      re->data[j + step_vec] = y;
    }
  }
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_at_layer_3
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_at_layer_3_ea(size_t *zeta_i,
                                              Eurydice_arr_9e *re) {
  KRML_MAYBE_FOR16(i, (size_t)0U, (size_t)16U, (size_t)1U, size_t round = i;
                   zeta_i[0U]++;
                   Eurydice_arr_d6 uu____0 =
                       libcrux_ml_kem_vector_portable_ntt_layer_3_step_b8(
                           re->data[round], zeta(zeta_i[0U]));
                   re->data[round] = uu____0;);
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_at_layer_2
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_at_layer_2_ea(size_t *zeta_i,
                                              Eurydice_arr_9e *re) {
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t round = i; zeta_i[0U]++;
      re->data[round] = libcrux_ml_kem_vector_portable_ntt_layer_2_step_b8(
          re->data[round], zeta(zeta_i[0U]), zeta(zeta_i[0U] + (size_t)1U));
      zeta_i[0U]++;);
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_at_layer_1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_at_layer_1_ea(size_t *zeta_i,
                                              Eurydice_arr_9e *re) {
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t round = i; zeta_i[0U]++;
      re->data[round] = libcrux_ml_kem_vector_portable_ntt_layer_1_step_b8(
          re->data[round], zeta(zeta_i[0U]), zeta(zeta_i[0U] + (size_t)1U),
          zeta(zeta_i[0U] + (size_t)2U), zeta(zeta_i[0U] + (size_t)3U));
      zeta_i[0U] += (size_t)3U;);
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.poly_barrett_reduce
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void poly_barrett_reduce_ea(Eurydice_arr_9e *myself) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 uu____0 =
        libcrux_ml_kem_vector_portable_barrett_reduce_b8(myself->data[i0]);
    myself->data[i0] = uu____0;
  }
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.poly_barrett_reduce_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void poly_barrett_reduce_d6_ea(Eurydice_arr_9e *self) {
  poly_barrett_reduce_ea(self);
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_binomially_sampled_ring_element
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_binomially_sampled_ring_element_ea(
    Eurydice_arr_9e *re) {
  ntt_at_layer_7_ea(re);
  size_t zeta_i = (size_t)1U;
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)6U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)5U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)4U);
  ntt_at_layer_3_ea(&zeta_i, re);
  ntt_at_layer_2_ea(&zeta_i, re);
  ntt_at_layer_1_ea(&zeta_i, re);
  poly_barrett_reduce_d6_ea(re);
}

/**
 Sample a vector of ring elements from a centered binomial distribution and
 convert them into their NTT representations.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.sample_vector_cbd_then_ntt
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- ETA= 2
- ETA_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE uint8_t sample_vector_cbd_then_ntt_bf0(
    Eurydice_arr_d20 *re_as_ntt, const Eurydice_arr_fa *prf_input,
    uint8_t domain_separator) {
  Eurydice_arr_890 prf_inputs;
  Eurydice_arr_fa repeat_expression[4U];
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U,
      repeat_expression[i] = core_array__core__clone__Clone_for__T__N___clone(
          (size_t)33U, prf_input, uint8_t, Eurydice_arr_fa););
  memcpy(prf_inputs.data, repeat_expression,
         (size_t)4U * sizeof(Eurydice_arr_fa));
  domain_separator =
      libcrux_ml_kem_utils_prf_input_inc_23(&prf_inputs, domain_separator);
  Eurydice_arr_3b prf_outputs = PRFxN_4a_f5(&prf_inputs);
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e uu____0 = sample_from_binomial_distribution_66(
          Eurydice_array_to_slice_shared_78(&prf_outputs.data[i0]));
      re_as_ntt->data[i0] = uu____0;
      ntt_binomially_sampled_ring_element_ea(&re_as_ntt->data[i0]););
  return domain_separator;
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@3]> for
libcrux_ml_kem::ind_cpa::generate_keypair_unpacked::closure<Vector, Hasher,
Scheme, K, ETA1, ETA1_RANDOMNESS_SIZE>[TraitClause@0, TraitClause@1,
TraitClause@2, TraitClause@3, TraitClause@4, TraitClause@5]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cpa.generate_keypair_unpacked.call_mut_73 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_73_390(void **_) { return ZERO_d6_ea(); }

/**
 Given two `KyberPolynomialRingElement`s in their NTT representations,
 compute their product. Given two polynomials in the NTT domain `f^` and `ĵ`,
 the `iᵗʰ` coefficient of the product `k̂` is determined by the calculation:

 ```plaintext
 ĥ[2·i] + ĥ[2·i + 1]X = (f^[2·i] + f^[2·i + 1]X)·(ĝ[2·i] + ĝ[2·i + 1]X) mod (X²
 - ζ^(2·BitRev₇(i) + 1))
 ```

 This function almost implements <strong>Algorithm 10</strong> of the
 NIST FIPS 203 standard, which is reproduced below:

 ```plaintext
 Input: Two arrays fˆ ∈ ℤ₂₅₆ and ĝ ∈ ℤ₂₅₆.
 Output: An array ĥ ∈ ℤq.

 for(i ← 0; i < 128; i++)
     (ĥ[2i], ĥ[2i+1]) ← BaseCaseMultiply(fˆ[2i], fˆ[2i+1], ĝ[2i], ĝ[2i+1],
 ζ^(2·BitRev₇(i) + 1)) end for return ĥ
 ```
 We say "almost" because the coefficients of the ring element output by
 this function are in the Montgomery domain.

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.ntt_multiply
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
ntt_multiply_ea(const Eurydice_arr_9e *myself, const Eurydice_arr_9e *rhs) {
  Eurydice_arr_9e out = ZERO_ea();
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 uu____0 = libcrux_ml_kem_vector_portable_ntt_multiply_b8(
        &myself->data[i0], &rhs->data[i0], zeta((size_t)64U + (size_t)4U * i0),
        zeta((size_t)64U + (size_t)4U * i0 + (size_t)1U),
        zeta((size_t)64U + (size_t)4U * i0 + (size_t)2U),
        zeta((size_t)64U + (size_t)4U * i0 + (size_t)3U));
    out.data[i0] = uu____0;
  }
  return out;
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.ntt_multiply_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
ntt_multiply_d6_ea(const Eurydice_arr_9e *self, const Eurydice_arr_9e *rhs) {
  return ntt_multiply_ea(self, rhs);
}

/**
 Given two polynomial ring elements `lhs` and `rhs`, compute the pointwise
 sum of their constituent coefficients.
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_to_ring_element
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void add_to_ring_element_ee(Eurydice_arr_9e *myself,
                                                   const Eurydice_arr_9e *rhs) {
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_d6 uu____0 = libcrux_ml_kem_vector_portable_add_b8(
          myself->data[i0], &rhs->data[i0]);
      myself->data[i0] = uu____0;);
}

/**
 Given two polynomial ring elements `lhs` and `rhs`, compute the pointwise
 sum of their constituent coefficients.
*/
/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_to_ring_element_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void add_to_ring_element_d6_ee(
    Eurydice_arr_9e *self, const Eurydice_arr_9e *rhs) {
  add_to_ring_element_ee(self, rhs);
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.to_standard_domain
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
to_standard_domain_ea(Eurydice_arr_d6 vector) {
  return libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
      vector,
      LIBCRUX_ML_KEM_VECTOR_TRAITS_MONTGOMERY_R_SQUARED_MOD_FIELD_MODULUS);
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_standard_error_reduce
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void add_standard_error_reduce_ea(
    Eurydice_arr_9e *myself, const Eurydice_arr_9e *error) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t j = i;
    Eurydice_arr_d6 coefficient_normal_form =
        to_standard_domain_ea(myself->data[j]);
    Eurydice_arr_d6 sum = libcrux_ml_kem_vector_portable_add_b8(
        coefficient_normal_form, &error->data[j]);
    Eurydice_arr_d6 red = libcrux_ml_kem_vector_portable_barrett_reduce_b8(sum);
    myself->data[j] = red;
  }
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_standard_error_reduce_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void add_standard_error_reduce_d6_ea(
    Eurydice_arr_9e *self, const Eurydice_arr_9e *error) {
  add_standard_error_reduce_ea(self, error);
}

/**
 Compute Â ◦ ŝ + ê
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_As_plus_e
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void compute_As_plus_e_ee(
    Eurydice_arr_d20 *t_as_ntt, const Eurydice_arr_60 *matrix_A,
    const Eurydice_arr_d20 *s_as_ntt, const Eurydice_arr_d20 *error_as_ntt) {
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      const Eurydice_arr_d20 *row = &matrix_A->data[i0];
      Eurydice_arr_9e uu____0 = ZERO_d6_ea(); t_as_ntt->data[i0] = uu____0;
      KRML_MAYBE_FOR4(
          i1, (size_t)0U, (size_t)4U, (size_t)1U, size_t j = i1;
          const Eurydice_arr_9e *matrix_element = &row->data[j];
          Eurydice_arr_9e product =
              ntt_multiply_d6_ea(matrix_element, &s_as_ntt->data[j]);
          add_to_ring_element_d6_ee(&t_as_ntt->data[i0], &product););
      add_standard_error_reduce_d6_ea(&t_as_ntt->data[i0],
                                      &error_as_ntt->data[i0]););
}

/**
 This function implements most of <strong>Algorithm 12</strong> of the
 NIST FIPS 203 specification; this is the Kyber CPA-PKE key generation
 algorithm.

 We say "most of" since Algorithm 12 samples the required randomness within
 the function itself, whereas this implementation expects it to be provided
 through the `key_generation_seed` parameter.

 Algorithm 12 is reproduced below:

 ```plaintext
 Output: encryption key ekₚₖₑ ∈ 𝔹^{384k+32}.
 Output: decryption key dkₚₖₑ ∈ 𝔹^{384k}.

 d ←$ B
 (ρ,σ) ← G(d)
 N ← 0
 for (i ← 0; i < k; i++)
     for(j ← 0; j < k; j++)
         Â[i,j] ← SampleNTT(XOF(ρ, i, j))
     end for
 end for
 for(i ← 0; i < k; i++)
     s[i] ← SamplePolyCBD_{η₁}(PRF_{η₁}(σ,N))
     N ← N + 1
 end for
 for(i ← 0; i < k; i++)
     e[i] ← SamplePolyCBD_{η₂}(PRF_{η₂}(σ,N))
     N ← N + 1
 end for
 ŝ ← NTT(s)
 ê ← NTT(e)
 t̂ ← Â◦ŝ + ê
 ekₚₖₑ ← ByteEncode₁₂(t̂) ‖ ρ
 dkₚₖₑ ← ByteEncode₁₂(ŝ)
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.generate_keypair_unpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE void generate_keypair_unpacked_390(
    Eurydice_borrow_slice_u8 key_generation_seed, Eurydice_arr_d20 *private_key,
    libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94 *public_key) {
  Eurydice_arr_c7 hashed = cpa_keygen_seed_39_fe(key_generation_seed);
  Eurydice_borrow_slice_u8_x2 uu____0 = Eurydice_slice_split_at(
      Eurydice_array_to_slice_shared_17(&hashed), (size_t)32U, uint8_t,
      Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 seed_for_A = uu____0.fst;
  Eurydice_borrow_slice_u8 seed_for_secret_and_error = uu____0.snd;
  Eurydice_arr_60 *uu____1 = &public_key->A;
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_31 lvalue0 =
      libcrux_ml_kem_utils_into_padded_array_de(seed_for_A);
  sample_matrix_A_910(uu____1, &lvalue0, true);
  Eurydice_arr_fa prf_input =
      libcrux_ml_kem_utils_into_padded_array_29(seed_for_secret_and_error);
  uint8_t domain_separator =
      sample_vector_cbd_then_ntt_bf0(private_key, &prf_input, 0U);
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_73_390(&lvalue););
  Eurydice_arr_d20 error_as_ntt = arr_struct;
  sample_vector_cbd_then_ntt_bf0(&error_as_ntt, &prf_input, domain_separator);
  compute_As_plus_e_ee(&public_key->t_as_ntt, &public_key->A, &private_key[0U],
                       &error_as_ntt);
  Eurydice_arr_ec arr;
  memcpy(arr.data, seed_for_A.ptr, (size_t)32U * sizeof(uint8_t));
  Eurydice_arr_ec uu____2 = core_result_unwrap_26_39((KRML_CLITERAL(
      core_result_Result_07){.tag = core_result_Ok, .val = {.case_Ok = arr}}));
  public_key->seed_for_A = uu____2;
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for
libcrux_ml_kem::ind_cca::unpacked::transpose_a::closure::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.transpose_a.closure.call_mut_b4 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_b4_ee(void **_) { return ZERO_d6_ea(); }

/**
This function found in impl {core::ops::function::FnMut<(usize),
[libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]; K]> for
libcrux_ml_kem::ind_cca::unpacked::transpose_a::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.transpose_a.call_mut_22 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_d20 call_mut_22_ee(void **_) {
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_b4_ee(&lvalue););
  return arr_struct;
}

/**
This function found in impl {core::clone::Clone for
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.clone_c1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) inline Eurydice_arr_9e clone_c1_ea(const Eurydice_arr_9e *self) {
  return core_array__core__clone__Clone_for__T__N___clone(
      (size_t)16U, self, Eurydice_arr_d6, Eurydice_arr_9e);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.transpose_a
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_60 transpose_a_ee(Eurydice_arr_60 ind_cpa_a) {
  Eurydice_arr_60 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_22_ee(&lvalue););
  Eurydice_arr_60 A = arr_struct;
  KRML_MAYBE_FOR4(
      i0, (size_t)0U, (size_t)4U, (size_t)1U, size_t i1 = i0; KRML_MAYBE_FOR4(
          i, (size_t)0U, (size_t)4U, (size_t)1U, size_t j = i;
          Eurydice_arr_9e uu____0 = clone_c1_ea(&ind_cpa_a.data[j].data[i1]);
          A.data[i1].data[j] = uu____0;););
  return A;
}

/**
 Generate Unpacked Keys
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_generate_keypair_b80(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked *out) {
  Eurydice_borrow_slice_u8 ind_cpa_keypair_randomness =
      Eurydice_array_to_subslice_shared_d43(
          &randomness,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end =
                  LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE}));
  Eurydice_borrow_slice_u8 implicit_rejection_value =
      Eurydice_array_to_subslice_from_shared_5f0(
          &randomness,
          LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE);
  generate_keypair_unpacked_390(ind_cpa_keypair_randomness,
                                &out->private_key.ind_cpa_private_key,
                                &out->public_key.ind_cpa_public_key);
  Eurydice_arr_60 A = transpose_a_ee(out->public_key.ind_cpa_public_key.A);
  out->public_key.ind_cpa_public_key.A = A;
  Eurydice_arr_d1 pk_serialized = serialize_public_key_1c(
      &out->public_key.ind_cpa_public_key.t_as_ntt,
      Eurydice_array_to_slice_shared_01(
          &out->public_key.ind_cpa_public_key.seed_for_A));
  Eurydice_arr_ec uu____0 =
      H_4a_23(Eurydice_array_to_slice_shared_b50(&pk_serialized));
  out->public_key.public_key_hash = uu____0;
  Eurydice_arr_ec arr;
  memcpy(arr.data, implicit_rejection_value.ptr, (size_t)32U * sizeof(uint8_t));
  Eurydice_arr_ec uu____1 = core_result_unwrap_26_39((KRML_CLITERAL(
      core_result_Result_07){.tag = core_result_Ok, .val = {.case_Ok = arr}}));
  out->private_key.implicit_rejection_value = uu____1;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.encaps_prepare
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_c7 encaps_prepare_fe(Eurydice_borrow_slice_u8 randomness,
                                         Eurydice_borrow_slice_u8 pk_hash) {
  Eurydice_arr_c7 to_hash =
      libcrux_ml_kem_utils_into_padded_array_c9(randomness);
  Eurydice_slice_copy(Eurydice_array_to_subslice_from_mut_5f(
                          &to_hash, LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE),
                      pk_hash, uint8_t);
  return G_4a_23(Eurydice_array_to_slice_shared_17(&to_hash));
}

/**
A monomorphic instance of n-tuple
with types Eurydice_arr_d20, libcrux_ml_kem_polynomial_PolynomialRingElement_1d

*/
typedef struct tuple_12_s {
  Eurydice_arr_d20 fst;
  Eurydice_arr_9e snd;
} tuple_12;

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]> for libcrux_ml_kem::ind_cpa::encrypt_c1::closure<Vector, Hasher,
K, C1_LEN, U_COMPRESSION_FACTOR, BLOCK_LEN, ETA1, ETA1_RANDOMNESS_SIZE, ETA2,
ETA2_RANDOMNESS_SIZE>[TraitClause@0, TraitClause@1, TraitClause@2,
TraitClause@3]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c1.call_mut_f1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- C1_LEN= 1408
- U_COMPRESSION_FACTOR= 11
- BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_f1_870(void **_) { return ZERO_d6_ea(); }

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]> for libcrux_ml_kem::ind_cpa::encrypt_c1::closure#1<Vector,
Hasher, K, C1_LEN, U_COMPRESSION_FACTOR, BLOCK_LEN, ETA1, ETA1_RANDOMNESS_SIZE,
ETA2, ETA2_RANDOMNESS_SIZE>[TraitClause@0, TraitClause@1, TraitClause@2,
TraitClause@3]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c1.call_mut_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- C1_LEN= 1408
- U_COMPRESSION_FACTOR= 11
- BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_dd_870(void **_) { return ZERO_d6_ea(); }

/**
 Sample a vector of ring elements from a centered binomial distribution.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.sample_ring_element_cbd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- ETA2_RANDOMNESS_SIZE= 128
- ETA2= 2
*/
static __attribute__((unused)) KRML_MUSTINLINE uint8_t sample_ring_element_cbd_bf0(
    const Eurydice_arr_fa *prf_input, uint8_t domain_separator,
    Eurydice_arr_d20 *error_1) {
  Eurydice_arr_890 prf_inputs;
  Eurydice_arr_fa repeat_expression[4U];
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U,
      repeat_expression[i] = core_array__core__clone__Clone_for__T__N___clone(
          (size_t)33U, prf_input, uint8_t, Eurydice_arr_fa););
  memcpy(prf_inputs.data, repeat_expression,
         (size_t)4U * sizeof(Eurydice_arr_fa));
  domain_separator =
      libcrux_ml_kem_utils_prf_input_inc_23(&prf_inputs, domain_separator);
  Eurydice_arr_3b prf_outputs = PRFxN_4a_f5(&prf_inputs);
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e uu____0 = sample_from_binomial_distribution_66(
          Eurydice_array_to_slice_shared_78(&prf_outputs.data[i0]));
      error_1->data[i0] = uu____0;);
  return domain_separator;
}

/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRF
with const generics
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_89 PRF_ec(Eurydice_borrow_slice_u8 input) {
  Eurydice_arr_89 digest = {.data = {0U}};
  libcrux_sha3_portable_shake256(Eurydice_array_to_slice_mut_78(&digest),
                                 input);
  return digest;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRF_4a
with const generics
- K= 4
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_89 PRF_4a_f50(Eurydice_borrow_slice_u8 input) {
  return PRF_ec(input);
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for libcrux_ml_kem::matrix::compute_vector_u::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_vector_u.call_mut_a8
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_a8_ee(void **_) { return ZERO_d6_ea(); }

/**
A monomorphic instance of libcrux_ml_kem.invert_ntt.invert_ntt_at_layer_1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void invert_ntt_at_layer_1_ea(size_t *zeta_i,
                                                     Eurydice_arr_9e *re) {
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t round = i; zeta_i[0U]--;
      re->data[round] = libcrux_ml_kem_vector_portable_inv_ntt_layer_1_step_b8(
          re->data[round], zeta(zeta_i[0U]), zeta(zeta_i[0U] - (size_t)1U),
          zeta(zeta_i[0U] - (size_t)2U), zeta(zeta_i[0U] - (size_t)3U));
      zeta_i[0U] -= (size_t)3U;);
}

/**
A monomorphic instance of libcrux_ml_kem.invert_ntt.invert_ntt_at_layer_2
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void invert_ntt_at_layer_2_ea(size_t *zeta_i,
                                                     Eurydice_arr_9e *re) {
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t round = i; zeta_i[0U]--;
      re->data[round] = libcrux_ml_kem_vector_portable_inv_ntt_layer_2_step_b8(
          re->data[round], zeta(zeta_i[0U]), zeta(zeta_i[0U] - (size_t)1U));
      zeta_i[0U]--;);
}

/**
A monomorphic instance of libcrux_ml_kem.invert_ntt.invert_ntt_at_layer_3
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void invert_ntt_at_layer_3_ea(size_t *zeta_i,
                                                     Eurydice_arr_9e *re) {
  KRML_MAYBE_FOR16(i, (size_t)0U, (size_t)16U, (size_t)1U, size_t round = i;
                   zeta_i[0U]--;
                   Eurydice_arr_d6 uu____0 =
                       libcrux_ml_kem_vector_portable_inv_ntt_layer_3_step_b8(
                           re->data[round], zeta(zeta_i[0U]));
                   re->data[round] = uu____0;);
}

/**
A monomorphic instance of
libcrux_ml_kem.invert_ntt.inv_ntt_layer_int_vec_step_reduce with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE
    libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2
    inv_ntt_layer_int_vec_step_reduce_ea(Eurydice_arr_d6 a, Eurydice_arr_d6 b,
                                         int16_t zeta_r) {
  Eurydice_arr_d6 a_minus_b = libcrux_ml_kem_vector_portable_sub_b8(b, &a);
  a = libcrux_ml_kem_vector_portable_barrett_reduce_b8(
      libcrux_ml_kem_vector_portable_add_b8(a, &b));
  b = libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
      a_minus_b, zeta_r);
  return (KRML_CLITERAL(
      libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2){.fst = a,
                                                                    .snd = b});
}

/**
A monomorphic instance of libcrux_ml_kem.invert_ntt.invert_ntt_at_layer_4_plus
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void invert_ntt_at_layer_4_plus_ea(size_t *zeta_i,
                                                          Eurydice_arr_9e *re,
                                                          size_t layer) {
  size_t step = (size_t)1U << (uint32_t)layer;
  for (size_t i0 = (size_t)0U; i0 < (size_t)128U >> (uint32_t)layer; i0++) {
    size_t round = i0;
    zeta_i[0U]--;
    size_t offset = round * step * (size_t)2U;
    size_t offset_vec =
        offset / LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR;
    size_t step_vec =
        step / LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR;
    for (size_t i = offset_vec; i < offset_vec + step_vec; i++) {
      size_t j = i;
      libcrux_ml_kem_vector_portable_vector_type_PortableVector_x2 uu____0 =
          inv_ntt_layer_int_vec_step_reduce_ea(
              re->data[j], re->data[j + step_vec], zeta(zeta_i[0U]));
      Eurydice_arr_d6 x = uu____0.fst;
      Eurydice_arr_d6 y = uu____0.snd;
      re->data[j] = x;
      re->data[j + step_vec] = y;
    }
  }
}

/**
A monomorphic instance of libcrux_ml_kem.invert_ntt.invert_ntt_montgomery
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE void invert_ntt_montgomery_ee(Eurydice_arr_9e *re) {
  size_t zeta_i =
      LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT / (size_t)2U;
  invert_ntt_at_layer_1_ea(&zeta_i, re);
  invert_ntt_at_layer_2_ea(&zeta_i, re);
  invert_ntt_at_layer_3_ea(&zeta_i, re);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)4U);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)5U);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)6U);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)7U);
  poly_barrett_reduce_d6_ea(re);
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_error_reduce
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void add_error_reduce_ea(Eurydice_arr_9e *myself,
                                                const Eurydice_arr_9e *error) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t j = i;
    Eurydice_arr_d6 coefficient_normal_form =
        libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
            myself->data[j], 1441);
    Eurydice_arr_d6 sum = libcrux_ml_kem_vector_portable_add_b8(
        coefficient_normal_form, &error->data[j]);
    Eurydice_arr_d6 red = libcrux_ml_kem_vector_portable_barrett_reduce_b8(sum);
    myself->data[j] = red;
  }
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_error_reduce_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void add_error_reduce_d6_ea(
    Eurydice_arr_9e *self, const Eurydice_arr_9e *error) {
  add_error_reduce_ea(self, error);
}

/**
 Compute u := InvertNTT(Aᵀ ◦ r̂) + e₁
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_vector_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d20 compute_vector_u_ee(
    const Eurydice_arr_60 *a_as_ntt, const Eurydice_arr_d20 *r_as_ntt,
    const Eurydice_arr_d20 *error_1) {
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_a8_ee(&lvalue););
  Eurydice_arr_d20 result = arr_struct;
  KRML_MAYBE_FOR4(
      i0, (size_t)0U, (size_t)4U, (size_t)1U, size_t i1 = i0;
      const Eurydice_arr_d20 *row = &a_as_ntt->data[i1];
      KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t j = i;
                      const Eurydice_arr_9e *a_element = &row->data[j];
                      Eurydice_arr_9e product =
                          ntt_multiply_d6_ea(a_element, &r_as_ntt->data[j]);
                      add_to_ring_element_d6_ee(&result.data[i1], &product););
      invert_ntt_montgomery_ee(&result.data[i1]);
      add_error_reduce_d6_ea(&result.data[i1], &error_1->data[i1]););
  return result;
}

/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress.compress
with const generics
- COEFFICIENT_BITS= 10
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 compress_ef(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int16_t uu____0 = libcrux_secrets_int_as_i16_f5(
        libcrux_ml_kem_vector_portable_compress_compress_ciphertext_coefficient(
            (uint8_t)10, libcrux_secrets_int_as_u16_f5(a.data[i0])));
    a.data[i0] = uu____0;
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress_b8
with const generics
- COEFFICIENT_BITS= 10
*/
static __attribute__((unused)) Eurydice_arr_d6 compress_b8_ef(Eurydice_arr_d6 a) {
  return compress_ef(a);
}

/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress.compress
with const generics
- COEFFICIENT_BITS= 11
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 compress_c4(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int16_t uu____0 = libcrux_secrets_int_as_i16_f5(
        libcrux_ml_kem_vector_portable_compress_compress_ciphertext_coefficient(
            (uint8_t)11, libcrux_secrets_int_as_u16_f5(a.data[i0])));
    a.data[i0] = uu____0;
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress_b8
with const generics
- COEFFICIENT_BITS= 11
*/
static __attribute__((unused)) Eurydice_arr_d6 compress_b8_c4(Eurydice_arr_d6 a) {
  return compress_c4(a);
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.compress_then_serialize_11
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- OUT_LEN= 352
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_e7
compress_then_serialize_11_bd(const Eurydice_arr_9e *re) {
  Eurydice_arr_e7 serialized = {.data = {0U}};
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficient = compress_b8_c4(
        libcrux_ml_kem_vector_portable_to_unsigned_representative_b8(
            re->data[i0]));
    Eurydice_arr_80 bytes =
        libcrux_ml_kem_vector_portable_serialize_11_b8(coefficient);
    Eurydice_slice_copy(
        Eurydice_array_to_subslice_mut_d415(
            &serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                             .start = (size_t)22U * i0,
                             .end = (size_t)22U * i0 + (size_t)22U})),
        Eurydice_array_to_slice_shared_98(&bytes), uint8_t);
  }
  return serialized;
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.compress_then_serialize_ring_element_u with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- COMPRESSION_FACTOR= 11
- OUT_LEN= 352
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_e7
compress_then_serialize_ring_element_u_86(const Eurydice_arr_9e *re) {
  return compress_then_serialize_11_bd(re);
}

/**
 Call [`compress_then_serialize_ring_element_u`] on each ring element.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.compress_then_serialize_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- OUT_LEN= 1408
- COMPRESSION_FACTOR= 11
- BLOCK_LEN= 352
*/
static __attribute__((unused)) KRML_MUSTINLINE void compress_then_serialize_u_2e(
    Eurydice_arr_d20 input, Eurydice_mut_borrow_slice_u8 out) {
  KRML_MAYBE_FOR4(
      i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e re = input.data[i0];
      Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
          out, (KRML_CLITERAL(core_ops_range_Range_87){
                   .start = i0 * ((size_t)1408U / (size_t)4U),
                   .end = (i0 + (size_t)1U) * ((size_t)1408U / (size_t)4U)}));
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_e7 lvalue = compress_then_serialize_ring_element_u_86(&re);
      Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_25(&lvalue),
                          uint8_t););
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- C1_LEN= 1408
- U_COMPRESSION_FACTOR= 11
- BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE tuple_12 encrypt_c1_870(
    Eurydice_borrow_slice_u8 randomness, const Eurydice_arr_60 *matrix,
    Eurydice_mut_borrow_slice_u8 ciphertext) {
  Eurydice_arr_fa prf_input =
      libcrux_ml_kem_utils_into_padded_array_29(randomness);
  Eurydice_arr_d20 arr_struct0;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct0.data[i] = call_mut_f1_870(&lvalue););
  Eurydice_arr_d20 r_as_ntt = arr_struct0;
  uint8_t domain_separator0 =
      sample_vector_cbd_then_ntt_bf0(&r_as_ntt, &prf_input, 0U);
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_dd_870(&lvalue););
  Eurydice_arr_d20 error_1 = arr_struct;
  uint8_t domain_separator =
      sample_ring_element_cbd_bf0(&prf_input, domain_separator0, &error_1);
  prf_input.data[32U] = domain_separator;
  Eurydice_arr_89 prf_output =
      PRF_4a_f50(Eurydice_array_to_slice_shared_b5(&prf_input));
  Eurydice_arr_9e error_2 = sample_from_binomial_distribution_66(
      Eurydice_array_to_slice_shared_78(&prf_output));
  Eurydice_arr_d20 u = compute_vector_u_ee(matrix, &r_as_ntt, &error_1);
  compress_then_serialize_u_2e(u, ciphertext);
  return (KRML_CLITERAL(tuple_12){.fst = r_as_ntt, .snd = error_2});
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_message with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_message_ea(const Eurydice_arr_ec *serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_d6 coefficient_compressed =
          libcrux_ml_kem_vector_portable_deserialize_1_b8(
              Eurydice_array_to_subslice_shared_d42(
                  serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                                  .start = (size_t)2U * i0,
                                  .end = (size_t)2U * i0 + (size_t)2U})));
      Eurydice_arr_d6 uu____0 = libcrux_ml_kem_vector_portable_decompress_1_b8(
          coefficient_compressed);
      re.data[i0] = uu____0;);
  return re;
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_message_error_reduce
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e add_message_error_reduce_ea(
    const Eurydice_arr_9e *myself, const Eurydice_arr_9e *message,
    Eurydice_arr_9e result) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficient_normal_form =
        libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
            result.data[i0], 1441);
    Eurydice_arr_d6 sum1 = libcrux_ml_kem_vector_portable_add_b8(
        myself->data[i0], &message->data[i0]);
    Eurydice_arr_d6 sum2 =
        libcrux_ml_kem_vector_portable_add_b8(coefficient_normal_form, &sum1);
    Eurydice_arr_d6 red =
        libcrux_ml_kem_vector_portable_barrett_reduce_b8(sum2);
    result.data[i0] = red;
  }
  return result;
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_message_error_reduce_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e add_message_error_reduce_d6_ea(
    const Eurydice_arr_9e *self, const Eurydice_arr_9e *message,
    Eurydice_arr_9e result) {
  return add_message_error_reduce_ea(self, message, result);
}

/**
 Compute InverseNTT(tᵀ ◦ r̂) + e₂ + message
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_ring_element_v
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e compute_ring_element_v_ee(
    const Eurydice_arr_d20 *t_as_ntt, const Eurydice_arr_d20 *r_as_ntt,
    const Eurydice_arr_9e *error_2, const Eurydice_arr_9e *message) {
  Eurydice_arr_9e result = ZERO_d6_ea();
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
                  Eurydice_arr_9e product = ntt_multiply_d6_ea(
                      &t_as_ntt->data[i0], &r_as_ntt->data[i0]);
                  add_to_ring_element_d6_ee(&result, &product););
  invert_ntt_montgomery_ee(&result);
  return add_message_error_reduce_d6_ea(error_2, message, result);
}

/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress.compress
with const generics
- COEFFICIENT_BITS= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 compress_d1(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int16_t uu____0 = libcrux_secrets_int_as_i16_f5(
        libcrux_ml_kem_vector_portable_compress_compress_ciphertext_coefficient(
            (uint8_t)4, libcrux_secrets_int_as_u16_f5(a.data[i0])));
    a.data[i0] = uu____0;
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress_b8
with const generics
- COEFFICIENT_BITS= 4
*/
static __attribute__((unused)) Eurydice_arr_d6 compress_b8_d1(Eurydice_arr_d6 a) {
  return compress_d1(a);
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.compress_then_serialize_4
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void compress_then_serialize_4_ea(
    Eurydice_arr_9e re, Eurydice_mut_borrow_slice_u8 serialized) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficient =
        compress_b8_d1(to_unsigned_field_modulus_ea(re.data[i0]));
    Eurydice_array_u8x8 bytes =
        libcrux_ml_kem_vector_portable_serialize_4_b8(coefficient);
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                            .start = (size_t)8U * i0,
                            .end = (size_t)8U * i0 + (size_t)8U})),
        Eurydice_array_to_slice_shared_6e(&bytes), uint8_t);
  }
}

/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress.compress
with const generics
- COEFFICIENT_BITS= 5
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6 compress_f4(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int16_t uu____0 = libcrux_secrets_int_as_i16_f5(
        libcrux_ml_kem_vector_portable_compress_compress_ciphertext_coefficient(
            (uint8_t)5, libcrux_secrets_int_as_u16_f5(a.data[i0])));
    a.data[i0] = uu____0;
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of libcrux_ml_kem.vector.portable.compress_b8
with const generics
- COEFFICIENT_BITS= 5
*/
static __attribute__((unused)) Eurydice_arr_d6 compress_b8_f4(Eurydice_arr_d6 a) {
  return compress_f4(a);
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.compress_then_serialize_5
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE void compress_then_serialize_5_ea(
    Eurydice_arr_9e re, Eurydice_mut_borrow_slice_u8 serialized) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficients = compress_b8_f4(
        libcrux_ml_kem_vector_portable_to_unsigned_representative_b8(
            re.data[i0]));
    Eurydice_arr_6d bytes =
        libcrux_ml_kem_vector_portable_serialize_5_b8(coefficients);
    Eurydice_slice_copy(
        Eurydice_slice_subslice_mut_c8(
            serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                            .start = (size_t)10U * i0,
                            .end = (size_t)10U * i0 + (size_t)10U})),
        Eurydice_array_to_slice_shared_30(&bytes), uint8_t);
  }
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.compress_then_serialize_ring_element_v with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- COMPRESSION_FACTOR= 5
- OUT_LEN= 160
*/
static __attribute__((unused)) KRML_MUSTINLINE void compress_then_serialize_ring_element_v_1c(
    Eurydice_arr_9e re, Eurydice_mut_borrow_slice_u8 out) {
  compress_then_serialize_5_ea(re, out);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c2
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- V_COMPRESSION_FACTOR= 5
- C2_LEN= 160
*/
static __attribute__((unused)) KRML_MUSTINLINE void encrypt_c2_1c(
    const Eurydice_arr_d20 *t_as_ntt, const Eurydice_arr_d20 *r_as_ntt,
    const Eurydice_arr_9e *error_2, const Eurydice_arr_ec *message,
    Eurydice_mut_borrow_slice_u8 ciphertext) {
  Eurydice_arr_9e message_as_ring_element =
      deserialize_then_decompress_message_ea(message);
  Eurydice_arr_9e v = compute_ring_element_v_ee(t_as_ntt, r_as_ntt, error_2,
                                                &message_as_ring_element);
  compress_then_serialize_ring_element_v_1c(v, ciphertext);
}

/**
 This function implements <strong>Algorithm 13</strong> of the
 NIST FIPS 203 specification; this is the Kyber CPA-PKE encryption algorithm.

 Algorithm 13 is reproduced below:

 ```plaintext
 Input: encryption key ekₚₖₑ ∈ 𝔹^{384k+32}.
 Input: message m ∈ 𝔹^{32}.
 Input: encryption randomness r ∈ 𝔹^{32}.
 Output: ciphertext c ∈ 𝔹^{32(dᵤk + dᵥ)}.

 N ← 0
 t̂ ← ByteDecode₁₂(ekₚₖₑ[0:384k])
 ρ ← ekₚₖₑ[384k: 384k + 32]
 for (i ← 0; i < k; i++)
     for(j ← 0; j < k; j++)
         Â[i,j] ← SampleNTT(XOF(ρ, i, j))
     end for
 end for
 for(i ← 0; i < k; i++)
     r[i] ← SamplePolyCBD_{η₁}(PRF_{η₁}(r,N))
     N ← N + 1
 end for
 for(i ← 0; i < k; i++)
     e₁[i] ← SamplePolyCBD_{η₂}(PRF_{η₂}(r,N))
     N ← N + 1
 end for
 e₂ ← SamplePolyCBD_{η₂}(PRF_{η₂}(r,N))
 r̂ ← NTT(r)
 u ← NTT-¹(Âᵀ ◦ r̂) + e₁
 μ ← Decompress₁(ByteDecode₁(m)))
 v ← NTT-¹(t̂ᵀ ◦ rˆ) + e₂ + μ
 c₁ ← ByteEncode_{dᵤ}(Compress_{dᵤ}(u))
 c₂ ← ByteEncode_{dᵥ}(Compress_{dᵥ}(v))
 return c ← (c₁ ‖ c₂)
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_unpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_LEN= 1408
- C2_LEN= 160
- U_COMPRESSION_FACTOR= 11
- V_COMPRESSION_FACTOR= 5
- BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d1 encrypt_unpacked_d50(
    const libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94
        *public_key,
    const Eurydice_arr_ec *message, Eurydice_borrow_slice_u8 randomness) {
  Eurydice_arr_d1 ciphertext = {.data = {0U}};
  tuple_12 uu____0 = encrypt_c1_870(
      randomness, &public_key->A,
      Eurydice_array_to_subslice_mut_d416(
          &ciphertext, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = (size_t)0U, .end = (size_t)1408U})));
  Eurydice_arr_d20 r_as_ntt = uu____0.fst;
  Eurydice_arr_9e error_2 = uu____0.snd;
  encrypt_c2_1c(
      &public_key->t_as_ntt, &r_as_ntt, &error_2, message,
      Eurydice_array_to_subslice_from_mut_5f4(&ciphertext, (size_t)1408U));
  return ciphertext;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- VECTOR_U_BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_ind_cca_unpacked_encapsulate_a70(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    const Eurydice_arr_ec *randomness) {
  Eurydice_arr_c7 hashed = encaps_prepare_fe(
      Eurydice_array_to_slice_shared_01(randomness),
      Eurydice_array_to_slice_shared_01(&public_key->public_key_hash));
  Eurydice_borrow_slice_u8_x2 uu____0 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret = uu____0.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____0.snd;
  Eurydice_arr_d1 ciphertext = encrypt_unpacked_d50(
      &public_key->ind_cpa_public_key, randomness, pseudorandomness);
  Eurydice_arr_ec shared_secret_array = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(&shared_secret_array),
                      shared_secret, uint8_t);
  return (KRML_CLITERAL(tuple_25){
      .fst = libcrux_ml_kem_types_from_19_d9(ciphertext),
      .snd = shared_secret_array});
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for
libcrux_ml_kem::ind_cpa::deserialize_then_decompress_u::closure<Vector, K,
CIPHERTEXT_SIZE, U_COMPRESSION_FACTOR>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cpa.deserialize_then_decompress_u.call_mut_35 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- U_COMPRESSION_FACTOR= 11
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_35_1c(void **_) { return ZERO_d6_ea(); }

/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.compress.decompress_ciphertext_coefficient with
const generics
- COEFFICIENT_BITS= 10
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
decompress_ciphertext_coefficient_ef(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int32_t decompressed =
        libcrux_secrets_int_as_i32_f5(a.data[i0]) *
        libcrux_secrets_int_as_i32_f5(
            libcrux_secrets_int_public_integers_classify_27_39(
                LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS));
    decompressed = (int32_t)((uint32_t)decompressed << 1U) +
                   (int32_t)((uint32_t)1 << (uint32_t)10);
    decompressed >>= (uint32_t)(10 + 1);
    a.data[i0] = libcrux_secrets_int_as_i16_36(decompressed);
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.decompress_ciphertext_coefficient_b8 with const
generics
- COEFFICIENT_BITS= 10
*/
static __attribute__((unused)) Eurydice_arr_d6 decompress_ciphertext_coefficient_b8_ef(
    Eurydice_arr_d6 a) {
  return decompress_ciphertext_coefficient_ef(a);
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_10 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_10_ea(Eurydice_borrow_slice_u8 serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  for (size_t i = (size_t)0U; i < serialized.meta / (size_t)20U; i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 bytes = Eurydice_slice_subslice_shared_c8(
        serialized,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * (size_t)20U, .end = i0 * (size_t)20U + (size_t)20U}));
    Eurydice_arr_d6 coefficient =
        libcrux_ml_kem_vector_portable_deserialize_10_b8(bytes);
    Eurydice_arr_d6 uu____0 =
        decompress_ciphertext_coefficient_b8_ef(coefficient);
    re.data[i0] = uu____0;
  }
  return re;
}

/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.compress.decompress_ciphertext_coefficient with
const generics
- COEFFICIENT_BITS= 11
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
decompress_ciphertext_coefficient_c4(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int32_t decompressed =
        libcrux_secrets_int_as_i32_f5(a.data[i0]) *
        libcrux_secrets_int_as_i32_f5(
            libcrux_secrets_int_public_integers_classify_27_39(
                LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS));
    decompressed = (int32_t)((uint32_t)decompressed << 1U) +
                   (int32_t)((uint32_t)1 << (uint32_t)11);
    decompressed >>= (uint32_t)(11 + 1);
    a.data[i0] = libcrux_secrets_int_as_i16_36(decompressed);
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.decompress_ciphertext_coefficient_b8 with const
generics
- COEFFICIENT_BITS= 11
*/
static __attribute__((unused)) Eurydice_arr_d6 decompress_ciphertext_coefficient_b8_c4(
    Eurydice_arr_d6 a) {
  return decompress_ciphertext_coefficient_c4(a);
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_11_ea(Eurydice_borrow_slice_u8 serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  for (size_t i = (size_t)0U; i < serialized.meta / (size_t)22U; i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 bytes = Eurydice_slice_subslice_shared_c8(
        serialized,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * (size_t)22U, .end = i0 * (size_t)22U + (size_t)22U}));
    Eurydice_arr_d6 coefficient =
        libcrux_ml_kem_vector_portable_deserialize_11_b8(bytes);
    Eurydice_arr_d6 uu____0 =
        decompress_ciphertext_coefficient_b8_c4(coefficient);
    re.data[i0] = uu____0;
  }
  return re;
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_ring_element_u with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- COMPRESSION_FACTOR= 11
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_ring_element_u_ee(
    Eurydice_borrow_slice_u8 serialized) {
  return deserialize_then_decompress_11_ea(serialized);
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_vector_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- VECTOR_U_COMPRESSION_FACTOR= 11
*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_vector_u_ee(Eurydice_arr_9e *re) {
  size_t zeta_i = (size_t)0U;
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)7U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)6U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)5U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)4U);
  ntt_at_layer_3_ea(&zeta_i, re);
  ntt_at_layer_2_ea(&zeta_i, re);
  ntt_at_layer_1_ea(&zeta_i, re);
  poly_barrett_reduce_d6_ea(re);
}

/**
 Call [`deserialize_then_decompress_ring_element_u`] on each ring element
 in the `ciphertext`.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.deserialize_then_decompress_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- U_COMPRESSION_FACTOR= 11
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d20
deserialize_then_decompress_u_1c(const Eurydice_arr_d1 *ciphertext) {
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_35_1c(&lvalue););
  Eurydice_arr_d20 u_as_ntt = arr_struct;
  for (size_t i = (size_t)0U;
       i <
       (size_t)1568U / (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                        (size_t)11U / (size_t)8U);
       i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 u_bytes = Eurydice_array_to_subslice_shared_d45(
        ciphertext,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start =
                i0 * (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                      (size_t)11U / (size_t)8U),
            .end = i0 * (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                         (size_t)11U / (size_t)8U) +
                   LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                       (size_t)11U / (size_t)8U}));
    u_as_ntt.data[i0] = deserialize_then_decompress_ring_element_u_ee(u_bytes);
    ntt_vector_u_ee(&u_as_ntt.data[i0]);
  }
  return u_as_ntt;
}

/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.compress.decompress_ciphertext_coefficient with
const generics
- COEFFICIENT_BITS= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
decompress_ciphertext_coefficient_d1(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int32_t decompressed =
        libcrux_secrets_int_as_i32_f5(a.data[i0]) *
        libcrux_secrets_int_as_i32_f5(
            libcrux_secrets_int_public_integers_classify_27_39(
                LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS));
    decompressed = (int32_t)((uint32_t)decompressed << 1U) +
                   (int32_t)((uint32_t)1 << (uint32_t)4);
    decompressed >>= (uint32_t)(4 + 1);
    a.data[i0] = libcrux_secrets_int_as_i16_36(decompressed);
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.decompress_ciphertext_coefficient_b8 with const
generics
- COEFFICIENT_BITS= 4
*/
static __attribute__((unused)) Eurydice_arr_d6 decompress_ciphertext_coefficient_b8_d1(
    Eurydice_arr_d6 a) {
  return decompress_ciphertext_coefficient_d1(a);
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.deserialize_then_decompress_4
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_4_ea(Eurydice_borrow_slice_u8 serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  for (size_t i = (size_t)0U; i < serialized.meta / (size_t)8U; i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 bytes = Eurydice_slice_subslice_shared_c8(
        serialized,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * (size_t)8U, .end = i0 * (size_t)8U + (size_t)8U}));
    Eurydice_arr_d6 coefficient =
        libcrux_ml_kem_vector_portable_deserialize_4_b8(bytes);
    Eurydice_arr_d6 uu____0 =
        decompress_ciphertext_coefficient_b8_d1(coefficient);
    re.data[i0] = uu____0;
  }
  return re;
}

/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.compress.decompress_ciphertext_coefficient with
const generics
- COEFFICIENT_BITS= 5
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d6
decompress_ciphertext_coefficient_f4(Eurydice_arr_d6 a) {
  for (size_t i = (size_t)0U;
       i < LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_ELEMENTS_IN_VECTOR; i++) {
    size_t i0 = i;
    int32_t decompressed =
        libcrux_secrets_int_as_i32_f5(a.data[i0]) *
        libcrux_secrets_int_as_i32_f5(
            libcrux_secrets_int_public_integers_classify_27_39(
                LIBCRUX_ML_KEM_VECTOR_TRAITS_FIELD_MODULUS));
    decompressed = (int32_t)((uint32_t)decompressed << 1U) +
                   (int32_t)((uint32_t)1 << (uint32_t)5);
    decompressed >>= (uint32_t)(5 + 1);
    a.data[i0] = libcrux_secrets_int_as_i16_36(decompressed);
  }
  return a;
}

/**
This function found in impl {libcrux_ml_kem::vector::traits::Operations for
libcrux_ml_kem::vector::portable::vector_type::PortableVector}
*/
/**
A monomorphic instance of
libcrux_ml_kem.vector.portable.decompress_ciphertext_coefficient_b8 with const
generics
- COEFFICIENT_BITS= 5
*/
static __attribute__((unused)) Eurydice_arr_d6 decompress_ciphertext_coefficient_b8_f4(
    Eurydice_arr_d6 a) {
  return decompress_ciphertext_coefficient_f4(a);
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.deserialize_then_decompress_5
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_5_ea(Eurydice_borrow_slice_u8 serialized) {
  Eurydice_arr_9e re = ZERO_d6_ea();
  for (size_t i = (size_t)0U; i < serialized.meta / (size_t)10U; i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 bytes = Eurydice_slice_subslice_shared_c8(
        serialized,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * (size_t)10U, .end = i0 * (size_t)10U + (size_t)10U}));
    re.data[i0] = libcrux_ml_kem_vector_portable_deserialize_5_b8(bytes);
    Eurydice_arr_d6 uu____1 =
        decompress_ciphertext_coefficient_b8_f4(re.data[i0]);
    re.data[i0] = uu____1;
  }
  return re;
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_ring_element_v with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
- COMPRESSION_FACTOR= 5
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_ring_element_v_1c(
    Eurydice_borrow_slice_u8 serialized) {
  return deserialize_then_decompress_5_ea(serialized);
}

/**
A monomorphic instance of libcrux_ml_kem.polynomial.subtract_reduce
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
subtract_reduce_ea(const Eurydice_arr_9e *myself, Eurydice_arr_9e b) {
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficient_normal_form =
        libcrux_ml_kem_vector_portable_montgomery_multiply_by_constant_b8(
            b.data[i0], 1441);
    Eurydice_arr_d6 diff = libcrux_ml_kem_vector_portable_sub_b8(
        myself->data[i0], &coefficient_normal_form);
    Eurydice_arr_d6 red =
        libcrux_ml_kem_vector_portable_barrett_reduce_b8(diff);
    b.data[i0] = red;
  }
  return b;
}

/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.subtract_reduce_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
subtract_reduce_d6_ea(const Eurydice_arr_9e *self, Eurydice_arr_9e b) {
  return subtract_reduce_ea(self, b);
}

/**
 The following functions compute various expressions involving
 vectors and matrices. The computation of these expressions has been
 abstracted away into these functions in order to save on loop iterations.
 Compute v − InverseNTT(sᵀ ◦ NTT(u))
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_message
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e compute_message_ee(
    const Eurydice_arr_9e *v, const Eurydice_arr_d20 *secret_as_ntt,
    const Eurydice_arr_d20 *u_as_ntt) {
  Eurydice_arr_9e result = ZERO_d6_ea();
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U, size_t i0 = i;
                  Eurydice_arr_9e product = ntt_multiply_d6_ea(
                      &secret_as_ntt->data[i0], &u_as_ntt->data[i0]);
                  add_to_ring_element_d6_ee(&result, &product););
  invert_ntt_montgomery_ee(&result);
  return subtract_reduce_d6_ea(v, result);
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.compress_then_serialize_message with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics

*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec
compress_then_serialize_message_ea(Eurydice_arr_9e re) {
  Eurydice_arr_ec serialized = {.data = {0U}};
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_d6 coefficient = to_unsigned_field_modulus_ea(re.data[i0]);
      Eurydice_arr_d6 coefficient_compressed =
          libcrux_ml_kem_vector_portable_compress_1_b8(coefficient);
      Eurydice_array_u8x2 bytes =
          libcrux_ml_kem_vector_portable_serialize_1_b8(coefficient_compressed);
      Eurydice_slice_copy(
          Eurydice_array_to_subslice_mut_d44(
              &serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                               .start = (size_t)2U * i0,
                               .end = (size_t)2U * i0 + (size_t)2U})),
          Eurydice_array_to_slice_shared_82(&bytes), uint8_t););
  return serialized;
}

/**
 This function implements <strong>Algorithm 14</strong> of the
 NIST FIPS 203 specification; this is the Kyber CPA-PKE decryption algorithm.

 Algorithm 14 is reproduced below:

 ```plaintext
 Input: decryption key dkₚₖₑ ∈ 𝔹^{384k}.
 Input: ciphertext c ∈ 𝔹^{32(dᵤk + dᵥ)}.
 Output: message m ∈ 𝔹^{32}.

 c₁ ← c[0 : 32dᵤk]
 c₂ ← c[32dᵤk : 32(dᵤk + dᵥ)]
 u ← Decompress_{dᵤ}(ByteDecode_{dᵤ}(c₁))
 v ← Decompress_{dᵥ}(ByteDecode_{dᵥ}(c₂))
 ŝ ← ByteDecode₁₂(dkₚₖₑ)
 w ← v - NTT-¹(ŝᵀ ◦ NTT(u))
 m ← ByteEncode₁(Compress₁(w))
 return m
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.decrypt_unpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- VECTOR_U_ENCODED_SIZE= 1408
- U_COMPRESSION_FACTOR= 11
- V_COMPRESSION_FACTOR= 5
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec decrypt_unpacked_38(
    const Eurydice_arr_d20 *secret_key, const Eurydice_arr_d1 *ciphertext) {
  Eurydice_arr_d20 u_as_ntt = deserialize_then_decompress_u_1c(ciphertext);
  Eurydice_arr_9e v = deserialize_then_decompress_ring_element_v_1c(
      Eurydice_array_to_subslice_from_shared_5f2(ciphertext, (size_t)1408U));
  Eurydice_arr_9e message = compute_message_ee(&v, secret_key, &u_as_ntt);
  return compress_then_serialize_message_ea(message);
}

/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRF
with const generics
- LEN= 32
*/
static __attribute__((unused)) inline Eurydice_arr_ec PRF_ce(Eurydice_borrow_slice_u8 input) {
  Eurydice_arr_ec digest = {.data = {0U}};
  libcrux_sha3_portable_shake256(Eurydice_array_to_slice_mut_01(&digest),
                                 input);
  return digest;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRF_4a
with const generics
- K= 4
- LEN= 32
*/
static __attribute__((unused)) inline Eurydice_arr_ec PRF_4a_f5(Eurydice_borrow_slice_u8 input) {
  return PRF_ce(input);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1600
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_unpacked_decapsulate_0c0(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    const Eurydice_arr_d1 *ciphertext) {
  Eurydice_arr_ec decrypted = decrypt_unpacked_38(
      &key_pair->private_key.ind_cpa_private_key, ciphertext);
  Eurydice_arr_c7 to_hash0 = libcrux_ml_kem_utils_into_padded_array_c9(
      Eurydice_array_to_slice_shared_01(&decrypted));
  Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_array_to_subslice_from_mut_5f(
      &to_hash0, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE);
  Eurydice_slice_copy(
      uu____0,
      Eurydice_array_to_slice_shared_01(&key_pair->public_key.public_key_hash),
      uint8_t);
  Eurydice_arr_c7 hashed =
      G_4a_23(Eurydice_array_to_slice_shared_17(&to_hash0));
  Eurydice_borrow_slice_u8_x2 uu____1 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret = uu____1.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____1.snd;
  Eurydice_arr_14 to_hash = libcrux_ml_kem_utils_into_padded_array_49(
      Eurydice_array_to_slice_shared_01(
          &key_pair->private_key.implicit_rejection_value));
  Eurydice_mut_borrow_slice_u8 uu____2 =
      Eurydice_array_to_subslice_from_mut_5f3(
          &to_hash, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE);
  Eurydice_slice_copy(uu____2, libcrux_ml_kem_types_as_ref_c1_d9(ciphertext),
                      uint8_t);
  Eurydice_arr_ec implicit_rejection_shared_secret =
      PRF_4a_f5(Eurydice_array_to_slice_shared_720(&to_hash));
  Eurydice_arr_d1 expected_ciphertext = encrypt_unpacked_d50(
      &key_pair->public_key.ind_cpa_public_key, &decrypted, pseudorandomness);
  Eurydice_borrow_slice_u8 uu____3 =
      libcrux_ml_kem_types_as_ref_c1_d9(ciphertext);
  uint8_t selector =
      libcrux_ml_kem_constant_time_ops_compare_ciphertexts_in_constant_time(
          uu____3, Eurydice_array_to_slice_shared_b50(&expected_ciphertext));
  return libcrux_ml_kem_constant_time_ops_select_shared_secret_in_constant_time(
      shared_secret,
      Eurydice_array_to_slice_shared_01(&implicit_rejection_shared_secret),
      selector);
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for
libcrux_ml_kem::serialize::deserialize_ring_elements_reduced_out::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_ring_elements_reduced_out.call_mut_0b with
types libcrux_ml_kem_vector_portable_vector_type_PortableVector with const
generics
- K= 4
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_0b_ee(void **_) { return ZERO_d6_ea(); }

/**
 This function deserializes ring elements and reduces the result by the field
 modulus.

 This function MUST NOT be used on secret inputs.
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_ring_elements_reduced_out with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d20
deserialize_ring_elements_reduced_out_ee(Eurydice_borrow_slice_u8 public_key) {
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_0b_ee(&lvalue););
  Eurydice_arr_d20 deserialized_pk = arr_struct;
  deserialize_ring_elements_reduced_ee(public_key, &deserialized_pk);
  return deserialized_pk;
}

/**
 Validate an ML-KEM public key.

 This implements the Modulus check in 7.2 2.
 Note that the size check in 7.2 1 is covered by the `PUBLIC_KEY_SIZE` in the
 `public_key` type.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_public_key_1c(
    const Eurydice_arr_d1 *public_key) {
  Eurydice_arr_d20 deserialized_pk = deserialize_ring_elements_reduced_out_ee(
      Eurydice_array_to_subslice_to_shared_211(
          public_key,
          libcrux_ml_kem_constants_ranked_bytes_per_ring_element((size_t)4U)));
  Eurydice_arr_d1 public_key_serialized = serialize_public_key_1c(
      &deserialized_pk,
      Eurydice_array_to_subslice_from_shared_5f2(
          public_key,
          libcrux_ml_kem_constants_ranked_bytes_per_ring_element((size_t)4U)));
  return Eurydice_array_eq((size_t)1568U, public_key, &public_key_serialized,
                           uint8_t);
}

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key_only
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_only_4c(
    const Eurydice_arr_a8 *private_key) {
  Eurydice_arr_ec t = H_4a_23(Eurydice_array_to_subslice_shared_d46(
      private_key, (KRML_CLITERAL(core_ops_range_Range_87){
                       .start = (size_t)384U * (size_t)4U,
                       .end = (size_t)768U * (size_t)4U + (size_t)32U})));
  Eurydice_borrow_slice_u8 expected = Eurydice_array_to_subslice_shared_d46(
      private_key, (KRML_CLITERAL(core_ops_range_Range_87){
                       .start = (size_t)768U * (size_t)4U + (size_t)32U,
                       .end = (size_t)768U * (size_t)4U + (size_t)64U}));
  return Eurydice_array_eq_slice_shared((size_t)32U, &t, &expected, uint8_t,
                                        bool);
}

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
 Note that the size checks in 7.2 1 and 2 are covered by the `SECRET_KEY_SIZE`
 and `CIPHERTEXT_SIZE` in the `private_key` and `ciphertext` types.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CIPHERTEXT_SIZE= 1568
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_79(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *_ciphertext) {
  return libcrux_ml_kem_ind_cca_validate_private_key_only_4c(private_key);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- PRIVATE_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE libcrux_ml_kem_utils_extraction_helper_Keypair1024
generate_keypair_300(Eurydice_borrow_slice_u8 key_generation_seed) {
  Eurydice_arr_d20 private_key = default_70_ee();
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94 public_key =
      default_8b_ee();
  generate_keypair_unpacked_390(key_generation_seed, &private_key, &public_key);
  return serialize_unpacked_secret_key_1c(&public_key, &private_key);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.serialize_kem_secret_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- SERIALIZED_KEY_LEN= 3168
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_a8 serialize_kem_secret_key_4c(
    Eurydice_borrow_slice_u8 private_key, Eurydice_borrow_slice_u8 public_key,
    Eurydice_borrow_slice_u8 implicit_rejection_value) {
  Eurydice_arr_a8 out = {.data = {0U}};
  serialize_kem_secret_key_mut_4c(private_key, public_key,
                                  implicit_rejection_value, &out);
  return out;
}

/**
 Packed API

 Generate a key pair.

 Depending on the `Vector` and `Hasher` used, this requires different hardware
 features
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair
libcrux_ml_kem_ind_cca_generate_keypair_b80(const Eurydice_arr_c7 *randomness) {
  Eurydice_borrow_slice_u8 ind_cpa_keypair_randomness =
      Eurydice_array_to_subslice_shared_d43(
          randomness,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end =
                  LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE}));
  Eurydice_borrow_slice_u8 implicit_rejection_value =
      Eurydice_array_to_subslice_from_shared_5f0(
          randomness,
          LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE);
  libcrux_ml_kem_utils_extraction_helper_Keypair1024 uu____0 =
      generate_keypair_300(ind_cpa_keypair_randomness);
  Eurydice_arr_df ind_cpa_private_key = uu____0.fst;
  Eurydice_arr_d1 public_key = uu____0.snd;
  Eurydice_arr_a8 secret_key_serialized = serialize_kem_secret_key_4c(
      Eurydice_array_to_slice_shared_2f(&ind_cpa_private_key),
      Eurydice_array_to_slice_shared_b50(&public_key),
      implicit_rejection_value);
  Eurydice_arr_a8 private_key =
      libcrux_ml_kem_types_from_b2_0e(secret_key_serialized);
  return libcrux_ml_kem_types_from_17_70(
      private_key, libcrux_ml_kem_types_from_51_d9(public_key));
}

/**
This function found in impl {libcrux_ml_kem::variant::Variant for
libcrux_ml_kem::variant::MlKem}
*/
/**
A monomorphic instance of libcrux_ml_kem.variant.entropy_preprocess_39
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec
entropy_preprocess_39_fe(Eurydice_borrow_slice_u8 randomness) {
  Eurydice_arr_ec out = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(&out), randomness,
                      uint8_t);
  return out;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.build_unpacked_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- T_AS_NTT_ENCODED_SIZE= 1536
*/
static __attribute__((unused)) KRML_MUSTINLINE
    libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94
    build_unpacked_public_key_050(Eurydice_borrow_slice_u8 public_key) {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94
      unpacked_public_key = default_8b_ee();
  build_unpacked_public_key_mut_050(public_key, &unpacked_public_key);
  return unpacked_public_key;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]] with const
generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_LEN= 1408
- C2_LEN= 160
- U_COMPRESSION_FACTOR= 11
- V_COMPRESSION_FACTOR= 5
- BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_d1
encrypt_d50(Eurydice_borrow_slice_u8 public_key, const Eurydice_arr_ec *message,
            Eurydice_borrow_slice_u8 randomness) {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_94
      unpacked_public_key = build_unpacked_public_key_050(public_key);
  return encrypt_unpacked_d50(&unpacked_public_key, message, randomness);
}

/**
This function found in impl {libcrux_ml_kem::variant::Variant for
libcrux_ml_kem::variant::MlKem}
*/
/**
A monomorphic instance of libcrux_ml_kem.variant.kdf_39
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]]
with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec
kdf_39_4c(Eurydice_borrow_slice_u8 shared_secret) {
  Eurydice_arr_ec out = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(&out), shared_secret,
                      uint8_t);
  return out;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_ind_cca_encapsulate_990(
    const Eurydice_arr_d1 *public_key, const Eurydice_arr_ec *randomness) {
  Eurydice_arr_ec randomness0 =
      entropy_preprocess_39_fe(Eurydice_array_to_slice_shared_01(randomness));
  Eurydice_arr_c7 to_hash = libcrux_ml_kem_utils_into_padded_array_c9(
      Eurydice_array_to_slice_shared_01(&randomness0));
  Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_array_to_subslice_from_mut_5f(
      &to_hash, LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE);
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_ec lvalue = H_4a_23(Eurydice_array_to_slice_shared_b50(
      libcrux_ml_kem_types_as_slice_e6_d9(public_key)));
  Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_01(&lvalue),
                      uint8_t);
  Eurydice_arr_c7 hashed = G_4a_23(Eurydice_array_to_slice_shared_17(&to_hash));
  Eurydice_borrow_slice_u8_x2 uu____1 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret = uu____1.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____1.snd;
  Eurydice_arr_d1 ciphertext =
      encrypt_d50(Eurydice_array_to_slice_shared_b50(
                      libcrux_ml_kem_types_as_slice_e6_d9(public_key)),
                  &randomness0, pseudorandomness);
  Eurydice_arr_d1 uu____2 = libcrux_ml_kem_types_from_19_d9(ciphertext);
  return (
      KRML_CLITERAL(tuple_25){.fst = uu____2, .snd = kdf_39_4c(shared_secret)});
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for libcrux_ml_kem::ind_cpa::decrypt::closure<Vector, K,
CIPHERTEXT_SIZE, VECTOR_U_ENCODED_SIZE, U_COMPRESSION_FACTOR,
V_COMPRESSION_FACTOR>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.decrypt.call_mut_0b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- VECTOR_U_ENCODED_SIZE= 1408
- U_COMPRESSION_FACTOR= 11
- V_COMPRESSION_FACTOR= 5
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_0b_38(void **_) { return ZERO_d6_ea(); }

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.decrypt
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- VECTOR_U_ENCODED_SIZE= 1408
- U_COMPRESSION_FACTOR= 11
- V_COMPRESSION_FACTOR= 5
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec decrypt_38(
    Eurydice_borrow_slice_u8 secret_key, const Eurydice_arr_d1 *ciphertext) {
  Eurydice_arr_d20 arr_struct;
  KRML_MAYBE_FOR4(i, (size_t)0U, (size_t)4U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_0b_38(&lvalue););
  Eurydice_arr_d20 secret_key_unpacked = arr_struct;
  deserialize_vector_ee(secret_key, &secret_key_unpacked);
  return decrypt_unpacked_38(&secret_key_unpacked, ciphertext);
}

/**
 This code verifies on some machines, runs out of memory on others
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$4size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1600
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_decapsulate_fd0(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext) {
  Eurydice_borrow_slice_u8_x4 uu____0 =
      libcrux_ml_kem_types_unpack_private_key_e3(
          Eurydice_array_to_slice_shared_68(private_key));
  Eurydice_borrow_slice_u8 ind_cpa_secret_key = uu____0.fst;
  Eurydice_borrow_slice_u8 ind_cpa_public_key = uu____0.snd;
  Eurydice_borrow_slice_u8 ind_cpa_public_key_hash = uu____0.thd;
  Eurydice_borrow_slice_u8 implicit_rejection_value = uu____0.f3;
  Eurydice_arr_ec decrypted = decrypt_38(ind_cpa_secret_key, ciphertext);
  Eurydice_arr_c7 to_hash0 = libcrux_ml_kem_utils_into_padded_array_c9(
      Eurydice_array_to_slice_shared_01(&decrypted));
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_from_mut_5f(
          &to_hash0, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE),
      ind_cpa_public_key_hash, uint8_t);
  Eurydice_arr_c7 hashed =
      G_4a_23(Eurydice_array_to_slice_shared_17(&to_hash0));
  Eurydice_borrow_slice_u8_x2 uu____1 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret0 = uu____1.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____1.snd;
  Eurydice_arr_14 to_hash =
      libcrux_ml_kem_utils_into_padded_array_49(implicit_rejection_value);
  Eurydice_mut_borrow_slice_u8 uu____2 =
      Eurydice_array_to_subslice_from_mut_5f3(
          &to_hash, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE);
  Eurydice_slice_copy(uu____2, libcrux_ml_kem_types_as_ref_c1_d9(ciphertext),
                      uint8_t);
  Eurydice_arr_ec implicit_rejection_shared_secret =
      PRF_4a_f5(Eurydice_array_to_slice_shared_720(&to_hash));
  Eurydice_arr_d1 expected_ciphertext =
      encrypt_d50(ind_cpa_public_key, &decrypted, pseudorandomness);
  Eurydice_borrow_slice_u8 uu____3 =
      Eurydice_array_to_slice_shared_01(&implicit_rejection_shared_secret);
  Eurydice_arr_ec implicit_rejection_shared_secret0 = kdf_39_4c(uu____3);
  Eurydice_arr_ec shared_secret = kdf_39_4c(shared_secret0);
  Eurydice_borrow_slice_u8 uu____4 =
      libcrux_ml_kem_types_as_ref_c1_d9(ciphertext);
  return libcrux_ml_kem_constant_time_ops_compare_ciphertexts_select_shared_secret_in_constant_time(
      uu____4, Eurydice_array_to_slice_shared_b50(&expected_ciphertext),
      Eurydice_array_to_slice_shared_01(&shared_secret),
      Eurydice_array_to_slice_shared_01(&implicit_rejection_shared_secret0));
}

/**
 See [deserialize_ring_elements_reduced_out].
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_ring_elements_reduced with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void deserialize_ring_elements_reduced_68(
    Eurydice_borrow_slice_u8 public_key, Eurydice_arr_bb *deserialized_pk) {
  for (size_t i = (size_t)0U;
       i < public_key.meta / LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT;
       i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 ring_element = Eurydice_slice_subslice_shared_c8(
        public_key,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT,
            .end = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT +
                   LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT}));
    Eurydice_arr_9e uu____0 =
        deserialize_to_reduced_ring_element_ea(ring_element);
    deserialized_pk->data[i0] = uu____0;
  }
}

/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_init_absorb_final with const
generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_1b shake128_init_absorb_final_78(
    const Eurydice_arr_81 *input) {
  Eurydice_arr_1b shake128_state;
  Eurydice_arr_7c repeat_expression[3U];
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  repeat_expression[i] =
                      libcrux_sha3_portable_incremental_shake128_init(););
  memcpy(shake128_state.data, repeat_expression,
         (size_t)3U * sizeof(Eurydice_arr_7c));
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
                  libcrux_sha3_portable_incremental_shake128_absorb_final(
                      &shake128_state.data[i0],
                      Eurydice_array_to_slice_shared_e9(&input->data[i0])););
  return shake128_state;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_init_absorb_final_4a with const
generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_1b shake128_init_absorb_final_4a_78(
    const Eurydice_arr_81 *input) {
  return shake128_init_absorb_final_78(input);
}

/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_first_three_blocks with
const generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_7e shake128_squeeze_first_three_blocks_78(
    Eurydice_arr_1b *st) {
  Eurydice_arr_7e out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      libcrux_sha3_portable_incremental_shake128_squeeze_first_three_blocks(
          &st->data[i0], Eurydice_array_to_slice_mut_48(&out.data[i0])););
  return out;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_first_three_blocks_4a
with const generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_7e shake128_squeeze_first_three_blocks_4a_78(
    Eurydice_arr_1b *self) {
  return shake128_squeeze_first_three_blocks_78(self);
}

/**
 If `bytes` contains a set of uniformly random bytes, this function
 uniformly samples a ring element `â` that is treated as being the NTT
 representation of the corresponding polynomial `a`.

 Since rejection sampling is used, it is possible the supplied bytes are
 not enough to sample the element, in which case an `Err` is returned and the
 caller must try again with a fresh set of bytes.

 This function <strong>partially</strong> implements <strong>Algorithm
 6</strong> of the NIST FIPS 203 standard, We say "partially" because this
 implementation only accepts a finite set of bytes as input and returns an error
 if the set is not enough; Algorithm 6 of the FIPS 203 standard on the other
 hand samples from an infinite stream of bytes until the ring element is filled.
 Algorithm 6 is reproduced below:

 ```plaintext
 Input: byte stream B ∈ 𝔹*.
 Output: array â ∈ ℤ₂₅₆.

 i ← 0
 j ← 0
 while j < 256 do
     d₁ ← B[i] + 256·(B[i+1] mod 16)
     d₂ ← ⌊B[i+1]/16⌋ + 16·B[i+2]
     if d₁ < q then
         â[j] ← d₁
         j ← j + 1
     end if
     if d₂ < q and j < 256 then
         â[j] ← d₂
         j ← j + 1
     end if
     i ← i + 3
 end while
 return â
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of
libcrux_ml_kem.sampling.sample_from_uniform_distribution_next with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- N= 504
*/
static __attribute__((unused)) KRML_MUSTINLINE bool sample_from_uniform_distribution_next_b6(
    const Eurydice_arr_7e *randomness, Eurydice_arr_eb *sampled_coefficients,
    Eurydice_arr_b1 *out) {
  KRML_MAYBE_FOR3(
      i0, (size_t)0U, (size_t)3U, (size_t)1U, size_t i1 = i0;
      for (size_t i = (size_t)0U; i < (size_t)504U / (size_t)24U; i++) {
        size_t r = i;
        if (sampled_coefficients->data[i1] <
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
          size_t sampled = libcrux_ml_kem_vector_portable_rej_sample_b8(
              Eurydice_array_to_subslice_shared_d40(
                  &randomness->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = r * (size_t)24U,
                      .end = r * (size_t)24U + (size_t)24U})),
              Eurydice_array_to_subslice_mut_e7(
                  &out->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = sampled_coefficients->data[i1],
                      .end = sampled_coefficients->data[i1] + (size_t)16U})));
          size_t uu____0 = i1;
          sampled_coefficients->data[uu____0] += sampled;
        }
      });
  bool done = true;
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      if (sampled_coefficients->data[i0] >=
          LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
        sampled_coefficients->data[i0] =
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT;
      } else { done = false; });
  return done;
}

/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_next_block with const
generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_2c shake128_squeeze_next_block_78(
    Eurydice_arr_1b *st) {
  Eurydice_arr_2c out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      libcrux_sha3_portable_incremental_shake128_squeeze_next_block(
          &st->data[i0], Eurydice_array_to_slice_mut_2c(&out.data[i0])););
  return out;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of
libcrux_ml_kem.hash_functions.portable.shake128_squeeze_next_block_4a with const
generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_2c shake128_squeeze_next_block_4a_78(
    Eurydice_arr_1b *self) {
  return shake128_squeeze_next_block_78(self);
}

/**
 If `bytes` contains a set of uniformly random bytes, this function
 uniformly samples a ring element `â` that is treated as being the NTT
 representation of the corresponding polynomial `a`.

 Since rejection sampling is used, it is possible the supplied bytes are
 not enough to sample the element, in which case an `Err` is returned and the
 caller must try again with a fresh set of bytes.

 This function <strong>partially</strong> implements <strong>Algorithm
 6</strong> of the NIST FIPS 203 standard, We say "partially" because this
 implementation only accepts a finite set of bytes as input and returns an error
 if the set is not enough; Algorithm 6 of the FIPS 203 standard on the other
 hand samples from an infinite stream of bytes until the ring element is filled.
 Algorithm 6 is reproduced below:

 ```plaintext
 Input: byte stream B ∈ 𝔹*.
 Output: array â ∈ ℤ₂₅₆.

 i ← 0
 j ← 0
 while j < 256 do
     d₁ ← B[i] + 256·(B[i+1] mod 16)
     d₂ ← ⌊B[i+1]/16⌋ + 16·B[i+2]
     if d₁ < q then
         â[j] ← d₁
         j ← j + 1
     end if
     if d₂ < q and j < 256 then
         â[j] ← d₂
         j ← j + 1
     end if
     i ← i + 3
 end while
 return â
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of
libcrux_ml_kem.sampling.sample_from_uniform_distribution_next with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- N= 168
*/
static __attribute__((unused)) KRML_MUSTINLINE bool sample_from_uniform_distribution_next_b60(
    const Eurydice_arr_2c *randomness, Eurydice_arr_eb *sampled_coefficients,
    Eurydice_arr_b1 *out) {
  KRML_MAYBE_FOR3(
      i0, (size_t)0U, (size_t)3U, (size_t)1U, size_t i1 = i0;
      for (size_t i = (size_t)0U; i < (size_t)168U / (size_t)24U; i++) {
        size_t r = i;
        if (sampled_coefficients->data[i1] <
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
          size_t sampled = libcrux_ml_kem_vector_portable_rej_sample_b8(
              Eurydice_array_to_subslice_shared_d41(
                  &randomness->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = r * (size_t)24U,
                      .end = r * (size_t)24U + (size_t)24U})),
              Eurydice_array_to_subslice_mut_e7(
                  &out->data[i1],
                  (KRML_CLITERAL(core_ops_range_Range_87){
                      .start = sampled_coefficients->data[i1],
                      .end = sampled_coefficients->data[i1] + (size_t)16U})));
          size_t uu____0 = i1;
          sampled_coefficients->data[uu____0] += sampled;
        }
      });
  bool done = true;
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      if (sampled_coefficients->data[i0] >=
          LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT) {
        sampled_coefficients->data[i0] =
            LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT;
      } else { done = false; });
  return done;
}

/**
This function found in impl {core::ops::function::FnMut<([i16; 272usize]),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]> for libcrux_ml_kem::sampling::sample_from_xof::closure<Vector,
Hasher, K>[TraitClause@0, TraitClause@1, TraitClause@2, TraitClause@3]}
*/
/**
A monomorphic instance of libcrux_ml_kem.sampling.sample_from_xof.call_mut_0a
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_0a_91(Eurydice_arr_5b tupled_args) {
  Eurydice_arr_5b s = tupled_args;
  return from_i16_array_d6_ea(Eurydice_array_to_subslice_shared_e70(
      &s, (KRML_CLITERAL(core_ops_range_Range_87){.start = (size_t)0U,
                                                  .end = (size_t)256U})));
}

/**
A monomorphic instance of libcrux_ml_kem.sampling.sample_from_xof
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_bb
sample_from_xof_91(const Eurydice_arr_81 *seeds) {
  Eurydice_arr_eb sampled_coefficients = {.data = {0U}};
  Eurydice_arr_b1 out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  Eurydice_arr_1b xof_state = shake128_init_absorb_final_4a_78(seeds);
  Eurydice_arr_7e randomness0 =
      shake128_squeeze_first_three_blocks_4a_78(&xof_state);
  bool done = sample_from_uniform_distribution_next_b6(
      &randomness0, &sampled_coefficients, &out);
  while (true) {
    if (done) {
      break;
    } else {
      Eurydice_arr_2c randomness =
          shake128_squeeze_next_block_4a_78(&xof_state);
      done = sample_from_uniform_distribution_next_b60(
          &randomness, &sampled_coefficients, &out);
    }
  }
  Eurydice_arr_bb arr_mapped_str;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  arr_mapped_str.data[i] = call_mut_0a_91(out.data[i]););
  return arr_mapped_str;
}

/**
A monomorphic instance of libcrux_ml_kem.matrix.sample_matrix_A
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void sample_matrix_A_91(Eurydice_arr_1d *A_transpose,
                                               const Eurydice_arr_31 *seed,
                                               bool transpose) {
  KRML_MAYBE_FOR3(
      i0, (size_t)0U, (size_t)3U, (size_t)1U, size_t i1 = i0;
      Eurydice_arr_81 seeds; Eurydice_arr_31 repeat_expression[3U];
      KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                      repeat_expression[i] =
                          core_array__core__clone__Clone_for__T__N___clone(
                              (size_t)34U, seed, uint8_t, Eurydice_arr_31););
      memcpy(seeds.data, repeat_expression,
             (size_t)3U * sizeof(Eurydice_arr_31));
      KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t j = i;
                      seeds.data[j].data[32U] = (uint8_t)i1;
                      seeds.data[j].data[33U] = (uint8_t)j;);
      Eurydice_arr_bb sampled = sample_from_xof_91(&seeds); KRML_MAYBE_FOR3(
          i, (size_t)0U, (size_t)3U, (size_t)1U, size_t j = i;
          Eurydice_arr_9e sample = sampled.data[j]; if (transpose) {
            A_transpose->data[j].data[i1] = sample;
          } else { A_transpose->data[i1].data[j] = sample; }););
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.H_4a
with const generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_ec H_4a_78(Eurydice_borrow_slice_u8 input) {
  return libcrux_ml_kem_hash_functions_portable_H(input);
}

/**
 Generate an unpacked key from a serialized key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.unpack_public_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- T_AS_NTT_ENCODED_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_unpack_public_key_22(
    const Eurydice_arr_5f *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51
        *unpacked_public_key) {
  Eurydice_borrow_slice_u8 uu____0 =
      Eurydice_array_to_subslice_to_shared_210(public_key, (size_t)1152U);
  deserialize_ring_elements_reduced_68(
      uu____0, &unpacked_public_key->ind_cpa_public_key.t_as_ntt);
  unpacked_public_key->ind_cpa_public_key.seed_for_A =
      libcrux_ml_kem_utils_into_padded_array_ce(
          Eurydice_array_to_subslice_from_shared_5f1(public_key,
                                                     (size_t)1152U));
  Eurydice_arr_1d *uu____2 = &unpacked_public_key->ind_cpa_public_key.A;
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_31 lvalue = libcrux_ml_kem_utils_into_padded_array_de(
      Eurydice_array_to_subslice_from_shared_5f1(public_key, (size_t)1152U));
  sample_matrix_A_91(uu____2, &lvalue, false);
  Eurydice_arr_ec uu____3 = H_4a_78(Eurydice_array_to_slice_shared_ff(
      libcrux_ml_kem_types_as_slice_e6_3d(public_key)));
  unpacked_public_key->public_key_hash = uu____3;
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.public_key_11
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *
libcrux_ml_kem_ind_cca_unpacked_public_key_11_68(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self) {
  return &self->public_key;
}

/**
This function found in impl {core::clone::Clone for
libcrux_ml_kem::ind_cpa::unpacked::IndCpaPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@2]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.unpacked.clone_91
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) inline libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
clone_91_68(
    const libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 *self) {
  Eurydice_arr_bb uu____0 = core_array__core__clone__Clone_for__T__N___clone(
      (size_t)3U, &self->t_as_ntt, Eurydice_arr_9e, Eurydice_arr_bb);
  Eurydice_arr_ec uu____1 = core_array__core__clone__Clone_for__T__N___clone(
      (size_t)32U, &self->seed_for_A, uint8_t, Eurydice_arr_ec);
  return (
      KRML_CLITERAL(libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51){
          .t_as_ntt = uu____0,
          .seed_for_A = uu____1,
          .A = core_array__core__clone__Clone_for__T__N___clone(
              (size_t)3U, &self->A, Eurydice_arr_bb, Eurydice_arr_1d)});
}

/**
This function found in impl {core::clone::Clone for
libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@2]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.clone_d7
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51
libcrux_ml_kem_ind_cca_unpacked_clone_d7_68(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *self) {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 uu____0 =
      clone_91_68(&self->ind_cpa_public_key);
  return (
      KRML_CLITERAL(libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51){
          .ind_cpa_public_key = uu____0,
          .public_key_hash = core_array__core__clone__Clone_for__T__N___clone(
              (size_t)32U, &self->public_key_hash, uint8_t, Eurydice_arr_ec)});
}

/**
 Call [`serialize_uncompressed_ring_element`] for each ring element.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_vector
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void serialize_vector_68(
    const Eurydice_arr_bb *key, Eurydice_mut_borrow_slice_u8 out) {
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e re = key->data[i0];
      Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
          out,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT,
              .end = (i0 + (size_t)1U) *
                     LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT}));
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_b2 lvalue = serialize_uncompressed_ring_element_ea(&re);
      Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_a9(&lvalue),
                          uint8_t););
}

/**
 Concatenate `t` and `ρ` into the public key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_public_key_mut
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) KRML_MUSTINLINE void serialize_public_key_mut_b6(
    const Eurydice_arr_bb *t_as_ntt, Eurydice_borrow_slice_u8 seed_for_a,
    Eurydice_arr_5f *serialized) {
  serialize_vector_68(
      t_as_ntt,
      Eurydice_array_to_subslice_mut_d412(
          serialized,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end = libcrux_ml_kem_constants_ranked_bytes_per_ring_element(
                  (size_t)3U)})));
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_from_mut_5f2(
          serialized,
          libcrux_ml_kem_constants_ranked_bytes_per_ring_element((size_t)3U)),
      seed_for_a, uint8_t);
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.serialized_mut_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_b6(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *self,
    Eurydice_arr_5f *serialized) {
  serialize_public_key_mut_b6(
      &self->ind_cpa_public_key.t_as_ntt,
      Eurydice_array_to_slice_shared_01(&self->ind_cpa_public_key.seed_for_A),
      serialized);
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_mut_11_b6(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self,
    Eurydice_arr_5f *serialized) {
  libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_b6(&self->public_key,
                                                       serialized);
}

/**
 Concatenate `t` and `ρ` into the public key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_5f serialize_public_key_b6(
    const Eurydice_arr_bb *t_as_ntt, Eurydice_borrow_slice_u8 seed_for_a) {
  Eurydice_arr_5f public_key_serialized = {.data = {0U}};
  serialize_public_key_mut_b6(t_as_ntt, seed_for_a, &public_key_serialized);
  return public_key_serialized;
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.serialized_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_5f serialized_dd_b6(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *self) {
  return libcrux_ml_kem_types_from_51_3d(serialize_public_key_b6(
      &self->ind_cpa_public_key.t_as_ntt,
      Eurydice_array_to_slice_shared_01(&self->ind_cpa_public_key.seed_for_A)));
}

/**
 Get the serialized public key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_public_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) Eurydice_arr_5f libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_11_b6(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self) {
  return serialized_dd_b6(&self->public_key);
}

/**
 Serialize the secret key from the unpacked key pair generation.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.serialize_unpacked_secret_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PRIVATE_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) libcrux_ml_kem_utils_extraction_helper_Keypair768
serialize_unpacked_secret_key_30(
    const libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
        *public_key,
    const Eurydice_arr_bb *private_key) {
  Eurydice_arr_5f public_key_serialized = serialize_public_key_b6(
      &public_key->t_as_ntt,
      Eurydice_array_to_slice_shared_01(&public_key->seed_for_A));
  Eurydice_arr_0e secret_key_serialized = {.data = {0U}};
  serialize_vector_68(private_key,
                      Eurydice_array_to_slice_mut_f4(&secret_key_serialized));
  return (KRML_CLITERAL(libcrux_ml_kem_utils_extraction_helper_Keypair768){
      .fst = secret_key_serialized, .snd = public_key_serialized});
}

/**
 Serialize the secret key.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.serialize_kem_secret_key_mut
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- SERIALIZED_KEY_LEN= 2400
*/
static __attribute__((unused)) KRML_MUSTINLINE void serialize_kem_secret_key_mut_52(
    Eurydice_borrow_slice_u8 private_key, Eurydice_borrow_slice_u8 public_key,
    Eurydice_borrow_slice_u8 implicit_rejection_value,
    Eurydice_arr_7d *serialized) {
  size_t pointer = (size_t)0U;
  Eurydice_slice_copy(Eurydice_array_to_subslice_mut_d413(
                          serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                                          .start = pointer,
                                          .end = pointer + private_key.meta})),
                      private_key, uint8_t);
  pointer += private_key.meta;
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_mut_d413(
          serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                          .start = pointer, .end = pointer + public_key.meta})),
      public_key, uint8_t);
  pointer += public_key.meta;
  Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_array_to_subslice_mut_d413(
      serialized,
      (KRML_CLITERAL(core_ops_range_Range_87){
          .start = pointer,
          .end = pointer + LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE}));
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_ec lvalue = H_4a_78(public_key);
  Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_01(&lvalue),
                      uint8_t);
  pointer += LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE;
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_mut_d413(
          serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                          .start = pointer,
                          .end = pointer + implicit_rejection_value.meta})),
      implicit_rejection_value, uint8_t);
}

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_mut_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_21(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self,
    Eurydice_arr_7d *serialized) {
  libcrux_ml_kem_utils_extraction_helper_Keypair768 uu____0 =
      serialize_unpacked_secret_key_30(&self->public_key.ind_cpa_public_key,
                                       &self->private_key.ind_cpa_private_key);
  Eurydice_arr_0e ind_cpa_private_key = uu____0.fst;
  Eurydice_arr_5f ind_cpa_public_key = uu____0.snd;
  serialize_kem_secret_key_mut_52(
      Eurydice_array_to_slice_shared_f4(&ind_cpa_private_key),
      Eurydice_array_to_slice_shared_ff(&ind_cpa_public_key),
      Eurydice_array_to_slice_shared_01(
          &self->private_key.implicit_rejection_value),
      serialized);
}

/**
 Get the serialized private key.
*/
/**
This function found in impl
{libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.serialized_private_key_11 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) Eurydice_arr_7d libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_11_21(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *self) {
  Eurydice_arr_7d sk = libcrux_ml_kem_types_default_d3_79();
  libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_21(self, &sk);
  return sk;
}

/**
 Call [`deserialize_to_uncompressed_ring_element`] for each ring element.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.deserialize_vector
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void deserialize_vector_68(
    Eurydice_borrow_slice_u8 secret_key, Eurydice_arr_bb *secret_as_ntt) {
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e uu____0 = deserialize_to_uncompressed_ring_element_ea(
          Eurydice_slice_subslice_shared_c8(
              secret_key,
              (KRML_CLITERAL(core_ops_range_Range_87){
                  .start = i0 * LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT,
                  .end = (i0 + (size_t)1U) *
                         LIBCRUX_ML_KEM_CONSTANTS_BYTES_PER_RING_ELEMENT})));
      secret_as_ntt->data[i0] = uu____0;);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.build_unpacked_public_key_mut
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- T_AS_NTT_ENCODED_SIZE= 1152
*/
static __attribute__((unused)) KRML_MUSTINLINE void build_unpacked_public_key_mut_05(
    Eurydice_borrow_slice_u8 public_key,
    libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
        *unpacked_public_key) {
  deserialize_ring_elements_reduced_68(
      Eurydice_slice_subslice_to_shared_72(public_key, (size_t)1152U),
      &unpacked_public_key->t_as_ntt);
  Eurydice_borrow_slice_u8 seed =
      Eurydice_slice_subslice_from_shared_6d(public_key, (size_t)1152U);
  Eurydice_arr_1d *uu____0 = &unpacked_public_key->A;
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_31 lvalue = libcrux_ml_kem_utils_into_padded_array_de(seed);
  sample_matrix_A_91(uu____0, &lvalue, false);
}

/**
 Take a serialized private key and generate an unpacked key pair from it.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.keys_from_private_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- T_AS_NTT_ENCODED_SIZE= 1152
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_keys_from_private_key_01(
    const Eurydice_arr_7d *private_key,
    libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *key_pair) {
  Eurydice_borrow_slice_u8_x4 uu____0 =
      libcrux_ml_kem_types_unpack_private_key_64(
          Eurydice_array_to_slice_shared_51(private_key));
  Eurydice_borrow_slice_u8 ind_cpa_secret_key = uu____0.fst;
  Eurydice_borrow_slice_u8 ind_cpa_public_key = uu____0.snd;
  Eurydice_borrow_slice_u8 ind_cpa_public_key_hash = uu____0.thd;
  Eurydice_borrow_slice_u8 implicit_rejection_value = uu____0.f3;
  deserialize_vector_68(ind_cpa_secret_key,
                        &key_pair->private_key.ind_cpa_private_key);
  build_unpacked_public_key_mut_05(ind_cpa_public_key,
                                   &key_pair->public_key.ind_cpa_public_key);
  Eurydice_slice_copy(
      Eurydice_array_to_slice_mut_01(&key_pair->public_key.public_key_hash),
      ind_cpa_public_key_hash, uint8_t);
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(
                          &key_pair->private_key.implicit_rejection_value),
                      implicit_rejection_value, uint8_t);
  Eurydice_slice_copy(
      Eurydice_array_to_slice_mut_01(
          &key_pair->public_key.ind_cpa_public_key.seed_for_A),
      Eurydice_slice_subslice_from_shared_6d(ind_cpa_public_key, (size_t)1152U),
      uint8_t);
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cpa::unpacked::IndCpaPrivateKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.unpacked.default_70
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_bb default_70_68(void) {
  Eurydice_arr_bb lit;
  Eurydice_arr_9e repeat_expression[3U];
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  repeat_expression[i] = ZERO_d6_ea(););
  memcpy(lit.data, repeat_expression, (size_t)3U * sizeof(Eurydice_arr_9e));
  return lit;
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cpa::unpacked::IndCpaPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.unpacked.default_8b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 default_8b_68(
    void) {
  Eurydice_arr_bb uu____0;
  Eurydice_arr_9e repeat_expression0[3U];
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  repeat_expression0[i] = ZERO_d6_ea(););
  memcpy(uu____0.data, repeat_expression0,
         (size_t)3U * sizeof(Eurydice_arr_9e));
  Eurydice_arr_ec uu____1 = {.data = {0U}};
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 lit0;
  lit0.t_as_ntt = uu____0;
  lit0.seed_for_A = uu____1;
  Eurydice_arr_bb repeat_expression1[3U];
  KRML_MAYBE_FOR3(
      i0, (size_t)0U, (size_t)3U, (size_t)1U, Eurydice_arr_bb lit;
      Eurydice_arr_9e repeat_expression[3U];
      KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                      repeat_expression[i] = ZERO_d6_ea(););
      memcpy(lit.data, repeat_expression, (size_t)3U * sizeof(Eurydice_arr_9e));
      repeat_expression1[i0] = lit;);
  memcpy(lit0.A.data, repeat_expression1, (size_t)3U * sizeof(Eurydice_arr_bb));
  return lit0;
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemPublicKeyUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_30
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51
libcrux_ml_kem_ind_cca_unpacked_default_30_68(void) {
  return (
      KRML_CLITERAL(libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51){
          .ind_cpa_public_key = default_8b_68(),
          .public_key_hash = {.data = {0U}}});
}

/**
This function found in impl {core::default::Default for
libcrux_ml_kem::ind_cca::unpacked::MlKemKeyPairUnpacked<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.default_7b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
libcrux_ml_kem_ind_cca_unpacked_default_7b_68(void) {
  libcrux_ml_kem_ind_cca_unpacked_MlKemPrivateKeyUnpacked_51 uu____0 = {
      .ind_cpa_private_key = default_70_68(),
      .implicit_rejection_value = {.data = {0U}}};
  return (KRML_CLITERAL(
      libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked){
      .private_key = uu____0,
      .public_key = libcrux_ml_kem_ind_cca_unpacked_default_30_68()});
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.G_4a
with const generics
- K= 3
*/
static __attribute__((unused)) inline Eurydice_arr_c7 G_4a_78(Eurydice_borrow_slice_u8 input) {
  return libcrux_ml_kem_hash_functions_portable_G(input);
}

/**
This function found in impl {libcrux_ml_kem::variant::Variant for
libcrux_ml_kem::variant::MlKem}
*/
/**
A monomorphic instance of libcrux_ml_kem.variant.cpa_keygen_seed_39
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_c7
cpa_keygen_seed_39_13(Eurydice_borrow_slice_u8 key_generation_seed) {
  Eurydice_arr_fa seed = {.data = {0U}};
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_mut_d48(
          &seed,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end =
                  LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE})),
      key_generation_seed, uint8_t);
  seed.data[LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE] =
      (uint8_t)(size_t)3U;
  return G_4a_78(Eurydice_array_to_slice_shared_b5(&seed));
}

/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRFxN
with const generics
- K= 3
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_58 PRFxN_3b(const Eurydice_arr_800 *input) {
  Eurydice_arr_58 out = {
      .data = {{.data = {0U}}, {.data = {0U}}, {.data = {0U}}}};
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
                  libcrux_sha3_portable_shake256(
                      Eurydice_array_to_slice_mut_78(&out.data[i0]),
                      Eurydice_array_to_slice_shared_b5(&input->data[i0])););
  return out;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRFxN_4a
with const generics
- K= 3
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_58 PRFxN_4a_3b(const Eurydice_arr_800 *input) {
  return PRFxN_3b(input);
}

/**
 Sample a vector of ring elements from a centered binomial distribution and
 convert them into their NTT representations.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.sample_vector_cbd_then_ntt
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- ETA= 2
- ETA_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE uint8_t sample_vector_cbd_then_ntt_bf(
    Eurydice_arr_bb *re_as_ntt, const Eurydice_arr_fa *prf_input,
    uint8_t domain_separator) {
  Eurydice_arr_800 prf_inputs;
  Eurydice_arr_fa repeat_expression[3U];
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U,
      repeat_expression[i] = core_array__core__clone__Clone_for__T__N___clone(
          (size_t)33U, prf_input, uint8_t, Eurydice_arr_fa););
  memcpy(prf_inputs.data, repeat_expression,
         (size_t)3U * sizeof(Eurydice_arr_fa));
  domain_separator =
      libcrux_ml_kem_utils_prf_input_inc_78(&prf_inputs, domain_separator);
  Eurydice_arr_58 prf_outputs = PRFxN_4a_3b(&prf_inputs);
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e uu____0 = sample_from_binomial_distribution_66(
          Eurydice_array_to_slice_shared_78(&prf_outputs.data[i0]));
      re_as_ntt->data[i0] = uu____0;
      ntt_binomially_sampled_ring_element_ea(&re_as_ntt->data[i0]););
  return domain_separator;
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@3]> for
libcrux_ml_kem::ind_cpa::generate_keypair_unpacked::closure<Vector, Hasher,
Scheme, K, ETA1, ETA1_RANDOMNESS_SIZE>[TraitClause@0, TraitClause@1,
TraitClause@2, TraitClause@3, TraitClause@4, TraitClause@5]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cpa.generate_keypair_unpacked.call_mut_73 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_73_39(void **_) { return ZERO_d6_ea(); }

/**
 Given two polynomial ring elements `lhs` and `rhs`, compute the pointwise
 sum of their constituent coefficients.
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_to_ring_element
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void add_to_ring_element_68(Eurydice_arr_9e *myself,
                                                   const Eurydice_arr_9e *rhs) {
  KRML_MAYBE_FOR16(
      i, (size_t)0U, (size_t)16U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_d6 uu____0 = libcrux_ml_kem_vector_portable_add_b8(
          myself->data[i0], &rhs->data[i0]);
      myself->data[i0] = uu____0;);
}

/**
 Given two polynomial ring elements `lhs` and `rhs`, compute the pointwise
 sum of their constituent coefficients.
*/
/**
This function found in impl
{libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.polynomial.add_to_ring_element_d6
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void add_to_ring_element_d6_68(
    Eurydice_arr_9e *self, const Eurydice_arr_9e *rhs) {
  add_to_ring_element_68(self, rhs);
}

/**
 Compute Â ◦ ŝ + ê
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_As_plus_e
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void compute_As_plus_e_68(
    Eurydice_arr_bb *t_as_ntt, const Eurydice_arr_1d *matrix_A,
    const Eurydice_arr_bb *s_as_ntt, const Eurydice_arr_bb *error_as_ntt) {
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      const Eurydice_arr_bb *row = &matrix_A->data[i0];
      Eurydice_arr_9e uu____0 = ZERO_d6_ea(); t_as_ntt->data[i0] = uu____0;
      KRML_MAYBE_FOR3(
          i1, (size_t)0U, (size_t)3U, (size_t)1U, size_t j = i1;
          const Eurydice_arr_9e *matrix_element = &row->data[j];
          Eurydice_arr_9e product =
              ntt_multiply_d6_ea(matrix_element, &s_as_ntt->data[j]);
          add_to_ring_element_d6_68(&t_as_ntt->data[i0], &product););
      add_standard_error_reduce_d6_ea(&t_as_ntt->data[i0],
                                      &error_as_ntt->data[i0]););
}

/**
 This function implements most of <strong>Algorithm 12</strong> of the
 NIST FIPS 203 specification; this is the Kyber CPA-PKE key generation
 algorithm.

 We say "most of" since Algorithm 12 samples the required randomness within
 the function itself, whereas this implementation expects it to be provided
 through the `key_generation_seed` parameter.

 Algorithm 12 is reproduced below:

 ```plaintext
 Output: encryption key ekₚₖₑ ∈ 𝔹^{384k+32}.
 Output: decryption key dkₚₖₑ ∈ 𝔹^{384k}.

 d ←$ B
 (ρ,σ) ← G(d)
 N ← 0
 for (i ← 0; i < k; i++)
     for(j ← 0; j < k; j++)
         Â[i,j] ← SampleNTT(XOF(ρ, i, j))
     end for
 end for
 for(i ← 0; i < k; i++)
     s[i] ← SamplePolyCBD_{η₁}(PRF_{η₁}(σ,N))
     N ← N + 1
 end for
 for(i ← 0; i < k; i++)
     e[i] ← SamplePolyCBD_{η₂}(PRF_{η₂}(σ,N))
     N ← N + 1
 end for
 ŝ ← NTT(s)
 ê ← NTT(e)
 t̂ ← Â◦ŝ + ê
 ekₚₖₑ ← ByteEncode₁₂(t̂) ‖ ρ
 dkₚₖₑ ← ByteEncode₁₂(ŝ)
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.generate_keypair_unpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE void generate_keypair_unpacked_39(
    Eurydice_borrow_slice_u8 key_generation_seed, Eurydice_arr_bb *private_key,
    libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 *public_key) {
  Eurydice_arr_c7 hashed = cpa_keygen_seed_39_13(key_generation_seed);
  Eurydice_borrow_slice_u8_x2 uu____0 = Eurydice_slice_split_at(
      Eurydice_array_to_slice_shared_17(&hashed), (size_t)32U, uint8_t,
      Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 seed_for_A = uu____0.fst;
  Eurydice_borrow_slice_u8 seed_for_secret_and_error = uu____0.snd;
  Eurydice_arr_1d *uu____1 = &public_key->A;
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_31 lvalue0 =
      libcrux_ml_kem_utils_into_padded_array_de(seed_for_A);
  sample_matrix_A_91(uu____1, &lvalue0, true);
  Eurydice_arr_fa prf_input =
      libcrux_ml_kem_utils_into_padded_array_29(seed_for_secret_and_error);
  uint8_t domain_separator =
      sample_vector_cbd_then_ntt_bf(private_key, &prf_input, 0U);
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_73_39(&lvalue););
  Eurydice_arr_bb error_as_ntt = arr_struct;
  sample_vector_cbd_then_ntt_bf(&error_as_ntt, &prf_input, domain_separator);
  compute_As_plus_e_68(&public_key->t_as_ntt, &public_key->A, &private_key[0U],
                       &error_as_ntt);
  Eurydice_arr_ec arr;
  memcpy(arr.data, seed_for_A.ptr, (size_t)32U * sizeof(uint8_t));
  Eurydice_arr_ec uu____2 = core_result_unwrap_26_39((KRML_CLITERAL(
      core_result_Result_07){.tag = core_result_Ok, .val = {.case_Ok = arr}}));
  public_key->seed_for_A = uu____2;
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for
libcrux_ml_kem::ind_cca::unpacked::transpose_a::closure::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.transpose_a.closure.call_mut_b4 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_b4_68(void **_) { return ZERO_d6_ea(); }

/**
This function found in impl {core::ops::function::FnMut<(usize),
[libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]; K]> for
libcrux_ml_kem::ind_cca::unpacked::transpose_a::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.unpacked.transpose_a.call_mut_22 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_bb call_mut_22_68(void **_) {
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_b4_68(&lvalue););
  return arr_struct;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.transpose_a
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_1d transpose_a_68(Eurydice_arr_1d ind_cpa_a) {
  Eurydice_arr_1d arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_22_68(&lvalue););
  Eurydice_arr_1d A = arr_struct;
  KRML_MAYBE_FOR3(
      i0, (size_t)0U, (size_t)3U, (size_t)1U, size_t i1 = i0; KRML_MAYBE_FOR3(
          i, (size_t)0U, (size_t)3U, (size_t)1U, size_t j = i;
          Eurydice_arr_9e uu____0 = clone_c1_ea(&ind_cpa_a.data[j].data[i1]);
          A.data[i1].data[j] = uu____0;););
  return A;
}

/**
 Generate Unpacked Keys
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) void libcrux_ml_kem_ind_cca_unpacked_generate_keypair_b8(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked *out) {
  Eurydice_borrow_slice_u8 ind_cpa_keypair_randomness =
      Eurydice_array_to_subslice_shared_d43(
          &randomness,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end =
                  LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE}));
  Eurydice_borrow_slice_u8 implicit_rejection_value =
      Eurydice_array_to_subslice_from_shared_5f0(
          &randomness,
          LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE);
  generate_keypair_unpacked_39(ind_cpa_keypair_randomness,
                               &out->private_key.ind_cpa_private_key,
                               &out->public_key.ind_cpa_public_key);
  Eurydice_arr_1d A = transpose_a_68(out->public_key.ind_cpa_public_key.A);
  out->public_key.ind_cpa_public_key.A = A;
  Eurydice_arr_5f pk_serialized = serialize_public_key_b6(
      &out->public_key.ind_cpa_public_key.t_as_ntt,
      Eurydice_array_to_slice_shared_01(
          &out->public_key.ind_cpa_public_key.seed_for_A));
  Eurydice_arr_ec uu____0 =
      H_4a_78(Eurydice_array_to_slice_shared_ff(&pk_serialized));
  out->public_key.public_key_hash = uu____0;
  Eurydice_arr_ec arr;
  memcpy(arr.data, implicit_rejection_value.ptr, (size_t)32U * sizeof(uint8_t));
  Eurydice_arr_ec uu____1 = core_result_unwrap_26_39((KRML_CLITERAL(
      core_result_Result_07){.tag = core_result_Ok, .val = {.case_Ok = arr}}));
  out->private_key.implicit_rejection_value = uu____1;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.encaps_prepare
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_c7 encaps_prepare_13(Eurydice_borrow_slice_u8 randomness,
                                         Eurydice_borrow_slice_u8 pk_hash) {
  Eurydice_arr_c7 to_hash =
      libcrux_ml_kem_utils_into_padded_array_c9(randomness);
  Eurydice_slice_copy(Eurydice_array_to_subslice_from_mut_5f(
                          &to_hash, LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE),
                      pk_hash, uint8_t);
  return G_4a_78(Eurydice_array_to_slice_shared_17(&to_hash));
}

/**
A monomorphic instance of n-tuple
with types Eurydice_arr_bb, libcrux_ml_kem_polynomial_PolynomialRingElement_1d

*/
typedef struct tuple_5b_s {
  Eurydice_arr_bb fst;
  Eurydice_arr_9e snd;
} tuple_5b;

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]> for libcrux_ml_kem::ind_cpa::encrypt_c1::closure<Vector, Hasher,
K, C1_LEN, U_COMPRESSION_FACTOR, BLOCK_LEN, ETA1, ETA1_RANDOMNESS_SIZE, ETA2,
ETA2_RANDOMNESS_SIZE>[TraitClause@0, TraitClause@1, TraitClause@2,
TraitClause@3]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c1.call_mut_f1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- C1_LEN= 960
- U_COMPRESSION_FACTOR= 10
- BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_f1_87(void **_) { return ZERO_d6_ea(); }

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@2]> for libcrux_ml_kem::ind_cpa::encrypt_c1::closure#1<Vector,
Hasher, K, C1_LEN, U_COMPRESSION_FACTOR, BLOCK_LEN, ETA1, ETA1_RANDOMNESS_SIZE,
ETA2, ETA2_RANDOMNESS_SIZE>[TraitClause@0, TraitClause@1, TraitClause@2,
TraitClause@3]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c1.call_mut_dd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- C1_LEN= 960
- U_COMPRESSION_FACTOR= 10
- BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_dd_87(void **_) { return ZERO_d6_ea(); }

/**
 Sample a vector of ring elements from a centered binomial distribution.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.sample_ring_element_cbd
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- ETA2_RANDOMNESS_SIZE= 128
- ETA2= 2
*/
static __attribute__((unused)) KRML_MUSTINLINE uint8_t
sample_ring_element_cbd_bf(const Eurydice_arr_fa *prf_input,
                           uint8_t domain_separator, Eurydice_arr_bb *error_1) {
  Eurydice_arr_800 prf_inputs;
  Eurydice_arr_fa repeat_expression[3U];
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U,
      repeat_expression[i] = core_array__core__clone__Clone_for__T__N___clone(
          (size_t)33U, prf_input, uint8_t, Eurydice_arr_fa););
  memcpy(prf_inputs.data, repeat_expression,
         (size_t)3U * sizeof(Eurydice_arr_fa));
  domain_separator =
      libcrux_ml_kem_utils_prf_input_inc_78(&prf_inputs, domain_separator);
  Eurydice_arr_58 prf_outputs = PRFxN_4a_3b(&prf_inputs);
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e uu____0 = sample_from_binomial_distribution_66(
          Eurydice_array_to_slice_shared_78(&prf_outputs.data[i0]));
      error_1->data[i0] = uu____0;);
  return domain_separator;
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRF_4a
with const generics
- K= 3
- LEN= 128
*/
static __attribute__((unused)) inline Eurydice_arr_89 PRF_4a_3b0(Eurydice_borrow_slice_u8 input) {
  return PRF_ec(input);
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for libcrux_ml_kem::matrix::compute_vector_u::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_vector_u.call_mut_a8
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_a8_68(void **_) { return ZERO_d6_ea(); }

/**
A monomorphic instance of libcrux_ml_kem.invert_ntt.invert_ntt_montgomery
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE void invert_ntt_montgomery_68(Eurydice_arr_9e *re) {
  size_t zeta_i =
      LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT / (size_t)2U;
  invert_ntt_at_layer_1_ea(&zeta_i, re);
  invert_ntt_at_layer_2_ea(&zeta_i, re);
  invert_ntt_at_layer_3_ea(&zeta_i, re);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)4U);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)5U);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)6U);
  invert_ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)7U);
  poly_barrett_reduce_d6_ea(re);
}

/**
 Compute u := InvertNTT(Aᵀ ◦ r̂) + e₁
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_vector_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_bb compute_vector_u_68(
    const Eurydice_arr_1d *a_as_ntt, const Eurydice_arr_bb *r_as_ntt,
    const Eurydice_arr_bb *error_1) {
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_a8_68(&lvalue););
  Eurydice_arr_bb result = arr_struct;
  KRML_MAYBE_FOR3(
      i0, (size_t)0U, (size_t)3U, (size_t)1U, size_t i1 = i0;
      const Eurydice_arr_bb *row = &a_as_ntt->data[i1];
      KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t j = i;
                      const Eurydice_arr_9e *a_element = &row->data[j];
                      Eurydice_arr_9e product =
                          ntt_multiply_d6_ea(a_element, &r_as_ntt->data[j]);
                      add_to_ring_element_d6_68(&result.data[i1], &product););
      invert_ntt_montgomery_68(&result.data[i1]);
      add_error_reduce_d6_ea(&result.data[i1], &error_1->data[i1]););
  return result;
}

/**
A monomorphic instance of libcrux_ml_kem.serialize.compress_then_serialize_10
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- OUT_LEN= 320
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_b0
compress_then_serialize_10_e1(const Eurydice_arr_9e *re) {
  Eurydice_arr_b0 serialized = {.data = {0U}};
  for (size_t i = (size_t)0U; i < VECTORS_IN_RING_ELEMENT; i++) {
    size_t i0 = i;
    Eurydice_arr_d6 coefficient =
        compress_b8_ef(to_unsigned_field_modulus_ea(re->data[i0]));
    Eurydice_arr_fc bytes =
        libcrux_ml_kem_vector_portable_serialize_10_b8(coefficient);
    Eurydice_slice_copy(
        Eurydice_array_to_subslice_mut_d49(
            &serialized, (KRML_CLITERAL(core_ops_range_Range_87){
                             .start = (size_t)20U * i0,
                             .end = (size_t)20U * i0 + (size_t)20U})),
        Eurydice_array_to_slice_shared_8f(&bytes), uint8_t);
  }
  return serialized;
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.compress_then_serialize_ring_element_u with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- COMPRESSION_FACTOR= 10
- OUT_LEN= 320
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_b0
compress_then_serialize_ring_element_u_f7(const Eurydice_arr_9e *re) {
  return compress_then_serialize_10_e1(re);
}

/**
 Call [`compress_then_serialize_ring_element_u`] on each ring element.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.compress_then_serialize_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- OUT_LEN= 960
- COMPRESSION_FACTOR= 10
- BLOCK_LEN= 320
*/
static __attribute__((unused)) KRML_MUSTINLINE void compress_then_serialize_u_21(
    Eurydice_arr_bb input, Eurydice_mut_borrow_slice_u8 out) {
  KRML_MAYBE_FOR3(
      i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
      Eurydice_arr_9e re = input.data[i0];
      Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_slice_subslice_mut_c8(
          out, (KRML_CLITERAL(core_ops_range_Range_87){
                   .start = i0 * ((size_t)960U / (size_t)3U),
                   .end = (i0 + (size_t)1U) * ((size_t)960U / (size_t)3U)}));
      /* original Rust expression is not an lvalue in C */
      Eurydice_arr_b0 lvalue = compress_then_serialize_ring_element_u_f7(&re);
      Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_56(&lvalue),
                          uint8_t););
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c1
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- C1_LEN= 960
- U_COMPRESSION_FACTOR= 10
- BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE tuple_5b encrypt_c1_87(
    Eurydice_borrow_slice_u8 randomness, const Eurydice_arr_1d *matrix,
    Eurydice_mut_borrow_slice_u8 ciphertext) {
  Eurydice_arr_fa prf_input =
      libcrux_ml_kem_utils_into_padded_array_29(randomness);
  Eurydice_arr_bb arr_struct0;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct0.data[i] = call_mut_f1_87(&lvalue););
  Eurydice_arr_bb r_as_ntt = arr_struct0;
  uint8_t domain_separator0 =
      sample_vector_cbd_then_ntt_bf(&r_as_ntt, &prf_input, 0U);
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_dd_87(&lvalue););
  Eurydice_arr_bb error_1 = arr_struct;
  uint8_t domain_separator =
      sample_ring_element_cbd_bf(&prf_input, domain_separator0, &error_1);
  prf_input.data[32U] = domain_separator;
  Eurydice_arr_89 prf_output =
      PRF_4a_3b0(Eurydice_array_to_slice_shared_b5(&prf_input));
  Eurydice_arr_9e error_2 = sample_from_binomial_distribution_66(
      Eurydice_array_to_slice_shared_78(&prf_output));
  Eurydice_arr_bb u = compute_vector_u_68(matrix, &r_as_ntt, &error_1);
  compress_then_serialize_u_21(u, ciphertext);
  return (KRML_CLITERAL(tuple_5b){.fst = r_as_ntt, .snd = error_2});
}

/**
 Compute InverseNTT(tᵀ ◦ r̂) + e₂ + message
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_ring_element_v
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e compute_ring_element_v_68(
    const Eurydice_arr_bb *t_as_ntt, const Eurydice_arr_bb *r_as_ntt,
    const Eurydice_arr_9e *error_2, const Eurydice_arr_9e *message) {
  Eurydice_arr_9e result = ZERO_d6_ea();
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
                  Eurydice_arr_9e product = ntt_multiply_d6_ea(
                      &t_as_ntt->data[i0], &r_as_ntt->data[i0]);
                  add_to_ring_element_d6_68(&result, &product););
  invert_ntt_montgomery_68(&result);
  return add_message_error_reduce_d6_ea(error_2, message, result);
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.compress_then_serialize_ring_element_v with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- COMPRESSION_FACTOR= 4
- OUT_LEN= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE void compress_then_serialize_ring_element_v_30(
    Eurydice_arr_9e re, Eurydice_mut_borrow_slice_u8 out) {
  compress_then_serialize_4_ea(re, out);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_c2
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- V_COMPRESSION_FACTOR= 4
- C2_LEN= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE void encrypt_c2_30(
    const Eurydice_arr_bb *t_as_ntt, const Eurydice_arr_bb *r_as_ntt,
    const Eurydice_arr_9e *error_2, const Eurydice_arr_ec *message,
    Eurydice_mut_borrow_slice_u8 ciphertext) {
  Eurydice_arr_9e message_as_ring_element =
      deserialize_then_decompress_message_ea(message);
  Eurydice_arr_9e v = compute_ring_element_v_68(t_as_ntt, r_as_ntt, error_2,
                                                &message_as_ring_element);
  compress_then_serialize_ring_element_v_30(v, ciphertext);
}

/**
 This function implements <strong>Algorithm 13</strong> of the
 NIST FIPS 203 specification; this is the Kyber CPA-PKE encryption algorithm.

 Algorithm 13 is reproduced below:

 ```plaintext
 Input: encryption key ekₚₖₑ ∈ 𝔹^{384k+32}.
 Input: message m ∈ 𝔹^{32}.
 Input: encryption randomness r ∈ 𝔹^{32}.
 Output: ciphertext c ∈ 𝔹^{32(dᵤk + dᵥ)}.

 N ← 0
 t̂ ← ByteDecode₁₂(ekₚₖₑ[0:384k])
 ρ ← ekₚₖₑ[384k: 384k + 32]
 for (i ← 0; i < k; i++)
     for(j ← 0; j < k; j++)
         Â[i,j] ← SampleNTT(XOF(ρ, i, j))
     end for
 end for
 for(i ← 0; i < k; i++)
     r[i] ← SamplePolyCBD_{η₁}(PRF_{η₁}(r,N))
     N ← N + 1
 end for
 for(i ← 0; i < k; i++)
     e₁[i] ← SamplePolyCBD_{η₂}(PRF_{η₂}(r,N))
     N ← N + 1
 end for
 e₂ ← SamplePolyCBD_{η₂}(PRF_{η₂}(r,N))
 r̂ ← NTT(r)
 u ← NTT-¹(Âᵀ ◦ r̂) + e₁
 μ ← Decompress₁(ByteDecode₁(m)))
 v ← NTT-¹(t̂ᵀ ◦ rˆ) + e₂ + μ
 c₁ ← ByteEncode_{dᵤ}(Compress_{dᵤ}(u))
 c₂ ← ByteEncode_{dᵥ}(Compress_{dᵥ}(v))
 return c ← (c₁ ‖ c₂)
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt_unpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_LEN= 960
- C2_LEN= 128
- U_COMPRESSION_FACTOR= 10
- V_COMPRESSION_FACTOR= 4
- BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_2b encrypt_unpacked_d5(
    const libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
        *public_key,
    const Eurydice_arr_ec *message, Eurydice_borrow_slice_u8 randomness) {
  Eurydice_arr_2b ciphertext = {.data = {0U}};
  tuple_5b uu____0 = encrypt_c1_87(
      randomness, &public_key->A,
      Eurydice_array_to_subslice_mut_d410(
          &ciphertext, (KRML_CLITERAL(core_ops_range_Range_87){
                           .start = (size_t)0U, .end = (size_t)960U})));
  Eurydice_arr_bb r_as_ntt = uu____0.fst;
  Eurydice_arr_9e error_2 = uu____0.snd;
  encrypt_c2_30(
      &public_key->t_as_ntt, &r_as_ntt, &error_2, message,
      Eurydice_array_to_subslice_from_mut_5f1(&ciphertext, (size_t)960U));
  return ciphertext;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- PUBLIC_KEY_SIZE= 1184
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- VECTOR_U_BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_f4 libcrux_ml_kem_ind_cca_unpacked_encapsulate_a7(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_51 *public_key,
    const Eurydice_arr_ec *randomness) {
  Eurydice_arr_c7 hashed = encaps_prepare_13(
      Eurydice_array_to_slice_shared_01(randomness),
      Eurydice_array_to_slice_shared_01(&public_key->public_key_hash));
  Eurydice_borrow_slice_u8_x2 uu____0 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret = uu____0.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____0.snd;
  Eurydice_arr_2b ciphertext = encrypt_unpacked_d5(
      &public_key->ind_cpa_public_key, randomness, pseudorandomness);
  Eurydice_arr_ec shared_secret_array = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(&shared_secret_array),
                      shared_secret, uint8_t);
  return (KRML_CLITERAL(tuple_f4){
      .fst = libcrux_ml_kem_types_from_19_52(ciphertext),
      .snd = shared_secret_array});
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for
libcrux_ml_kem::ind_cpa::deserialize_then_decompress_u::closure<Vector, K,
CIPHERTEXT_SIZE, U_COMPRESSION_FACTOR>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cpa.deserialize_then_decompress_u.call_mut_35 with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- U_COMPRESSION_FACTOR= 10
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_35_30(void **_) { return ZERO_d6_ea(); }

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_ring_element_u with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- COMPRESSION_FACTOR= 10
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_ring_element_u_f7(
    Eurydice_borrow_slice_u8 serialized) {
  return deserialize_then_decompress_10_ea(serialized);
}

/**
A monomorphic instance of libcrux_ml_kem.ntt.ntt_vector_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- VECTOR_U_COMPRESSION_FACTOR= 10
*/
static __attribute__((unused)) KRML_MUSTINLINE void ntt_vector_u_f7(Eurydice_arr_9e *re) {
  size_t zeta_i = (size_t)0U;
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)7U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)6U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)5U);
  ntt_at_layer_4_plus_ea(&zeta_i, re, (size_t)4U);
  ntt_at_layer_3_ea(&zeta_i, re);
  ntt_at_layer_2_ea(&zeta_i, re);
  ntt_at_layer_1_ea(&zeta_i, re);
  poly_barrett_reduce_d6_ea(re);
}

/**
 Call [`deserialize_then_decompress_ring_element_u`] on each ring element
 in the `ciphertext`.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.deserialize_then_decompress_u
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- U_COMPRESSION_FACTOR= 10
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_bb
deserialize_then_decompress_u_30(const Eurydice_arr_2b *ciphertext) {
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_35_30(&lvalue););
  Eurydice_arr_bb u_as_ntt = arr_struct;
  for (size_t i = (size_t)0U;
       i <
       (size_t)1088U / (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                        (size_t)10U / (size_t)8U);
       i++) {
    size_t i0 = i;
    Eurydice_borrow_slice_u8 u_bytes = Eurydice_array_to_subslice_shared_d4(
        ciphertext,
        (KRML_CLITERAL(core_ops_range_Range_87){
            .start =
                i0 * (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                      (size_t)10U / (size_t)8U),
            .end = i0 * (LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                         (size_t)10U / (size_t)8U) +
                   LIBCRUX_ML_KEM_CONSTANTS_COEFFICIENTS_IN_RING_ELEMENT *
                       (size_t)10U / (size_t)8U}));
    u_as_ntt.data[i0] = deserialize_then_decompress_ring_element_u_f7(u_bytes);
    ntt_vector_u_f7(&u_as_ntt.data[i0]);
  }
  return u_as_ntt;
}

/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_then_decompress_ring_element_v with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
- COMPRESSION_FACTOR= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e
deserialize_then_decompress_ring_element_v_b6(
    Eurydice_borrow_slice_u8 serialized) {
  return deserialize_then_decompress_4_ea(serialized);
}

/**
 The following functions compute various expressions involving
 vectors and matrices. The computation of these expressions has been
 abstracted away into these functions in order to save on loop iterations.
 Compute v − InverseNTT(sᵀ ◦ NTT(u))
*/
/**
A monomorphic instance of libcrux_ml_kem.matrix.compute_message
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_9e compute_message_68(
    const Eurydice_arr_9e *v, const Eurydice_arr_bb *secret_as_ntt,
    const Eurydice_arr_bb *u_as_ntt) {
  Eurydice_arr_9e result = ZERO_d6_ea();
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U, size_t i0 = i;
                  Eurydice_arr_9e product = ntt_multiply_d6_ea(
                      &secret_as_ntt->data[i0], &u_as_ntt->data[i0]);
                  add_to_ring_element_d6_68(&result, &product););
  invert_ntt_montgomery_68(&result);
  return subtract_reduce_d6_ea(v, result);
}

/**
 This function implements <strong>Algorithm 14</strong> of the
 NIST FIPS 203 specification; this is the Kyber CPA-PKE decryption algorithm.

 Algorithm 14 is reproduced below:

 ```plaintext
 Input: decryption key dkₚₖₑ ∈ 𝔹^{384k}.
 Input: ciphertext c ∈ 𝔹^{32(dᵤk + dᵥ)}.
 Output: message m ∈ 𝔹^{32}.

 c₁ ← c[0 : 32dᵤk]
 c₂ ← c[32dᵤk : 32(dᵤk + dᵥ)]
 u ← Decompress_{dᵤ}(ByteDecode_{dᵤ}(c₁))
 v ← Decompress_{dᵥ}(ByteDecode_{dᵥ}(c₂))
 ŝ ← ByteDecode₁₂(dkₚₖₑ)
 w ← v - NTT-¹(ŝᵀ ◦ NTT(u))
 m ← ByteEncode₁(Compress₁(w))
 return m
 ```

 The NIST FIPS 203 standard can be found at
 <https://csrc.nist.gov/pubs/fips/203/ipd>.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.decrypt_unpacked
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- VECTOR_U_ENCODED_SIZE= 960
- U_COMPRESSION_FACTOR= 10
- V_COMPRESSION_FACTOR= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec decrypt_unpacked_01(
    const Eurydice_arr_bb *secret_key, const Eurydice_arr_2b *ciphertext) {
  Eurydice_arr_bb u_as_ntt = deserialize_then_decompress_u_30(ciphertext);
  Eurydice_arr_9e v = deserialize_then_decompress_ring_element_v_b6(
      Eurydice_array_to_subslice_from_shared_5f(ciphertext, (size_t)960U));
  Eurydice_arr_9e message = compute_message_68(&v, secret_key, &u_as_ntt);
  return compress_then_serialize_message_ea(message);
}

/**
This function found in impl {libcrux_ml_kem::hash_functions::Hash<K> for
libcrux_ml_kem::hash_functions::portable::PortableHash<K>}
*/
/**
A monomorphic instance of libcrux_ml_kem.hash_functions.portable.PRF_4a
with const generics
- K= 3
- LEN= 32
*/
static __attribute__((unused)) inline Eurydice_arr_ec PRF_4a_3b(Eurydice_borrow_slice_u8 input) {
  return PRF_ce(input);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.unpacked.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- CIPHERTEXT_SIZE= 1088
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- C1_BLOCK_SIZE= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1120
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_unpacked_decapsulate_0c(
    const libcrux_ml_kem_mlkem768_portable_unpacked_MlKem768KeyPairUnpacked
        *key_pair,
    const Eurydice_arr_2b *ciphertext) {
  Eurydice_arr_ec decrypted = decrypt_unpacked_01(
      &key_pair->private_key.ind_cpa_private_key, ciphertext);
  Eurydice_arr_c7 to_hash0 = libcrux_ml_kem_utils_into_padded_array_c9(
      Eurydice_array_to_slice_shared_01(&decrypted));
  Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_array_to_subslice_from_mut_5f(
      &to_hash0, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE);
  Eurydice_slice_copy(
      uu____0,
      Eurydice_array_to_slice_shared_01(&key_pair->public_key.public_key_hash),
      uint8_t);
  Eurydice_arr_c7 hashed =
      G_4a_78(Eurydice_array_to_slice_shared_17(&to_hash0));
  Eurydice_borrow_slice_u8_x2 uu____1 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret = uu____1.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____1.snd;
  Eurydice_arr_af to_hash = libcrux_ml_kem_utils_into_padded_array_66(
      Eurydice_array_to_slice_shared_01(
          &key_pair->private_key.implicit_rejection_value));
  Eurydice_mut_borrow_slice_u8 uu____2 =
      Eurydice_array_to_subslice_from_mut_5f0(
          &to_hash, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE);
  Eurydice_slice_copy(uu____2, libcrux_ml_kem_types_as_ref_c1_52(ciphertext),
                      uint8_t);
  Eurydice_arr_ec implicit_rejection_shared_secret =
      PRF_4a_3b(Eurydice_array_to_slice_shared_81(&to_hash));
  Eurydice_arr_2b expected_ciphertext = encrypt_unpacked_d5(
      &key_pair->public_key.ind_cpa_public_key, &decrypted, pseudorandomness);
  Eurydice_borrow_slice_u8 uu____3 =
      libcrux_ml_kem_types_as_ref_c1_52(ciphertext);
  uint8_t selector =
      libcrux_ml_kem_constant_time_ops_compare_ciphertexts_in_constant_time(
          uu____3, Eurydice_array_to_slice_shared_06(&expected_ciphertext));
  return libcrux_ml_kem_constant_time_ops_select_shared_secret_in_constant_time(
      shared_secret,
      Eurydice_array_to_slice_shared_01(&implicit_rejection_shared_secret),
      selector);
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for
libcrux_ml_kem::serialize::deserialize_ring_elements_reduced_out::closure<Vector,
K>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_ring_elements_reduced_out.call_mut_0b with
types libcrux_ml_kem_vector_portable_vector_type_PortableVector with const
generics
- K= 3
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_0b_68(void **_) { return ZERO_d6_ea(); }

/**
 This function deserializes ring elements and reduces the result by the field
 modulus.

 This function MUST NOT be used on secret inputs.
*/
/**
A monomorphic instance of
libcrux_ml_kem.serialize.deserialize_ring_elements_reduced_out with types
libcrux_ml_kem_vector_portable_vector_type_PortableVector with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_bb
deserialize_ring_elements_reduced_out_68(Eurydice_borrow_slice_u8 public_key) {
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_0b_68(&lvalue););
  Eurydice_arr_bb deserialized_pk = arr_struct;
  deserialize_ring_elements_reduced_68(public_key, &deserialized_pk);
  return deserialized_pk;
}

/**
 Validate an ML-KEM public key.

 This implements the Modulus check in 7.2 2.
 Note that the size check in 7.2 1 is covered by the `PUBLIC_KEY_SIZE` in the
 `public_key` type.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- PUBLIC_KEY_SIZE= 1184
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_public_key_b6(
    const Eurydice_arr_5f *public_key) {
  Eurydice_arr_bb deserialized_pk = deserialize_ring_elements_reduced_out_68(
      Eurydice_array_to_subslice_to_shared_210(
          public_key,
          libcrux_ml_kem_constants_ranked_bytes_per_ring_element((size_t)3U)));
  Eurydice_arr_5f public_key_serialized = serialize_public_key_b6(
      &deserialized_pk,
      Eurydice_array_to_subslice_from_shared_5f1(
          public_key,
          libcrux_ml_kem_constants_ranked_bytes_per_ring_element((size_t)3U)));
  return Eurydice_array_eq((size_t)1184U, public_key, &public_key_serialized,
                           uint8_t);
}

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key_only
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_only_52(
    const Eurydice_arr_7d *private_key) {
  Eurydice_arr_ec t = H_4a_78(Eurydice_array_to_subslice_shared_d44(
      private_key, (KRML_CLITERAL(core_ops_range_Range_87){
                       .start = (size_t)384U * (size_t)3U,
                       .end = (size_t)768U * (size_t)3U + (size_t)32U})));
  Eurydice_borrow_slice_u8 expected = Eurydice_array_to_subslice_shared_d44(
      private_key, (KRML_CLITERAL(core_ops_range_Range_87){
                       .start = (size_t)768U * (size_t)3U + (size_t)32U,
                       .end = (size_t)768U * (size_t)3U + (size_t)64U}));
  return Eurydice_array_eq_slice_shared((size_t)32U, &t, &expected, uint8_t,
                                        bool);
}

/**
 Validate an ML-KEM private key.

 This implements the Hash check in 7.3 3.
 Note that the size checks in 7.2 1 and 2 are covered by the `SECRET_KEY_SIZE`
 and `CIPHERTEXT_SIZE` in the `private_key` and `ciphertext` types.
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.validate_private_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CIPHERTEXT_SIZE= 1088
*/
static __attribute__((unused)) bool libcrux_ml_kem_ind_cca_validate_private_key_ba(
    const Eurydice_arr_7d *private_key, const Eurydice_arr_2b *_ciphertext) {
  return libcrux_ml_kem_ind_cca_validate_private_key_only_52(private_key);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- PRIVATE_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE libcrux_ml_kem_utils_extraction_helper_Keypair768
generate_keypair_30(Eurydice_borrow_slice_u8 key_generation_seed) {
  Eurydice_arr_bb private_key = default_70_68();
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51 public_key =
      default_8b_68();
  generate_keypair_unpacked_39(key_generation_seed, &private_key, &public_key);
  return serialize_unpacked_secret_key_30(&public_key, &private_key);
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.serialize_kem_secret_key
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- SERIALIZED_KEY_LEN= 2400
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_7d serialize_kem_secret_key_52(
    Eurydice_borrow_slice_u8 private_key, Eurydice_borrow_slice_u8 public_key,
    Eurydice_borrow_slice_u8 implicit_rejection_value) {
  Eurydice_arr_7d out = {.data = {0U}};
  serialize_kem_secret_key_mut_52(private_key, public_key,
                                  implicit_rejection_value, &out);
  return out;
}

/**
 Packed API

 Generate a key pair.

 Depending on the `Vector` and `Hasher` used, this requires different hardware
 features
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.generate_keypair
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- CPA_PRIVATE_KEY_SIZE= 1152
- PRIVATE_KEY_SIZE= 2400
- PUBLIC_KEY_SIZE= 1184
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem768_MlKem768KeyPair
libcrux_ml_kem_ind_cca_generate_keypair_b8(const Eurydice_arr_c7 *randomness) {
  Eurydice_borrow_slice_u8 ind_cpa_keypair_randomness =
      Eurydice_array_to_subslice_shared_d43(
          randomness,
          (KRML_CLITERAL(core_ops_range_Range_87){
              .start = (size_t)0U,
              .end =
                  LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE}));
  Eurydice_borrow_slice_u8 implicit_rejection_value =
      Eurydice_array_to_subslice_from_shared_5f0(
          randomness,
          LIBCRUX_ML_KEM_CONSTANTS_CPA_PKE_KEY_GENERATION_SEED_SIZE);
  libcrux_ml_kem_utils_extraction_helper_Keypair768 uu____0 =
      generate_keypair_30(ind_cpa_keypair_randomness);
  Eurydice_arr_0e ind_cpa_private_key = uu____0.fst;
  Eurydice_arr_5f public_key = uu____0.snd;
  Eurydice_arr_7d secret_key_serialized = serialize_kem_secret_key_52(
      Eurydice_array_to_slice_shared_f4(&ind_cpa_private_key),
      Eurydice_array_to_slice_shared_ff(&public_key), implicit_rejection_value);
  Eurydice_arr_7d private_key =
      libcrux_ml_kem_types_from_b2_79(secret_key_serialized);
  return libcrux_ml_kem_types_from_17_bc(
      private_key, libcrux_ml_kem_types_from_51_3d(public_key));
}

/**
This function found in impl {libcrux_ml_kem::variant::Variant for
libcrux_ml_kem::variant::MlKem}
*/
/**
A monomorphic instance of libcrux_ml_kem.variant.entropy_preprocess_39
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec
entropy_preprocess_39_13(Eurydice_borrow_slice_u8 randomness) {
  Eurydice_arr_ec out = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(&out), randomness,
                      uint8_t);
  return out;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.build_unpacked_public_key
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- T_AS_NTT_ENCODED_SIZE= 1152
*/
static __attribute__((unused)) KRML_MUSTINLINE
    libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
    build_unpacked_public_key_05(Eurydice_borrow_slice_u8 public_key) {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
      unpacked_public_key = default_8b_68();
  build_unpacked_public_key_mut_05(public_key, &unpacked_public_key);
  return unpacked_public_key;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.encrypt
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]] with const
generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_LEN= 960
- C2_LEN= 128
- U_COMPRESSION_FACTOR= 10
- V_COMPRESSION_FACTOR= 4
- BLOCK_LEN= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_2b
encrypt_d5(Eurydice_borrow_slice_u8 public_key, const Eurydice_arr_ec *message,
           Eurydice_borrow_slice_u8 randomness) {
  libcrux_ml_kem_ind_cpa_unpacked_IndCpaPublicKeyUnpacked_51
      unpacked_public_key = build_unpacked_public_key_05(public_key);
  return encrypt_unpacked_d5(&unpacked_public_key, message, randomness);
}

/**
This function found in impl {libcrux_ml_kem::variant::Variant for
libcrux_ml_kem::variant::MlKem}
*/
/**
A monomorphic instance of libcrux_ml_kem.variant.kdf_39
with types libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]]
with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec
kdf_39_52(Eurydice_borrow_slice_u8 shared_secret) {
  Eurydice_arr_ec out = {.data = {0U}};
  Eurydice_slice_copy(Eurydice_array_to_slice_mut_01(&out), shared_secret,
                      uint8_t);
  return out;
}

/**
A monomorphic instance of libcrux_ml_kem.ind_cca.encapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- PUBLIC_KEY_SIZE= 1184
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- C1_BLOCK_SIZE= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_f4 libcrux_ml_kem_ind_cca_encapsulate_99(
    const Eurydice_arr_5f *public_key, const Eurydice_arr_ec *randomness) {
  Eurydice_arr_ec randomness0 =
      entropy_preprocess_39_13(Eurydice_array_to_slice_shared_01(randomness));
  Eurydice_arr_c7 to_hash = libcrux_ml_kem_utils_into_padded_array_c9(
      Eurydice_array_to_slice_shared_01(&randomness0));
  Eurydice_mut_borrow_slice_u8 uu____0 = Eurydice_array_to_subslice_from_mut_5f(
      &to_hash, LIBCRUX_ML_KEM_CONSTANTS_H_DIGEST_SIZE);
  /* original Rust expression is not an lvalue in C */
  Eurydice_arr_ec lvalue = H_4a_78(Eurydice_array_to_slice_shared_ff(
      libcrux_ml_kem_types_as_slice_e6_3d(public_key)));
  Eurydice_slice_copy(uu____0, Eurydice_array_to_slice_shared_01(&lvalue),
                      uint8_t);
  Eurydice_arr_c7 hashed = G_4a_78(Eurydice_array_to_slice_shared_17(&to_hash));
  Eurydice_borrow_slice_u8_x2 uu____1 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret = uu____1.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____1.snd;
  Eurydice_arr_2b ciphertext =
      encrypt_d5(Eurydice_array_to_slice_shared_ff(
                     libcrux_ml_kem_types_as_slice_e6_3d(public_key)),
                 &randomness0, pseudorandomness);
  Eurydice_arr_2b uu____2 = libcrux_ml_kem_types_from_19_52(ciphertext);
  return (
      KRML_CLITERAL(tuple_f4){.fst = uu____2, .snd = kdf_39_52(shared_secret)});
}

/**
This function found in impl {core::ops::function::FnMut<(usize),
libcrux_ml_kem::polynomial::PolynomialRingElement<Vector>[TraitClause@0,
TraitClause@1]> for libcrux_ml_kem::ind_cpa::decrypt::closure<Vector, K,
CIPHERTEXT_SIZE, VECTOR_U_ENCODED_SIZE, U_COMPRESSION_FACTOR,
V_COMPRESSION_FACTOR>[TraitClause@0, TraitClause@1]}
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.decrypt.call_mut_0b
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- VECTOR_U_ENCODED_SIZE= 960
- U_COMPRESSION_FACTOR= 10
- V_COMPRESSION_FACTOR= 4
*/
static __attribute__((unused)) Eurydice_arr_9e call_mut_0b_01(void **_) { return ZERO_d6_ea(); }

/**
A monomorphic instance of libcrux_ml_kem.ind_cpa.decrypt
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector
with const generics
- K= 3
- CIPHERTEXT_SIZE= 1088
- VECTOR_U_ENCODED_SIZE= 960
- U_COMPRESSION_FACTOR= 10
- V_COMPRESSION_FACTOR= 4
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec decrypt_01(
    Eurydice_borrow_slice_u8 secret_key, const Eurydice_arr_2b *ciphertext) {
  Eurydice_arr_bb arr_struct;
  KRML_MAYBE_FOR3(i, (size_t)0U, (size_t)3U, (size_t)1U,
                  /* original Rust expression is not an lvalue in C */
                  void *lvalue = (void *)0U;
                  arr_struct.data[i] = call_mut_0b_01(&lvalue););
  Eurydice_arr_bb secret_key_unpacked = arr_struct;
  deserialize_vector_68(secret_key, &secret_key_unpacked);
  return decrypt_unpacked_01(&secret_key_unpacked, ciphertext);
}

/**
 This code verifies on some machines, runs out of memory on others
*/
/**
A monomorphic instance of libcrux_ml_kem.ind_cca.decapsulate
with types libcrux_ml_kem_vector_portable_vector_type_PortableVector,
libcrux_ml_kem_hash_functions_portable_PortableHash[[$3size_t]],
libcrux_ml_kem_variant_MlKem with const generics
- K= 3
- SECRET_KEY_SIZE= 2400
- CPA_SECRET_KEY_SIZE= 1152
- PUBLIC_KEY_SIZE= 1184
- CIPHERTEXT_SIZE= 1088
- T_AS_NTT_ENCODED_SIZE= 1152
- C1_SIZE= 960
- C2_SIZE= 128
- VECTOR_U_COMPRESSION_FACTOR= 10
- VECTOR_V_COMPRESSION_FACTOR= 4
- C1_BLOCK_SIZE= 320
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1120
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_ind_cca_decapsulate_fd(
    const Eurydice_arr_7d *private_key, const Eurydice_arr_2b *ciphertext) {
  Eurydice_borrow_slice_u8_x4 uu____0 =
      libcrux_ml_kem_types_unpack_private_key_64(
          Eurydice_array_to_slice_shared_51(private_key));
  Eurydice_borrow_slice_u8 ind_cpa_secret_key = uu____0.fst;
  Eurydice_borrow_slice_u8 ind_cpa_public_key = uu____0.snd;
  Eurydice_borrow_slice_u8 ind_cpa_public_key_hash = uu____0.thd;
  Eurydice_borrow_slice_u8 implicit_rejection_value = uu____0.f3;
  Eurydice_arr_ec decrypted = decrypt_01(ind_cpa_secret_key, ciphertext);
  Eurydice_arr_c7 to_hash0 = libcrux_ml_kem_utils_into_padded_array_c9(
      Eurydice_array_to_slice_shared_01(&decrypted));
  Eurydice_slice_copy(
      Eurydice_array_to_subslice_from_mut_5f(
          &to_hash0, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE),
      ind_cpa_public_key_hash, uint8_t);
  Eurydice_arr_c7 hashed =
      G_4a_78(Eurydice_array_to_slice_shared_17(&to_hash0));
  Eurydice_borrow_slice_u8_x2 uu____1 =
      Eurydice_slice_split_at(Eurydice_array_to_slice_shared_17(&hashed),
                              LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE,
                              uint8_t, Eurydice_borrow_slice_u8_x2);
  Eurydice_borrow_slice_u8 shared_secret0 = uu____1.fst;
  Eurydice_borrow_slice_u8 pseudorandomness = uu____1.snd;
  Eurydice_arr_af to_hash =
      libcrux_ml_kem_utils_into_padded_array_66(implicit_rejection_value);
  Eurydice_mut_borrow_slice_u8 uu____2 =
      Eurydice_array_to_subslice_from_mut_5f0(
          &to_hash, LIBCRUX_ML_KEM_CONSTANTS_SHARED_SECRET_SIZE);
  Eurydice_slice_copy(uu____2, libcrux_ml_kem_types_as_ref_c1_52(ciphertext),
                      uint8_t);
  Eurydice_arr_ec implicit_rejection_shared_secret =
      PRF_4a_3b(Eurydice_array_to_slice_shared_81(&to_hash));
  Eurydice_arr_2b expected_ciphertext =
      encrypt_d5(ind_cpa_public_key, &decrypted, pseudorandomness);
  Eurydice_borrow_slice_u8 uu____3 =
      Eurydice_array_to_slice_shared_01(&implicit_rejection_shared_secret);
  Eurydice_arr_ec implicit_rejection_shared_secret0 = kdf_39_52(uu____3);
  Eurydice_arr_ec shared_secret = kdf_39_52(shared_secret0);
  Eurydice_borrow_slice_u8 uu____4 =
      libcrux_ml_kem_types_as_ref_c1_52(ciphertext);
  return libcrux_ml_kem_constant_time_ops_compare_ciphertexts_select_shared_secret_in_constant_time(
      uu____4, Eurydice_array_to_slice_shared_06(&expected_ciphertext),
      Eurydice_array_to_slice_shared_01(&shared_secret),
      Eurydice_array_to_slice_shared_01(&implicit_rejection_shared_secret0));
}


/* from libcrux/libcrux-ml-kem/extracts/c/generated/libcrux_mlkem1024_portable.c */
/*
 * SPDX-FileCopyrightText: 2025 Cryspen Sarl <info@cryspen.com>
 *
 * SPDX-License-Identifier: MIT or Apache-2.0
 *
 * This code was generated with the following revisions:
 * Charon: e656e17bff6ca5efac8ab6919b9b74cb9a8dd8ad
 * Eurydice: aaa9fa657fb6f09802edb890252040d94cd93982
 * Karamel: 8c19d41458ce5cbfea029ebc03334ba96d149039
 * F*: unset
 * Libcrux: ae092f4921933e45dec31610a1465191171e5b3f
 */



/**
 Portable decapsulate
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.decapsulate with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1600
*/
static __attribute__((unused)) Eurydice_arr_ec decapsulate_85(const Eurydice_arr_a8 *private_key,
                                      const Eurydice_arr_d1 *ciphertext) {
  return libcrux_ml_kem_ind_cca_decapsulate_fd0(private_key, ciphertext);
}

/**
 Decapsulate ML-KEM 1024

 Generates an [`MlKemSharedSecret`].
 The input is a reference to an [`MlKem1024PrivateKey`] and an
 [`MlKem1024Ciphertext`].
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_mlkem1024_portable_decapsulate(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext) {
  return decapsulate_85(private_key, ciphertext);
}

/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.encapsulate with const generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) tuple_25 encapsulate_07(const Eurydice_arr_d1 *public_key,
                               const Eurydice_arr_ec *randomness) {
  return libcrux_ml_kem_ind_cca_encapsulate_990(public_key, randomness);
}

/**
 Encapsulate ML-KEM 1024

 Generates an ([`MlKem1024Ciphertext`], [`MlKemSharedSecret`]) tuple.
 The input is a reference to an [`MlKem1024PublicKey`] and
 [`SHARED_SECRET_SIZE`] bytes of `randomness`.
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_mlkem1024_portable_encapsulate(
    const Eurydice_arr_d1 *public_key, Eurydice_arr_ec randomness) {
  return encapsulate_07(public_key, &randomness);
}

/**
 Portable generate key pair.
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.generate_keypair with const
generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair generate_keypair_b3(
    const Eurydice_arr_c7 *randomness) {
  return libcrux_ml_kem_ind_cca_generate_keypair_b80(randomness);
}

/**
 Generate ML-KEM 1024 Key Pair
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_MlKem1024KeyPair
libcrux_ml_kem_mlkem1024_portable_generate_key_pair(
    Eurydice_arr_c7 randomness) {
  return generate_keypair_b3(&randomness);
}

/**
 Private key validation
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.validate_private_key with const
generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CIPHERTEXT_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE bool validate_private_key_43(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext) {
  return libcrux_ml_kem_ind_cca_validate_private_key_79(private_key,
                                                        ciphertext);
}

/**
 Validate a private key.

 Returns `true` if valid, and `false` otherwise.
*/
static __attribute__((unused)) bool libcrux_ml_kem_mlkem1024_portable_validate_private_key(
    const Eurydice_arr_a8 *private_key, const Eurydice_arr_d1 *ciphertext) {
  return validate_private_key_43(private_key, ciphertext);
}

/**
 Private key validation
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.validate_private_key_only with
const generics
- K= 4
- SECRET_KEY_SIZE= 3168
*/
static __attribute__((unused)) KRML_MUSTINLINE bool validate_private_key_only_f5(
    const Eurydice_arr_a8 *private_key) {
  return libcrux_ml_kem_ind_cca_validate_private_key_only_4c(private_key);
}

/**
 Validate the private key only.

 Returns `true` if valid, and `false` otherwise.
*/
static __attribute__((unused)) bool libcrux_ml_kem_mlkem1024_portable_validate_private_key_only(
    const Eurydice_arr_a8 *private_key) {
  return validate_private_key_only_f5(private_key);
}

/**
 Public key validation
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.validate_public_key with const
generics
- K= 4
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE bool validate_public_key_f5(
    const Eurydice_arr_d1 *public_key) {
  return libcrux_ml_kem_ind_cca_validate_public_key_1c(public_key);
}

/**
 Validate a public key.

 Returns `true` if valid, and `false` otherwise.
*/
static __attribute__((unused)) bool libcrux_ml_kem_mlkem1024_portable_validate_public_key(
    const Eurydice_arr_d1 *public_key) {
  return validate_public_key_f5(public_key);
}

/**
 Unpacked decapsulate
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.unpacked.decapsulate with const
generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- CIPHERTEXT_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- C1_BLOCK_SIZE= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
- IMPLICIT_REJECTION_HASH_INPUT_SIZE= 1600
*/
static __attribute__((unused)) KRML_MUSTINLINE Eurydice_arr_ec decapsulate_850(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    const Eurydice_arr_d1 *ciphertext) {
  return libcrux_ml_kem_ind_cca_unpacked_decapsulate_0c0(key_pair, ciphertext);
}

/**
 Decapsulate ML-KEM 1024 (unpacked)

 Generates an [`MlKemSharedSecret`].
 The input is a reference to an unpacked key pair of type
 [`MlKem1024KeyPairUnpacked`] and an [`MlKem1024Ciphertext`].
*/
static __attribute__((unused)) Eurydice_arr_ec libcrux_ml_kem_mlkem1024_portable_unpacked_decapsulate(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *private_key,
    const Eurydice_arr_d1 *ciphertext) {
  return decapsulate_850(private_key, ciphertext);
}

/**
 Unpacked encapsulate
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.unpacked.encapsulate with const
generics
- K= 4
- CIPHERTEXT_SIZE= 1568
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
- C1_SIZE= 1408
- C2_SIZE= 160
- VECTOR_U_COMPRESSION_FACTOR= 11
- VECTOR_V_COMPRESSION_FACTOR= 5
- VECTOR_U_BLOCK_LEN= 352
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
- ETA2= 2
- ETA2_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE tuple_25 encapsulate_070(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    const Eurydice_arr_ec *randomness) {
  return libcrux_ml_kem_ind_cca_unpacked_encapsulate_a70(public_key,
                                                         randomness);
}

/**
 Encapsulate ML-KEM 1024 (unpacked)

 Generates an ([`MlKem1024Ciphertext`], [`MlKemSharedSecret`]) tuple.
 The input is a reference to an unpacked public key of type
 [`MlKem1024PublicKeyUnpacked`], the SHA3-256 hash of this public key, and
 [`SHARED_SECRET_SIZE`] bytes of `randomness`.
 TODO: The F* prefix opens required modules, it should go away when the
 following issue is resolved: <https://github.com/hacspec/hax/issues/770>
*/
static __attribute__((unused)) tuple_25 libcrux_ml_kem_mlkem1024_portable_unpacked_encapsulate(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    Eurydice_arr_ec randomness) {
  return encapsulate_070(public_key, &randomness);
}

/**
 Generate a key pair
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.unpacked.generate_keypair with
const generics
- K= 4
- CPA_PRIVATE_KEY_SIZE= 1536
- PRIVATE_KEY_SIZE= 3168
- PUBLIC_KEY_SIZE= 1568
- ETA1= 2
- ETA1_RANDOMNESS_SIZE= 128
*/
static __attribute__((unused)) KRML_MUSTINLINE void generate_keypair_b30(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked *out) {
  libcrux_ml_kem_ind_cca_unpacked_generate_keypair_b80(randomness, out);
}

/**
 Generate ML-KEM 1024 Key Pair in "unpacked" form
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_generate_key_pair_mut(
    Eurydice_arr_c7 randomness,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair) {
  generate_keypair_b30(randomness, key_pair);
}

/**
 Generate ML-KEM 1024 Key Pair in "unpacked" form.
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
libcrux_ml_kem_mlkem1024_portable_unpacked_generate_key_pair(
    Eurydice_arr_c7 randomness) {
  libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked key_pair =
      libcrux_ml_kem_ind_cca_unpacked_default_7b_ee();
  libcrux_ml_kem_mlkem1024_portable_unpacked_generate_key_pair_mut(randomness,
                                                                   &key_pair);
  return key_pair;
}

/**
 Create a new, empty unpacked key.
*/
static __attribute__((unused)) libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
libcrux_ml_kem_mlkem1024_portable_unpacked_init_key_pair(void) {
  return libcrux_ml_kem_ind_cca_unpacked_default_7b_ee();
}

/**
 Create a new, empty unpacked public key.
*/
static __attribute__((unused)) libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
libcrux_ml_kem_mlkem1024_portable_unpacked_init_public_key(void) {
  return libcrux_ml_kem_ind_cca_unpacked_default_30_ee();
}

/**
 Take a serialized private key and generate an unpacked key pair from it.
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.unpacked.keypair_from_private_key
with const generics
- K= 4
- SECRET_KEY_SIZE= 3168
- CPA_SECRET_KEY_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
- T_AS_NTT_ENCODED_SIZE= 1536
*/
static __attribute__((unused)) KRML_MUSTINLINE void keypair_from_private_key_39(
    const Eurydice_arr_a8 *private_key,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair) {
  libcrux_ml_kem_ind_cca_unpacked_keys_from_private_key_38(private_key,
                                                           key_pair);
}

/**
 Get an unpacked key from a private key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_from_private_mut(
    const Eurydice_arr_a8 *private_key,
    libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair) {
  keypair_from_private_key_39(private_key, key_pair);
}

/**
 Get the serialized private key.
*/
static __attribute__((unused)) Eurydice_arr_a8
libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_private_key(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair) {
  return libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_11_2e(key_pair);
}

/**
 Get the serialized private key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_private_key_mut(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    Eurydice_arr_a8 *serialized) {
  libcrux_ml_kem_ind_cca_unpacked_serialized_private_key_mut_11_2e(key_pair,
                                                                   serialized);
}

/**
 Get the serialized public key.
*/
static __attribute__((unused)) Eurydice_arr_d1
libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_public_key(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair) {
  return libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_11_1c(key_pair);
}

/**
 Get the serialized public key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_key_pair_serialized_public_key_mut(
    const libcrux_ml_kem_mlkem1024_portable_unpacked_MlKem1024KeyPairUnpacked
        *key_pair,
    Eurydice_arr_d1 *serialized) {
  libcrux_ml_kem_ind_cca_unpacked_serialized_public_key_mut_11_1c(key_pair,
                                                                  serialized);
}

/**
 Get the serialized public key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_serialized_public_key(
    const libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94 *public_key,
    Eurydice_arr_d1 *serialized) {
  libcrux_ml_kem_ind_cca_unpacked_serialized_mut_dd_1c(public_key, serialized);
}

/**
 Get the unpacked public key.
*/
/**
A monomorphic instance of
libcrux_ml_kem.ind_cca.instantiations.portable.unpacked.unpack_public_key with
const generics
- K= 4
- T_AS_NTT_ENCODED_SIZE= 1536
- PUBLIC_KEY_SIZE= 1568
*/
static __attribute__((unused)) KRML_MUSTINLINE void unpack_public_key_43(
    const Eurydice_arr_d1 *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
        *unpacked_public_key) {
  libcrux_ml_kem_ind_cca_unpacked_unpack_public_key_29(public_key,
                                                       unpacked_public_key);
}

/**
 Get the unpacked public key.
*/
static __attribute__((unused)) void libcrux_ml_kem_mlkem1024_portable_unpacked_unpacked_public_key(
    const Eurydice_arr_d1 *public_key,
    libcrux_ml_kem_ind_cca_unpacked_MlKemPublicKeyUnpacked_94
        *unpacked_public_key) {
  unpack_public_key_43(public_key, unpacked_public_key);
}

