/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#ifndef MENES_ETL_TYPELIST_HPP
#define MENES_ETL_TYPELIST_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/integral.hpp"
#include "etl/template.hpp"

namespace etl {

struct Null {
  private:
    Null();

  public:
    typedef Null Head;
    typedef Null Tail;
};

template <typename Head_, typename Tail_ = Null>
struct Cons {
    typedef Head_ Head;
    typedef Tail_ Tail;
};

namespace be {
    struct ListNull_;
}

template <
    typename Arg0_ = be::ListNull_,
    typename Arg1_ = be::ListNull_,
    typename Arg2_ = be::ListNull_,
    typename Arg3_ = be::ListNull_,
    typename Arg4_ = be::ListNull_,
    typename Arg5_ = be::ListNull_,
    typename Arg6_ = be::ListNull_,
    typename Arg7_ = be::ListNull_
>
struct List {
    typedef Cons<Arg0_, typename List<Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_, Arg7_>::Result> Result;
};

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_>
struct List<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_> {
    typedef Cons<Arg0_, typename List<Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_>::Result> Result;
};

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_>
struct List<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_> {
    typedef Cons<Arg0_, typename List<Arg1_, Arg2_, Arg3_, Arg4_, Arg5_>::Result> Result;
};

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_>
struct List<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_> {
    typedef Cons<Arg0_, typename List<Arg1_, Arg2_, Arg3_, Arg4_>::Result> Result;
};

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_>
struct List<Arg0_, Arg1_, Arg2_, Arg3_> {
    typedef Cons<Arg0_, typename List<Arg1_, Arg2_, Arg3_>::Result> Result;
};

template <typename Arg0_, typename Arg1_, typename Arg2_>
struct List<Arg0_, Arg1_, Arg2_> {
    typedef Cons<Arg0_, typename List<Arg1_, Arg2_>::Result> Result;
};

template <typename Arg0_, typename Arg1_>
struct List<Arg0_, Arg1_> {
    typedef Cons<Arg0_, typename List<Arg1_>::Result> Result;
};

template <typename Arg0_>
struct List<Arg0_> {
    typedef Cons<Arg0_> Result;
};

template <>
struct List<be::ListNull_, be::ListNull_, be::ListNull_, be::ListNull_, be::ListNull_, be::ListNull_, be::ListNull_> {
    typedef Null Result;
};

template <typename Left_, typename Right_>
struct Append {
    typedef Cons<typename Left_::Head, typename Append<typename Left_::Tail, Right_>::Result> Result;
};

template <typename Right_>
struct Append<Null, Right_> {
    typedef Right_ Result;
};

template <typename Cell_>
struct MaxSize {
  private:
    static const unsigned head_ = sizeof(typename Cell_::Head);
    static const unsigned tail_ = MaxSize<typename Cell_::Tail>::value;
  public:
    static const unsigned value = IfElse< (head_ > tail_), Integer<unsigned, head_>, Integer<unsigned, tail_> >::Result::value;
};

template <>
struct MaxSize<Null> {
    static const unsigned value = 0;
};

template <typename Cell_>
struct Length {
    static const unsigned value = Length<typename Cell_::Tail>::value + 1;
};

template <>
struct Length<Null> {
    static const unsigned value = 0;
};

template <typename Cell_>
union Union {
    typename Cell_::Head head;
    Union<typename Cell_::Tail> tail;
};

template <typename Type_>
union Union< Cons<Type_> > {
    Type_ head;
};

template <>
union Union<Null> {
};

template <typename Cell_>
struct Struct {
    typename Cell_::Head head;
    Struct<typename Cell_::Tail> tail;
};

template <typename Type_>
struct Struct< Cons<Type_> > {
    Type_ head;
};

template <>
struct Struct<Null> {
};

template <typename Cell_, template <typename> class Operation_>
struct ForEach {
    typedef Cons<typename Operation_<typename Cell_::Head>::Result, typename ForEach<typename Cell_::Tail, Operation_>::Result> Result;
};

template <template <typename> class Operation>
struct ForEach<Null, Operation> {
    typedef Null Result;
};

template <typename Cell_, unsigned Start_>
struct Skip {
    typedef typename Skip<typename Cell_::Tail, Start_ - 1>::Result Result;
};

template <typename Cell_>
struct Skip<Cell_, 0> {
    typedef Cell_ Result;
};

template <typename Cell_, unsigned Start_>
struct Tail {
    typedef typename Skip<Cell_, Length<Cell_>::value - Start_>::Result Result;
};

template <typename Cell_, unsigned Length_>
struct Head {
    typedef Cons<typename Cell_::Head, typename Head<typename Cell_::Tail, Length_ - 1>::Result> Result;
};

template <typename Cell_>
struct Head<Cell_, 0> {
    typedef Null Result;
};

template <typename Cell_, unsigned Start_>
struct Clip {
    typedef typename Head<Cell_, Length<Cell_>::value - Start_>::Result Result;
};

template <typename Cell_, unsigned Start_, unsigned End_ = Length<Cell_>::value>
struct Range {
    typedef Range<typename Cell_::Tail, Start_ - 1, End_ - 1> Result;
};

template <typename Cell_, unsigned End_>
struct Range<Cell_, 0, End_> {
    typedef Cons<typename Cell_::Head, typename Range<typename Cell_::Tail, 0, End_ - 1>::Result> Result;
};

template <typename Cell_>
struct Range<Cell_, 0, 0> {
    typedef Null Result;
};

template <typename Cell_, size_t Index_>
struct At {
    typedef typename Skip<Cell_, Index_>::Result::Head Result;
};

template <typename Cell_, typename Type_, unsigned Start_ = 0>
struct IndexOf {
    static const unsigned value = IndexOf<Skip<Cell_, Start_>, Type_, 0>::value;
};

template <typename Cell_, typename Type_>
struct IndexOf<Cell_, Type_, 0> {
  private:
    static const unsigned temp = IndexOf<typename Cell_::Tail, Type_>::value;

