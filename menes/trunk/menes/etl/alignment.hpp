/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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

#ifndef MENES_ETL_ALIGNMENT_HPP
#define MENES_ETL_ALIGNMENT_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/integral.hpp"
#include "etl/template.hpp"
#include "etl/bind.hpp"
#include "etl/list.hpp"
#include "etl/manip.hpp"

namespace etl {

namespace be {
class Undefined;
}

typedef Cons<char, Cons<wchar_t,
        Cons<short int, Cons<int, Cons<long int,
        Cons<float, Cons<double, Cons<long double,
        Cons<char *, Cons<wchar_t *,
        Cons<short int *, Cons<int *, Cons<long int *,
        Cons<float *, Cons<double *, Cons<long double *,
        Cons<void *,
        Cons<be::Undefined (*)(be::Undefined),
        Cons<be::Undefined *be::Undefined::*,
        Cons<be::Undefined (be::Undefined::*)(be::Undefined)
        > > > >  > > >  > > >  > >  > > >  > > >  > > Intrinsics;

typedef Append<Intrinsics, ForEach<Intrinsics, Bind1<StructOf>::Template>::Result>::Result IntrinsicAlignment;

template <size_t Limit_, typename Cell_>
struct LimitSize {
  private:
    static const size_t limit = (MaxSize<Cell_>::value == 0 ? 0 : Limit_);
    typedef typename LimitSize<limit, typename Cell_::Tail>::Result Tail;

  public:
    typedef typename IfElse<
        MaxSize<Cell_>::value == 0,
        Null,
        typename IfElse<
            (sizeof(typename Cell_::Head) > Limit_),
            Tail,
            Cons<typename Cell_::Head, Tail>
        >::Result
    >::Result Result;
};

template <>
struct LimitSize<0, Null> {
    typedef Null Result;
};

template <typename Cell_>
struct PlainOldAlignment {
    typedef Union<typename LimitSize<MaxSize<Cell_>::value, IntrinsicAlignment>::Result> Result;
};

template <typename Type_>
_finline Type_ AlignValue(Type_ pointer, size_t alignment) {
    size_t check(alignment - 1);
    return static_cast<Type_>((check + static_cast<uintptr_t>(pointer)) & ~check);
}

template <typename Type_>
_finline Type_ *AlignPointer(Type_ *pointer, size_t alignment = sizeof(Type_)) {
    size_t check(alignment - 1);
    return reinterpret_cast<Type_ *>((check + reinterpret_cast<uintptr_t>(pointer)) & ~check);
}

}

#endif//MENES_ETL_ALIGNMENT_HPP
