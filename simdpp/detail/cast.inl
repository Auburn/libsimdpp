/*  Copyright (C) 2013-2014  Povilas Kanapickas <povilas@radix.lt>

    Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
            http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LIBSIMDPP_SIMDPP_DETAIL_INSN_DETAIL_CAST_INL
#define LIBSIMDPP_SIMDPP_DETAIL_INSN_DETAIL_CAST_INL

#ifndef LIBSIMDPP_SIMD_H
    #error "This file must be included through simd.h"
#endif

#include <simdpp/types.h>
#include <simdpp/detail/cast.h>
#include <simdpp/core/to_mask.h>
#include <simdpp/types/traits.h>

namespace simdpp {
namespace SIMDPP_ARCH_NAMESPACE {
namespace detail {

template<class T> struct base_mask_vector_type { typedef T type; };
template<unsigned N> struct base_mask_vector_type<mask_int8<N> > { typedef uint8<N> type; };
template<unsigned N> struct base_mask_vector_type<mask_int16<N> > { typedef uint16<N> type; };
template<unsigned N> struct base_mask_vector_type<mask_int32<N> > { typedef uint32<N> type; };
template<unsigned N> struct base_mask_vector_type<mask_int64<N> > { typedef uint64<N> type; };
template<unsigned N> struct base_mask_vector_type<mask_float32<N> > { typedef float32<N> type; };
template<unsigned N> struct base_mask_vector_type<mask_float64<N> > { typedef float64<N> type; };

/*  Note that in this function we are invoking undefined behavior that happens
    to work in all compilers the library supports. The only non-undefined way
    to do bitwise data transfer between unrelated types without breaking strict
    aliasing rules is the memcpy() function. Unfortunately some compilers can't
    fully optimize out the overhead of the function which leads to unnecessary
    data movement to the stack.
*/
template<class T, class R> SIMDPP_INL
void cast_bitwise(const T& t, R& r)
{
    SIMDPP_STATIC_ASSERT(sizeof(R) == sizeof(T), "Size mismatch");
    union {
        T t_union;
        R r_union;
    };
    t_union = t;
    r = r_union;
}

#ifdef __GNUC__
// t_union and r_union on GCC 4.4
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

template<class T, class R> SIMDPP_INL
void cast_bitwise_vector(const T& t, R& r)
{
    SIMDPP_STATIC_ASSERT(sizeof(R) == sizeof(T), "Size mismatch");
#if 1 || __cplusplus < 201103L || (defined(_MSC_VER) && _MSC_VER < 1900)
    // We can't create use union of vector types because they are not trivial
    // types. In pre-C++11 mode they have empty user-defined default
    // constructor, which is required as the compiler will not provide one
    // by default due to additional constructors being present. Instead we
    // create union of native_type which are trivial types in all supported
    // configurations.
    typedef typename T::base_vector_type::native_type NativeT;
    typedef typename R::base_vector_type::native_type NativeR;
    union {
        NativeT t_union[T::vec_length];
        NativeR r_union[R::vec_length];
    };
    for (unsigned i = 0; i < T::vec_length; ++i)
        t_union[i] = t.vec(i).native();
    for (unsigned i = 0; i < R::vec_length; ++i)
        r.vec(i) = r_union[i];
#else
    union {
        T t_union;
        R r_union;
    };
    t_union = t;
    r = r_union;
#endif
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template<class T, class R> SIMDPP_INL
void cast_bitwise_unmask(const T& t, R& r)
{
    typedef typename base_mask_vector_type<T>::type TT;
    TT tt = t.unmask();
    cast_bitwise_vector(tt, r);
}

template<class T, class R> SIMDPP_INL
void cast_bitwise_remask(const T& t, R& r)
{
    typedef typename base_mask_vector_type<R>::type BaseMaskVector;
    BaseMaskVector rr;
    cast_bitwise_vector(t.unmask(), rr);
    r = to_mask(rr);
}

template<>
struct cast_wrapper<CAST_TYPE_OTHER> {
    template<class T, class R> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        cast_bitwise(t, r);
    }
};

template<>
struct cast_wrapper<CAST_TYPE_MASK_TO_MASK_BITWISE> {
    template<class T, class R> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        SIMDPP_STATIC_ASSERT(R::size_tag == T::size_tag,
                      "Conversions between masks with different element size is"
                      " not allowed");
        cast_bitwise_vector(t.eval(), r);
    }
};

template<>
struct cast_wrapper<CAST_TYPE_MASK_TO_MASK_UNMASK> {
    template<class R, class T> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        SIMDPP_STATIC_ASSERT(R::size_tag == T::size_tag,
                      "Conversions between masks with different element size is"
                      " not allowed");
        cast_bitwise_unmask(t.eval(), r);
    }
};

template<>
struct cast_wrapper<CAST_TYPE_MASK_TO_MASK_REMASK> {
    template<class R, class T> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        SIMDPP_STATIC_ASSERT(R::size_tag == T::size_tag,
                      "Conversions between masks with different element size is"
                      " not allowed");
        cast_bitwise_remask(t.eval(), r);
    }
};

template<>
struct cast_wrapper<CAST_TYPE_VECTOR_TO_MASK> {
    template<class R, class T> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        (void) t; (void) r;
        SIMDPP_STATIC_ASSERT(!(detail::is_same<T,T>::value), // fake dependency
                      "Conversion from non-mask type to a mask type is not allowed");
    }
};

template<>
struct cast_wrapper<CAST_TYPE_MASK_TO_VECTOR> {
    template<class R, class T> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        cast_bitwise_unmask(t.eval(), r);
    }
};

template<>
struct cast_wrapper<CAST_TYPE_VECTOR_TO_VECTOR> {
    template<class R, class T> SIMDPP_INL
    static void run(const T& t, R& r)
    {
        cast_bitwise_vector(t.eval(), r);
    }
};

} // namespace detail
} // namespace SIMDPP_ARCH_NAMESPACE
} // namespace simdpp

#endif

