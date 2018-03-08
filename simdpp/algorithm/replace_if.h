/*  Copyright (C) 2018  Povilas Kanapickas <povilas@radix.lt>

    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LIBSIMDPP_SIMDPP_ALGORITHM_REPLACE_IF_H
#define LIBSIMDPP_SIMDPP_ALGORITHM_REPLACE_IF_H

#ifndef LIBSIMDPP_SIMD_H
#error "This file must be included through simd.h"
#endif

#include <simdpp/algorithm/transform.h>

namespace simdpp {
   namespace SIMDPP_ARCH_NAMESPACE {

      template<typename T, typename UnaryPredicate>
         void replace_if(T* first, T* last, UnaryPredicate pred , const T& new_val)
      {
         struct local_predicate
         {
            using simd_type_T = typename typetraits<T>::simd_type;
            local_predicate(const UnaryPredicate& pred,const T & new_val) : m_new_val(new_val),m_new_val_simd(splat(new_val)),m_pred(pred) {}

            T   operator()( const T& a) const { return m_pred(a) ? m_new_val : a;}
            simd_type_T operator()(const simd_type_T& a) const { return blend(m_new_val_simd,a,m_pred(a)); }

            T m_new_val;
            simd_type_T  m_new_val_simd;
            UnaryPredicate m_pred;
         };

         transform(first, last, first, local_predicate(pred, new_val));
      }       
   } // namespace SIMDPP_ARCH_NAMESPACE
} // namespace simdpp

#endif //LIBSIMDPP_SIMDPP_ALGORITHM_REPLACE_IF_H
