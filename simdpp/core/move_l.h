/*  libsimdpp
    Copyright (C) 2013  Povilas Kanapickas povilas@radix.lt
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LIBSIMDPP_SIMDPP_CORE_MOVE_L_H
#define LIBSIMDPP_SIMDPP_CORE_MOVE_L_H

#ifndef LIBSIMDPP_SIMD_H
    #error "This file must be included through simd.h"
#endif

#include <simdpp/types.h>
#include <simdpp/neon/detail/shuffle.h>

namespace simdpp {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace SIMDPP_ARCH_NAMESPACE {
#endif


/// @{
/** Moves the elements in an int8x16 vector to the left by @a shift positions.

    @code
    shift:  pos:| 0   1    .  14  15  |
     0      r = [ a0  a1   .  a14 a15 ]
     1      r = [ a1  a2   .  a15  0  ]
     2      r = [ a2  a3   .   0   0  ]
      ...    ..   .. ..   ...  ..  .. ..
     14     r = [ a15  0   .   0   0  ]
     15     r = [  0   0   .   0   0  ]
     16     r = [  0   0   .   0   0  ]
    @endcode

    @par 256-bit version:
    The lower and higher 128-bit halves are processed as if 128-bit instruction
    was applied to each of them separately.

    @icost{SSE2-AVX, NEON, ALTIVEC, 2}
*/
template<unsigned shift>
gint8x16 move_l(gint8x16 a)
{
    static_assert(shift <= 16, "Selector out of range");
    if (shift == 0) return a;
    if (shift == 16) return uint8x16::zero();

#if SIMDPP_USE_NULL
    uint8x16 r;
    //use int to disable warnings wrt. comparison result always being true/false
    for (int i = 0; i < (int) (16-shift); i++) {
        r[i] = a[i + shift];
    }
    for (unsigned i = 16-shift; i < 16; i++) {
        r[i] = 0;
    }
    return r;
#elif SIMDPP_USE_SSE2
    return _mm_srli_si128(a, shift);
#elif SIMDPP_USE_NEON
    int8x16 z = int8x16::zero();
    return neon::detail::align<shift>(a, z);
#elif SIMDPP_USE_ALTIVEC
    // return align<shift>(a, uint8x16::zero());
    return vec_sld((__vector uint8_t)a, (__vector uint8_t)uint8x16::zero(), shift);
#endif
}

template<unsigned shift>
gint8x32 move_l(gint8x32 a)
{
    static_assert(shift <= 16, "Selector out of range");
    if (shift == 0) return a;
    if (shift == 16) return uint8x32::zero();

#if SIMDPP_USE_AVX2
    return _mm256_srli_si256(a, shift);
#else
    return {move_l<shift>(a[0]), move_l<shift>(a[1])};
#endif
}
/// @}

/// @{
/** Moves the 16-bit elements in a vector to the left by @a shift positions.

    @code
    shift:  pos:| 0  1   . 6  7  |
     0      r = [ a0 a1  . a6 a7 ]
     1      r = [ a1 a2  . a7  0 ]
     2      r = [ a2 a3  .  0  0 ]
      ...    ..   .. .. ... .. ..
     6      r = [ a6 a7  .  0  0 ]
     7      r = [ a7  0  .  0  0 ]
     8      r = [  0  0  .  0  0 ]
    @endcode

    @par 256-bit version:
    The lower and higher 128-bit halves are processed as if 128-bit instruction
    was applied to each of them separately.

    @icost{SSE2-AVX, NEON, ALTIVEC, 2}
*/
template<unsigned shift>
gint16x8 move_l(gint16x8 a)
{
    return move_l<shift*2>(gint8x16(a));
}

template<unsigned shift>
gint16x16 move_l(gint16x16 a)
{
    return move_l<shift*2>(gint8x32(a));
}
/// @}

/// @{
/** Moves the 32-bit elements in a vector to the left by @a shift positions.

    @code
    shift:  pos:| 0  1  2  3  |
     0      r = [ a0 a1 a2 a3 ]
     1      r = [ a1 a2 a3  0 ]
     2      r = [ a2 a3  0  0 ]
     3      r = [ a3  0  0  0 ]
     4      r = [  0  0  0  0 ]
    @endcode

    @par 256-bit version:
    The lower and higher 128-bit halves are processed as if 128-bit instruction
    was applied to each of them separately.

    @icost{SSE2-AVX, NEON, ALTIVEC, 2}
*/
template<unsigned shift>
gint32x4 move_l(gint32x4 a)
{
    return move_l<shift*4>(gint8x16(a));
}

template<unsigned shift>
gint32x8 move_l(gint32x8 a)
{
    return move_l<shift*4>(gint8x32(a));
}
/// @}

/// @{
/** Moves the 64-bit elements in a vector to the left by @a shift positions.

    @code
    shift:  pos:| 0  1  |
     0      r = [ a0 a1 ]
     1      r = [ a1  0 ]
     2      r = [  0  0 ]
    @endcode

    @par 256-bit version:
    The lower and higher 128-bit halves are processed as if 128-bit instruction
    was applied to each of them separately.

    @icost{SSE2-AVX, NEON, ALTIVEC, 2}
*/
template<unsigned shift>
gint64x2 move_l(gint64x2 a)
{
    return move_l<shift*8>(gint8x16(a));
}

template<unsigned shift>
gint64x4 move_l(gint64x4 a)
{
    return move_l<shift*8>(gint8x32(a));
}
/// @}

/// @{
/** Moves the 32-bit elements in a vector to the left by @a shift positions.

    @code
    shift:  pos:| 0  1  2  3  |
     0      r = [ a0 a1 a2 a3 ]
     1      r = [ a1 a2 a3  0 ]
     2      r = [ a2 a3  0  0 ]
     3      r = [ a3  0  0  0 ]
     4      r = [  0  0  0  0 ]
    @endcode

    @par 256-bit version:
    The lower and higher 128-bit halves are processed as if 128-bit instruction
    was applied to each of them separately.

    @icost{SSE2-AVX, NEON, ALTIVEC, 2}
*/
template<unsigned shift>
float32x4 move_l(float32x4 a)
{
    return float32x4(move_l<shift>(gint32x4(a)));
}

template<unsigned shift>
float32x8 move_l(float32x8 a)
{
    return float32x8(move_l<shift>(gint32x8(a)));
}
/// @}

/// @{
/** Moves the 64-bit elements in a vector to the left by @a shift positions.

    @code
    shift:  pos:| 0  1  |
     0      r = [ a0 a1 ]
     1      r = [ a1  0 ]
     2      r = [  0  0 ]
    @endcode

    @par 256-bit version:
    The lower and higher 128-bit halves are processed as if 128-bit instruction
    was applied to each of them separately.

    @icost{SSE2-AVX, NEON, ALTIVEC, 2}
*/
template<unsigned shift>
float64x2 move_l(float64x2 a)
{
    return float64x2(move_l<shift>(gint64x2(a)));
}

template<unsigned shift>
float64x4 move_l(float64x4 a)
{
    return float64x4(move_l<shift>(gint64x4(a)));
}
/// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
} // namespace SIMDPP_ARCH_NAMESPACE
#endif
} // namespace simdpp

#endif

