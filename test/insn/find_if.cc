/*  Copyright (C) 2018  Povilas Kanapickas <povilas@radix.lt>

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../utils/test_helpers.h"
#include "../utils/test_results.h"
#include <vector>
#include <numeric>
#include <simdpp/simd.h>

namespace SIMDPP_ARCH_NAMESPACE {

    template<typename T>
    struct UnaryPredicateSupValue
    {
    public:
        UnaryPredicateSupValue(T val) :m_val(val), m_val_simd(simdpp::splat(val)) {}
        using simd_mask_T = typename simdpp::typetraits<T>::simd_mask_type;
        using simd_type_T = typename simdpp::typetraits<T>::simd_type;

        bool                  operator()(T a)           const { return a > m_val; }
        simd_mask_T operator()(const simd_type_T& a) const { return cmp_gt(a, m_val_simd); }

        T m_val;
        simd_type_T m_val_simd;
    };

    template<typename T>
    void test_find_if_type(TestResultsSet& ts, TestReporter& tr)
    {
        using namespace simdpp;
        using vector_aligned_t = std::vector<T, aligned_allocator<T, typetraits<T>::alignment>>;
        {//test prologue
            vector_aligned_t ivect(5);
            std::iota(begin(ivect), end(ivect), T(1));
            const auto SupThree = UnaryPredicateSupValue<T>((T)3);
            auto resstd = std::find_if(begin(ivect), end(ivect), [](T el) { return el > 3; });
            auto res = find_if(ivect.data(), ivect.data() + ivect.size(), SupThree);
            TEST_EQUAL(tr, *resstd, *res);
        }
        { //test main loop and epilogue 
            vector_aligned_t ivect(100);
            std::iota(begin(ivect), end(ivect), T(1));
            const auto predicate = UnaryPredicateSupValue<T>((T)98);
            auto resstd = std::find_if(begin(ivect), end(ivect), [](T el) { return el > 98; });
            auto res = find_if(ivect.data(), ivect.data() + ivect.size(), predicate);
            TEST_EQUAL(tr, *resstd, *res);
        }
        { //test main loop
            vector_aligned_t ivect(100);
            std::iota(begin(ivect), end(ivect), T(1));
            const auto predicate = UnaryPredicateSupValue<T>((T)50);
            auto resstd = std::find_if(begin(ivect), end(ivect), [](T el) { return el > 50; });
            auto res = find_if(ivect.data(), ivect.data() + ivect.size(), predicate);
            TEST_EQUAL(tr, *resstd, *res);
        }
    }

    void test_find_if(TestResults& res, TestReporter& tr)
    {
        using namespace simdpp;
        TestResultsSet& ts = res.new_results_set("find_if");
        test_find_if_type<double>(ts, tr);
        test_find_if_type<float>(ts, tr);
        //test_find_if_type<uint64_t>(ts, tr); //FIXME 
        //test_find_if_type<int64_t>(ts, tr); //FIXME
        test_find_if_type<uint32_t>(ts, tr);
        test_find_if_type<int32_t>(ts, tr);
        test_find_if_type<uint16_t>(ts, tr);
        test_find_if_type<int16_t>(ts, tr);
        test_find_if_type<uint8_t>(ts, tr);
        test_find_if_type<int8_t>(ts, tr);
    }

} // namespace SIMDPP_ARCH_NAMESPACE