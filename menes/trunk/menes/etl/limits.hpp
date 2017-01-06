/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_ETL_LIMITS_HPP
#define MENES_ETL_LIMITS_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/traits.hpp"

#include <limits> //OK

namespace etl {

template <typename Type_, bool IsArray_ = IsArray<Type_>::value>
struct Limits {
    static const bool IsArithmetic = false;
    static const bool IsIntegral = false;
    static const bool IsFloating = false;
};

template <typename Type_>
struct Limits<Type_, false> {
    static const bool IsArithmetic = std::numeric_limits<Type_>::is_specialized;
    static const bool IsIntegral = std::numeric_limits<Type_>::is_integer;
    static const bool IsSigned = std::numeric_limits<Type_>::is_signed;
    static const bool IsFloating = std::numeric_limits<Type_>::is_iec559;

    static const unsigned Bits = sizeof(Type_) * 8;

    static const unsigned Digits2 = Bits - (IsSigned ? 1 : 0);
    static const unsigned Digits10 = std::numeric_limits<Type_>::digits10;

    _finline static Type_ Min() {
        return std::numeric_limits<Type_>::min();
    }

    _finline static Type_ Max() {
        return std::numeric_limits<Type_>::max();
    }
};

}

#endif//MENES_ETL_LIMITS_HPP
