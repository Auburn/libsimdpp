/*  libsimdpp
    Copyright (C) 2012  p12 tir5c3@yahoo.co.uk

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <simdpp/neon_vfp_sp.h>
#include "../main_test_function.inl"
#include "../arch_registration.h"

static ArchRegistration tester_neon_vfp_sp(SIMDPP_ARCH_NAMESPACE::main_test_function,
                                           "NEON_VFP_SP");