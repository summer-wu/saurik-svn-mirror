/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *·
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
 *·
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

#ifndef MENES_ETL_COMPOSITE_HPP
#define MENES_ETL_COMPOSITE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/integral.hpp"
#include "etl/list.hpp"
#include "etl/traits.hpp"

namespace etl {

namespace be {

template <typename Cell_>
class CompositeType :
    public CompositeType<typename Cell_::Tail>
{
  private:
    typedef CompositeType<Cell_> This_;
    typedef  CompositeType<typename Cell_::Tail> Base_;

    static const size_t Index_ = Length<Cell_>::value;

  protected:
    typename Cell_::Head value_;

  protected:
    CompositeType() {
    }

    template <typename Arg0_>
    CompositeType(const Arg0_ &arg0) :
        value_(arg0)
    {
    }

    template <typename Arg0_, typename Arg1_>
    CompositeType(const Arg0_ &arg0, const Arg1_ &arg1) :
        Base_(arg0),
        value_(arg1)
    {
    }

    _finline bool operator <(const This_ &rhs) const {
        return value_ == rhs.value_ ? Base_::operator <(rhs) : value_ < rhs.value_;
    }

    _finline bool operator ==(const This_ &rhs) const {
        return value_ == rhs.value_ && Base_::operator ==(rhs);
    }
};

template <>
class CompositeType<Null> {
  private:
    typedef CompositeType<Null> This_;

  public:
    _finline bool operator <(const This_ &rhs) const {
        return false;
    }

    _finline bool operator ==(const This_ &rhs) const {
        return true;
    }
};

}

template <typename Cell_>
class Composite :
    public be::CompositeType<typename Reverse<Cell_>::Result>
{
  private:
    typedef Composite<Cell_> This_;
    typedef typename Reverse<Cell_>::Result Order_;
    typedef be::CompositeType<Order_> Base_;

  public:
    typedef typename Cell_::Head Head;

  public:
    Composite() {
    }

    template <typename Arg0_>
    Composite(const Arg0_ &arg0) :
        Base_(arg0)
    {
    }

    template <typename Arg0_, typename Arg1_>
    Composite(const Arg0_ &arg0, const Arg1_ &arg1) :
        Base_(arg0, arg1)
    {
    }

    template <size_t Index_>
    _finline const typename At<Cell_, Index_>::Result &Get() const {
        return be::CompositeType<typename Tail<Order_, Index_ + 1>::Result>::value_;
    }

    template <size_t Index_>
    _finline typename At<Cell_, Index_>::Result &Get() {
        return be::CompositeType<typename Tail<Order_, Index_ + 1>::Result>::value_;
    }

    template <size_t Index_>
    void Set(const typename At<Cell_, Index_>::Result &value) {
        be::CompositeType<typename Tail<Order_, Index_ + 1>::Result>::value_ = value;
    }

    _finline const typename At<Cell_, 0>::Result &First() const {
        return be::CompositeType<typename Tail<Order_, 1>::Result>::value_;
    }

    _finline typename At<Cell_, 0>::Result &First() {
        return be::CompositeType<typename Tail<Order_, 1>::Result>::value_;
    }

    _finline const typename At<Cell_, 1>::Result &Second() const {
        return be::CompositeType<typename Tail<Order_, 2>::Result>::value_;
    }

    _finline typename At<Cell_, 1>::Result &Second() {
        return be::CompositeType<typename Tail<Order_, 2>::Result>::value_;
    }

    bool operator <(const This_ &rhs) const {
        return Base_::operator <(rhs);
    }

    bool operator ==(const This_ &rhs) const {
        return Base_::operator ==(rhs);
    }
};

template <typename Cell_>
struct IsMemMovable< etl::be::CompositeType<Cell_> > {
    static const bool value = IsMemMovable<typename Cell_::Head>::value && IsMemMovable< etl::be::CompositeType<typename Cell_::Tail> >::value;
};

template <>
struct IsMemMovable< etl::be::CompositeType<Null> > {
    static const bool value = true;
};

template <typename Cell_>
struct IsMemMovable< etl::Composite<Cell_> > {
    static const bool value = IsMemMovable< etl::be::CompositeType<typename etl::Reverse<Cell_>::Result> >::value;
};

template <typename First_, typename Second_>
class Pair :
    public etl::Composite<typename etl::List<First_, Second_>::Result>
{
  private:
    typedef Pair<First_, Second_> This_;
    typedef etl::Composite<typename etl::List<First_, Second_>::Result> Base_;

  public:
    Pair() {
    }

    Pair(const This_ &rhs) :
        Base_(rhs.First(), rhs.Second())
    {
    }

    Pair(const First_ &first) :
        Base_(first)
    {
    }

    Pair(const First_ &first, const Second_ &second) :
        Base_(first, second)
    {
    }

    template <typename Arg0_, typename Arg1_>
    Pair(const Pair<Arg0_, Arg1_> &rhs) :
        Base_(rhs.First(), rhs.Second())
    {
    }
};

template <typename First_, typename Second_>
struct IsMemMovable< etl::Pair<First_, Second_> > {
    static const bool value = IsMemMovable< etl::Composite<typename etl::List<First_, Second_>::Result> >::value;
};

template <
    typename Arg0_ = be::ListNull_,
    typename Arg1_ = be::ListNull_,
    typename Arg2_ = be::ListNull_,
    typename Arg3_ = be::ListNull_,
    typename Arg4_ = be::ListNull_,
    typename Arg5_ = be::ListNull_,
    typename Arg6_ = be::ListNull_
>
struct Tuple {
    typedef Composite<typename List<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_>::Result> Result;
};

}

#endif//MENES_ETL_COMPOSITE_HPP
