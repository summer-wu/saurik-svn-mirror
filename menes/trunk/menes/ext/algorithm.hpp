/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_EXT_ALGORITHM_HPP
#define MENES_EXT_ALGORITHM_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/template.hpp"
#include "etl/traits.hpp"

#include <cstring>

namespace ext {

template <typename Type_>
inline Type_ &min(Type_ &arg0, Type_ &arg1) {
    return arg0 < arg1 ? arg0 : arg1;
}

template <typename Type_>
inline Type_ &max(Type_ &arg0, Type_ &arg1) {
    return arg0 < arg1 ? arg1 : arg0;
}


template <typename Type_>
inline const Type_ &min(const Type_ &arg0, const Type_ &arg1) {
    return arg0 < arg1 ? arg0 : arg1;
}

template <typename Type_>
inline const Type_ &max(const Type_ &arg0, const Type_ &arg1) {
    return arg0 < arg1 ? arg1 : arg0;
}


template <typename Type_>
inline Type_ &min(Type_ &arg0, Type_ &arg1, Type_ &arg2) {
    return min(min(arg0, arg1), arg2);
}

template <typename Type_>
inline Type_ &max(Type_ &arg0, Type_ &arg1, Type_ &arg2) {
    return max(max(arg0, arg1), arg2);
}


template <typename Type_>
inline const Type_ &min(const Type_ &arg0, const Type_ &arg1, const Type_ &arg2) {
    return min(min(arg0, arg1), arg2);
}

template <typename Type_>
inline const Type_ &max(const Type_ &arg0, const Type_ &arg1, const Type_ &arg2) {
    return max(max(arg0, arg1), arg2);
}


template <typename Type_>
inline Type_ &min(Type_ &arg0, Type_ &arg1, Type_ &arg2, Type_ &arg3) {
    return min(min(arg0, arg1), min(arg2, arg3));
}

template <typename Type_>
inline Type_ &max(Type_ &arg0, Type_ &arg1, Type_ &arg2, Type_ &arg3) {
    return max(max(arg0, arg1), max(arg2, arg3));
}


template <typename Type_>
inline const Type_ &min(const Type_ &arg0, const Type_ &arg1, const Type_ &arg2, const Type_ &arg3) {
    return min(min(arg0, arg1), min(arg2, arg3));
}

template <typename Type_>
inline const Type_ &max(const Type_ &arg0, const Type_ &arg1, const Type_ &arg2, const Type_ &arg3) {
    return max(max(arg0, arg1), max(arg2, arg3));
}

template <typename Type_, typename Iterator_>
inline Type_ Sum(Iterator_ begin, Iterator_ end) {
    if (begin == end)
        return Type_();

    Type_ total(*(begin++));
    while (begin != end)
        total += *(begin++);
    return total;
}

template <typename Type_>
inline typename etl::IfTrue<etl::IsMemMovable<Type_>::value, void>::Result
Swap(Type_ &lhs, Type_ &rhs) {
    char temp[sizeof(Type_)];
    ::memcpy(temp, &lhs, sizeof(Type_));
    ::memcpy(&lhs, &rhs, sizeof(Type_));
    ::memcpy(&rhs, temp, sizeof(Type_));
}

template <typename Type_>
inline typename etl::IfTrue<!etl::IsMemMovable<Type_>::value && !etl::IsArray<Type_>::value, void>::Result
Swap(Type_ &lhs, Type_ &rhs) {
    Type_ temp(lhs);
    lhs = rhs;
    rhs = temp;
}

template <typename Type_, size_t Size_>
inline typename etl::IfTrue<etl::IsArray<Type_>::value, void>::Result
Swap(Type_ lhs[Size_], Type_ rhs[Size_]) {
    for (size_t i(0); i != Size_; ++i)
        ext::Swap(lhs[i], rhs[i]);
}

template <typename Type_>
inline void Rotate(Type_ &arg0, Type_ &arg1) {
    Type_ temp(arg0);
    arg0 = arg1;
    arg1 = temp;
}

template <typename Type_>
inline void Rotate(Type_ &arg0, Type_ &arg1, Type_ &arg2) {
    Type_ temp(arg0);
    arg0 = arg1;
    arg1 = arg2;
    arg2 = temp;
}

template <typename Type_>
inline void Rotate(Type_ &arg0, Type_ &arg1, Type_ &arg2, Type_ &arg3) {
    Type_ temp(arg0);
    arg0 = arg1;
    arg1 = arg2;
    arg2 = arg3;
    arg3 = temp;
}

template <typename Type_, typename Value_>
_finline Type_ GetAndSet(Type_ &lhs, Value_ rhs) {
    Type_ temp(lhs);
    lhs = rhs;
    return temp;
}

}

#endif//MENES_EXT_ALGORITHM_HPP
