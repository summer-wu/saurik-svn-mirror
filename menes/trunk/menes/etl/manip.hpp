/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#ifndef MENES_ETL_TYPEMANIP_HPP
#define MENES_ETL_TYPEMANIP_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace etl {

template <typename Type_>
class Identity {
    typedef Type_ Result;
};

template <typename Member_>
class ClassOf {
    Member_ member;
};

template <typename Member_>
struct StructOf {
    Member_ member;
};

template <typename Member_>
union UnionOf {
    Member_ member;
};

template <typename Type_>
struct PointerTo {
    typedef Type_ *Result;
};

template <typename Type_>
struct ReferenceTo {
    typedef Type_ &Result;
};

template <typename Type_>
struct ReferenceTo<Type_ &> {
    typedef Type_ &Result;
};

template <typename Type_>
struct StripConst {
    typedef Type_ Result;
};

template <typename Type_>
struct StripConst<const Type_> {
    typedef Type_ Result;
};

template <typename Type_>
struct StripVolatile {
    typedef Type_ Result;
};

template <typename Type_>
struct StripVolatile<volatile Type_> {
    typedef Type_ Result;
};

template <typename Type_>
struct StripPointer {
    typedef Type_ Result;
};

template <typename Type_>
struct StripPointer<Type_ *> {
    typedef Type_ Result;
};

template <typename Type_>
struct StripReference {
    typedef Type_ Result;
};

template <typename Type_>
struct StripReference<Type_ &> {
    typedef Type_ Result;
};

template <typename Type_>
struct StripArray {
    typedef Type_ Result;
};

template <typename Type_, size_t Size_>
struct StripArray<Type_ [Size_]> {
    typedef Type_ Result;
};

}

#endif//MENES_ETL_TYPEMANIP_HPP
