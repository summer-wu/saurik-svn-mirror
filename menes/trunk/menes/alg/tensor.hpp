/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_ALG_TENSOR_HPP
#define MENES_ALG_TENSOR_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/integral.hpp"
#include "etl/template.hpp"
#include "etl/list.hpp"

namespace alg {

template <typename Ranks_, typename Element_>
class Tensor :
    public Tensor<etl::Cons<etl::Integer<unsigned, Ranks_::Head::value - 1>, Ranks_>, Element_>
{
  private:
    typedef Tensor<Ranks_, Element_> This_;
    typedef Tensor<etl::Cons<etl::Integer<unsigned, Ranks_::Head::value - 1>, Ranks_>, Element_> Base_;

  private:
    typename Base_::Value value_;

  public:
    _finline Tensor() {
    }

    _finline Tensor(const Base_ &base, const typename Base_::Value &value = typename Base_::Value()) :
        Base_(base),
        value_(value)
    {
    }

    _finline const Base_ &GetBase() const {
        return *this;
    }

    _finline const typename Base_::Value &GetValue() const {
        return value_;
    }

    _finline typename Base_::Value &GetValue() {
        return value_;
    }
};

template <typename Ranks_, typename Element_>
class Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> {
  private:
    typedef Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> This_;

  public:
    typedef Tensor<typename Ranks_::Tail, Element_> Value;

    static const unsigned Rank = etl::Length<Ranks_>::value;

    const Value &operator [](size_t index) const {
        return reinterpret_cast<const Value *>(this)[index];
    }

    Value &operator [](size_t index) {
        return reinterpret_cast<Value *>(this)[index];
    }
};

template <typename Element_>
class Tensor<etl::Null, Element_> {
  private:
    typedef Tensor<etl::Null, Element_> This_;

  public:
    typedef Element_ Value;

    static const unsigned Rank = 0;

  private:
    Value value_;

  public:
    Tensor() {
    }

    Tensor(const Value &value) :
        value_(value)
    {
    }

    _finline const Value &GetValue() const {
        return value_;
    }

    _finline Value &GetValue() {
        return value_;
    }

    operator const Value &() const {
        return value_;
    }

