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

#ifndef MENES_EXT_WORKAROUNDS_HPP
#define MENES_EXT_WORKAROUNDS_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#define COMMA ,

#define EXT_EXPLICITCONSTRUCT_HPP(Type) \
  public: \
    Type(); \
    ~Type();

#define EXT_EXPLICITCONSTRUCT_CPP(Type) \
    Type::Type() { \
    } \
\
    Type::~Type() { \
    }

#define EXT_EXPLICITCONSTRUCT(Type) \
  public: \
    Type() { \
    } \
\
    ~Type() { \
    }

#define EXT_NOIMPLICITCOPY_HPP(Type) \
  private: \
    Type(const Type &rhs); \
    Type &operator =(const Type &rhs);

#define EXT_EXPLICITCOPY_HPP(Type) \
  public: \
    Type(const Type &rhs); \
    Type &operator =(const Type &rhs);

#define EXT_NOIMPLICITCOPY_CPP(Type) \
    Type::Type(const Type &rhs) { \
        _assert(false); \
    } \
\
    Type &Type::operator =(const Type &rhs) { \
        _assert(false); \
        return *this; \
    }

#define EXT_NOIMPLICITCOPY(Type) \
  private: \
    Type(const Type &rhs) { \
        _assert(false); \
    } \
\
    Type &operator =(const Type &rhs) { \
        _assert(false); \
        return *this; \
    }

#define EXT_TYPEDEFTHIS(Type) \
  private: \
    typedef Type This_;

#define EXT_PASSTHRU_CONSTRUCT(Type, Base) \
    Type() { \
        _initialize(); \
    } \
\
    template <typename Arg0_> \
    Type(const Arg0_ &arg0) : \
        Base(arg0) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_> \
    Type(Arg0_ &arg0) : \
        Base(arg0) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_> \
    Type(const Arg0_ &arg0, const Arg1_ &arg1) : \
        Base(arg0, arg1) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_> \
    Type(const Arg0_ &arg0, Arg1_ &arg1) : \
        Base(arg0, arg1) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_> \
    Type(Arg0_ &arg0, const Arg1_ &arg1) : \
        Base(arg0, arg1) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_> \
    Type(Arg0_ &arg0, Arg1_ &arg1) : \
        Base(arg0, arg1) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_, typename Arg2_> \
    Type(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2) : \
        Base(arg0, arg1, arg2) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_, typename Arg2_> \
    Type(Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2) : \
        Base(arg0, arg1, arg2) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_> \
    Type(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3) : \
        Base(arg0, arg1, arg2, arg3) \
    { \
        _initialize(); \
    } \
\
    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_> \
    Type(Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3) : \
        Base(arg0, arg1, arg2, arg3) \
    { \
        _initialize(); \
    }

template <typename Type_>
_finline Type_ &_unconst(const Type_ &value) {
    return const_cast<Type_ &>(value);
}

#endif//MENES_EXT_WORKAROUNDS_HPP