  public:
    static const unsigned value = (temp == ~unsigned() ? ~unsigned() : temp + 1);
};

template <typename Type_, typename Tail_>
struct IndexOf<Cons<Type_, Tail_>, Type_, 0> {
    static const unsigned value = 0;
};

template <typename Type_>
struct IndexOf<Null, Type_, 0> {
    static const unsigned value = ~unsigned();
};

template <typename Cell_, unsigned Index_>
struct Erase {
    typedef Cons<
        typename Cell_::Head,
        typename Erase<typename Cell_::Tail, Index_ - 1>::Result
    > Result;
};

template <typename Cell_>
struct Erase<Cell_, 0> {
    typedef typename Cell_::Tail Result;
};

template <typename Cell_>
struct Reverse {
    typedef typename Append<
        typename Reverse<typename Cell_::Tail>::Result,
        Cons<typename Cell_::Head>
    >::Result Result;
};

template <typename Type_>
struct Reverse< Cons<Type_, Null> > {
    typedef Cons<Type_, Null> Result;
};

template <>
struct Reverse<Null> {
    typedef Null Result;
};

template <typename Cell_, size_t Number_>
struct Multiply {
    typedef typename Append<
        typename Multiply<Cell_, Number_ - 1>::Result,
        Cell_
    >::Result Result;
};

template <typename Cell_>
struct Multiply<Cell_, 0> {
    typedef Null Result;
};

namespace be {
template <typename Left_>
struct Cross_ {
    template <typename Right_>
    struct Template {
        typedef Cons< Left_, Cons<Right_, Null> > Result;
    };
}; }

template <typename Left_, typename Right_>
struct Cross {
    typedef typename Append<
        typename ForEach<
            Right_,
            be::Cross_<typename Left_::Head>::template Template
        >::Result,
        typename Cross<typename Left_::Tail, Right_>::Result
    >::Result Result;
};

template <typename Right_>
struct Cross<Null, Right_> {
    typedef Null Result;
};

}

#endif//MENES_ETL_TYPELIST_HPP