    operator Value &() {
        return value_;
    }
};

template <typename Element_>
class Tensor<etl::Cons< etl::Integer<unsigned, 1> >, Element_> :
    public Tensor<etl::Cons< etl::Integer<unsigned, 0> >, Element_>
{
  private:
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 1> >, Element_> This_;
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 0> >, Element_> Base_;

  public:
    typename Base_::Value x;

  public:
    _finline Tensor(const Base_ &base, const typename Base_::Value &value) :
        Base_(base),
        x(value)
    {
    }

    _finline Tensor(
        const typename Base_::Value &x = typename Base_::Value()
    ) :
        x(x)
    {
    }

    _finline const Base_ &GetBase() const {
        return *this;
    }

    _finline const typename Base_::Value &GetValue() const {
        return x;
    }

    _finline typename Base_::Value &GetValue() {
        return x;
    }
};

template <typename Element_>
class Tensor<etl::Cons< etl::Integer<unsigned, 2> >, Element_> :
    public Tensor<etl::Cons< etl::Integer<unsigned, 1> >, Element_>
{
  private:
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 2> >, Element_> This_;
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 1> >, Element_> Base_;

  public:
    typedef typename Base_::Value Value;
    Value y;

  public:
    _finline Tensor(const Base_ &base, const typename Base_::Value &value) :
        Base_(base),
        y(value)
    {
    }

    _finline Tensor(
        const Value &x = Value(),
        const Value &y = Value()
    ) :
        Base_(x),
        y(y)
    {
    }

    _finline const Base_ &GetBase() const {
        return *this;
    }

    _finline const typename Base_::Value &GetValue() const {
        return y;
    }

    _finline typename Base_::Value &GetValue() {
        return y;
    }
};

template <typename Element_>
class Tensor<etl::Cons< etl::Integer<unsigned, 3> >, Element_> :
    public Tensor<etl::Cons< etl::Integer<unsigned, 2> >, Element_>
{
  private:
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 3> >, Element_> This_;
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 2> >, Element_> Base_;

  public:
    typename Base_::Value z;

  public:
    _finline Tensor(const Base_ &base, const typename Base_::Value &value) :
        Base_(base),
        z(value)
    {
    }

    _finline Tensor(
        const typename Base_::Value &x = typename Base_::Value(),
        const typename Base_::Value &y = typename Base_::Value(),
        const typename Base_::Value &z = typename Base_::Value()
    ) :
        Base_(x, y),
        z(z)
    {
    }

    _finline const Base_ &GetBase() const {
        return *this;
    }

    _finline const typename Base_::Value &GetValue() const {
        return z;
    }

    _finline typename Base_::Value &GetValue() {
        return z;
    }
};


template <typename Element_>
class Tensor<etl::Cons< etl::Integer<unsigned, 4> >, Element_> :
    public Tensor<etl::Cons< etl::Integer<unsigned, 3> >, Element_>
{
  private:
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 4> >, Element_> This_;
    typedef Tensor<etl::Cons< etl::Integer<unsigned, 3> >, Element_> Base_;

  public:
    typedef typename Base_::Value Value;
    Value w;

  public:
    _finline Tensor(const Base_ &base, const typename Base_::Value &value) :
        Base_(base),
        w(value)
    {
    }

    _finline Tensor(
        const Value &x = Value(),
        const Value &y = Value(),
        const Value &z = Value(),
        const Value &w = Value()
    ) :
        Base_(x, y, z),
        w(w)
    {
    }

    _finline const Base_ &GetBase() {
        return *this;
    }

    _finline const typename Base_::Value &GetValue() const {
        return w;
    }

    _finline typename Base_::Value &GetValue() {
        return w;
    }
};

/*template <typename Element_, typename Scalar_>
Tensor<etl::Null, Element_> operator +(const Tensor<etl::Null, Element_> &lhs, const Scalar_ &rhs) {
    return lhs.GetValue() + rhs;
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> operator +(const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_>();
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<Ranks_, Element_> operator +(const Tensor<Ranks_, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<Ranks_, Element_>(lhs.GetBase() + rhs, lhs.GetValue() + rhs);
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<Ranks_, Element_> operator +(const Scalar_ &lhs, const Tensor<Ranks_, Element_> &rhs) {
    return rhs + lhs;
}

template <typename Element_, typename Scalar_>
Tensor<etl::Null, Element_> operator -(const Tensor<etl::Null, Element_> &lhs, const Scalar_ &rhs) {
    return lhs.GetValue() - rhs;
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> operator -(const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_>();
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<Ranks_, Element_> operator -(const Tensor<Ranks_, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<Ranks_, Element_>(lhs.GetBase() - rhs, lhs.GetValue() - rhs);
}*/

/*template <typename Ranks_, typename Element_>
Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> operator +(
    const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &lhs,
    const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &rhs
) {
    return Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_>();
}

template <typename Ranks_, typename Element_>
Tensor<Ranks_, Element_> operator +(
    const Tensor<Ranks_, Element_> &lhs,
    const Tensor<Ranks_, Element_> &rhs
) {
    return Tensor<Ranks_, Element_>(lhs.GetBase() + rhs.GetBase(), lhs.GetValue() + rhs.GetValue());
}


template <typename Ranks_, typename Element_>
Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> operator -(const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &lhs, const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &rhs) {
    return Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_>();
}

template <typename Ranks_, typename Element_>
Tensor<Ranks_, Element_> operator -(const Tensor<Ranks_, Element_> &lhs, const Tensor<Ranks_, Element_> &rhs) {
    return Tensor<Ranks_, Element_>(lhs.GetBase() - rhs.GetBase(), lhs.GetValue() - rhs.GetValue());
}


template <typename Element_, typename Scalar_>
Tensor<etl::Null, Element_> operator *(const Tensor<etl::Null, Element_> &lhs, const Scalar_ &rhs) {
    return lhs.GetValue() * rhs;
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> operator *(const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_>();
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<Ranks_, Element_> operator *(const Tensor<Ranks_, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<Ranks_, Element_>(lhs.GetBase() * rhs, lhs.GetValue() * rhs);
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<Ranks_, Element_> operator *(const Scalar_ &lhs, const Tensor<Ranks_, Element_> &rhs) {
    return rhs * lhs;
}


template <typename Element_, typename Scalar_>
Tensor<etl::Null, Element_> operator /(const Tensor<etl::Null, Element_> &lhs, const Scalar_ &rhs) {
    return lhs.GetValue() / rhs;
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> operator /(const Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<etl::Cons<etl::Integer<unsigned, 0>, Ranks_>, Element_>();
}

template <typename Ranks_, typename Element_, typename Scalar_>
Tensor<Ranks_, Element_> operator /(const Tensor<Ranks_, Element_> &lhs, const Scalar_ &rhs) {
    return Tensor<Ranks_, Element_>(lhs.GetBase() / rhs, lhs.GetValue() / rhs);
}


template <unsigned Dimensions_, typename Element_>
Tensor<etl::Cons< etl::Integer<unsigned, etl::Combinations<unsigned, Dimensions_, 2>::value> >, Element_>
CrossProduct(
    const Tensor<etl::Cons< etl::Integer<unsigned, Dimensions_> >, Element_> &lhs,
    const Tensor<etl::Cons< etl::Integer<unsigned, Dimensions_> >, Element_> &rhs
) {
    return;
}

template <unsigned Dimensions_, typename Element_>
Tensor<etl::Cons< etl::Integer<unsigned, Dimensions_> >, Element_> CrossProduct(const Tensor<etl::Cons< etl::Integer<unsigned, Dimensions_> >, Element_> values[Dimensions_ - 1]) {
}*/


template <unsigned Dimension_, typename Element_>
struct Vector {
    typedef Tensor<etl::Cons< etl::Integer<unsigned, Dimension_> >, Element_> Result;
};

template <unsigned Rows_, unsigned Columns_, typename Element_>
struct Matrix {
    typedef Tensor<etl::Cons< etl::Integer<unsigned, Rows_>, etl::Cons< etl::Integer<unsigned, Columns_> > >, Element_> Result;
};

typedef Vector<1, float>::Result Vector1f;
typedef Vector<1, double>::Result Vector1d;

typedef Vector<2, float>::Result Vector2f;
typedef Vector<2, double>::Result Vector2d;

typedef Vector<3, float>::Result Vector3f;
typedef Vector<3, double>::Result Vector3d;

typedef Vector<4, float>::Result Vector4f;
typedef Vector<4, double>::Result Vector4d;

typedef Matrix<2, 2, float>::Result Matrix22f;
typedef Matrix<2, 2, double>::Result Matrix22d;

typedef Matrix<3, 3, float>::Result Matrix33f;
typedef Matrix<3, 3, double>::Result Matrix33d;

typedef Matrix<4, 4, float>::Result Matrix44f;
typedef Matrix<4, 4, double>::Result Matrix44d;

typedef Matrix<3, 4, float>::Result Matrix34f;
typedef Matrix<3, 4, double>::Result Matrix34d;

}

#endif//MENES_ALG_TENSOR_HPP
