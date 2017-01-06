/* Skiazo - Menes-Compatible OpenGL Graphics Engine
 * Copyright (C) 2006  Jay Freeman (saurik)
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

#ifndef MENES_ALG_VECTOR_HPP
#define MENES_ALG_VECTOR_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "alg/constants.hpp"
#include "alg/exponents.hpp"

namespace alg {

template <
    typename Element_,
    size_t Dimensions_,
    typename Sub_
> class Vector {
  private:
    Element_ data_[Dimensions_];

  public:
    _finline Element_ GetWeight() const {
        return static_cast<const Sub_ *>(this)->GetWeight();
    }

    _finline void SetWeight(const Element_ &weight) {
        return static_cast<const Sub_ *>(this)->SetWeight(weight);
    }

    _finline const Element_ *GetData() const {
        return data_;
    }

    _finline Element_ *GetData() {
        return data_;
    }

    _finline const Element_ &operator [](size_t element) const {
        _assert(element < Dimensions_);
        return data_[element];
    }

    _finline Element_ &operator [](size_t element) {
        _assert(element < Dimensions_);
        return data_[element];
    }
};

/* class ShortVector {{{ */
template <
    typename Element_,
    size_t Dimensions_,
    bool Bound_,
    typename Sub_
> class ShortVector_ :
    public Vector<
        Element_,
        Dimensions_,
        Sub_
    >
{
    typedef ShortVector_<Element_, Dimensions_, Bound_, Sub_> This_;
    typedef Vector<Element_, Dimensions_, Sub_> Base_;

  public:
    _finline void Set(const Element_ &rhs) {
        _forall (size_t, element, 0, Dimensions_)
            Base_::operator [](element) = rhs;
    }

    _finline void SetWeight(const Element_ &weight) const {
        _assert(weight == GetWeight());
    }

    _finline Element_ GetWeight() const {
        return Bound_ ? One<Element_>() : Zero<Element_>();
    }
};

template <typename Element_, size_t Dimensions_, bool Bound_>
class ShortVector :
    public ShortVector_<Element_, Dimensions_, Bound_,
        ShortVector<Element_, Dimensions_, Bound_>
    >
{
    typedef ShortVector<Element_, Dimensions_, Bound_> This_;
    typedef ShortVector_<Element_, Dimensions_, Bound_, This_> Base_;

  public:
    _finline ShortVector() {
    }

    _finline explicit ShortVector(const Element_ &rhs) {
        Set(rhs);
    }

    using Base_::Set;
};

template <typename Element_, bool Bound_>
class ShortVector<Element_, 2, Bound_> :
    public ShortVector_<Element_, 2, Bound_,
        ShortVector<Element_, 2, Bound_>
    >
{
    typedef ShortVector<Element_, 2, Bound_> This_;
    typedef ShortVector_<Element_, 2, Bound_, This_> Base_;

  public:
    _finline ShortVector() {
    }

    _finline explicit ShortVector(const Element_ &rhs) {
        Set(rhs);
    }

    _finline ShortVector(const Element_ &x, const Element_ &y) {
        Set(x, y);
    }

    using Base_::Set;

    _finline void Set(const Element_ &x, const Element_ &y) {
        Base_::operator [](0) = x;
        Base_::operator [](1) = y;
    }
};

template <typename Element_, bool Bound_>
class ShortVector<Element_, 3, Bound_> :
    public ShortVector_<Element_, 3, Bound_,
        ShortVector<Element_, 3, Bound_>
    >
{
    typedef ShortVector<Element_, 3, Bound_> This_;
    typedef ShortVector_<Element_, 3, Bound_, This_> Base_;

  public:
    _finline ShortVector() {
    }

    _finline explicit ShortVector(const Element_ &rhs) {
        Set(rhs);
    }

    _finline ShortVector(const Element_ &x, const Element_ &y, const Element_ &z) {
        Set(x, y, z);
    }

    using Base_::Set;

    _finline void Set(const Element_ &x, const Element_ &y, const Element_ &z) {
        Base_::operator [](0) = x;
        Base_::operator [](1) = y;
        Base_::operator [](2) = z;
    }
};
/* }}} */
/* class LongVector {{{ */
template <
    typename Element_,
    size_t Dimensions_
> class LongVector_ :
    public Vector<
        Element_,
        Dimensions_,
        LongVector_<
            Element_,
            Dimensions_
        >
    >
{
    typedef LongVector_<Element_, Dimensions_> This_;
    typedef Vector<Element_, Dimensions_, This_> Base_;

  private:
    Element_ weight_;

  public:
    _finline void Set(const Element_ &rhs, const Element_ &w) {
        _forall (size_t, element, 0, Dimensions_)
            Base_::operator [](element) = rhs;
        weight_ = w;
    }

    _finline void SetWeight(const Element_ &weight) const {
        weight_ = weight;
    }

    _finline const Element_ &GetWeight() const {
        return weight_;
    }
};

template <typename Element_, size_t Dimensions_>
class LongVector : public LongVector_<Element_, Dimensions_> {
    typedef LongVector_<Element_, Dimensions_> Base_;

  public:
    _finline LongVector() {
    }

    _finline explicit LongVector(const Element_ &rhs, const Element_ &w) {
        Set(rhs, w);
    }
};

template <typename Element_>
class LongVector<Element_, 2> : public LongVector_<Element_, 2> {
    typedef LongVector_<Element_, 2> Base_;

  public:
    _finline LongVector() {
    }

    _finline explicit LongVector(const Element_ &rhs, const Element_ &w) {
        Set(rhs, w);
    }

    _finline LongVector(const Element_ &x, const Element_ &y, const Element_ &w) {
        Set(x, y, w);
    }

    using Base_::Set;

    _finline void Set(const Element_ &x, const Element_ &y, const Element_ &w) {
        Base_::operator [](0) = x;
        Base_::operator [](1) = y;
        Base_::operator [](2) = w;
    }
};

template <typename Element_>
class LongVector<Element_, 3> : public LongVector_<Element_, 3> {
    typedef LongVector_<Element_, 3> Base_;

  public:
    _finline LongVector() {
    }

    _finline LongVector(const Element_ &x, const Element_ &y, const Element_ &z, const Element_ &w) {
        Set(x, y, z, w);
    }

    using Base_::Set;

    _finline void Set(const Element_ &x, const Element_ &y, const Element_ &z, const Element_ &w) {
        Base_::operator [](0) = x;
        Base_::operator [](1) = y;
        Base_::operator [](2) = z;
        Base_::operator [](3) = w;
    }
};
/* }}} */

template <typename Element_, unsigned Dimensions_, bool Bound_>
class PaddedVector :
    public ShortVector<Element_, Dimensions_, Bound_>
{
    typedef ShortVector<Element_, Dimensions_, Bound_> Base_;

  private:
    Element_ weight_;

  public:
    PaddedVector() :
        weight_(Base_::GetWeight())
    {
    }
};

/* Additive Operations {{{ */
#define VectorAdditive_(op, Value, Left, Right) \
    _finline Value operator op(const Left &lhs, const Right &rhs) { \
        Value value; \
        _forall (size_t, element, 0, Dimensions_) \
            value[element] = lhs[element] op rhs[element]; \
        value.SetWeight(lhs.GetWeight() op rhs.GetWeight()); \
        return value; \
    }

#define LongVectorAdditive_(op) \
    template < \
        typename Element_, \
        size_t Dimensions_, \
        typename LeftSub_, \
        typename RightSub_ \
    > VectorAdditive_(op, \
        LongVector<Element_ _comma Dimensions_>, \
        Vector<Element_ _comma Dimensions_ _comma LeftSub_>, \
        Vector<Element_ _comma Dimensions_ _comma RightSub_> \
    )

LongVectorAdditive_(+)
LongVectorAdditive_(-)

#define ShortVectorAdditive_(op, ValueBound, LeftBound, RightBound) \
template <typename Element_, size_t Dimensions_> \
VectorAdditive_(op, \
    ShortVector<Element_ _comma Dimensions_ _comma ValueBound>, \
    ShortVector<Element_ _comma Dimensions_ _comma LeftBound>, \
    ShortVector<Element_ _comma Dimensions_ _comma RightBound> \
)

ShortVectorAdditive_(+, true, true, false)
ShortVectorAdditive_(+, true, false, true)
ShortVectorAdditive_(+, false, false, false)

ShortVectorAdditive_(-, true, true, false)
ShortVectorAdditive_(-, false, true, true)
ShortVectorAdditive_(-, false, false, false)

#undef ShortVectorAdditive_
#undef LongVectorAdditive_
#undef VectorAdditive_
/* }}} */
/* Multiplicatve Operations {{{ */
#define VectorMultiplicative__(op, args, Value, Left) \
    template <args> \
    _finline Value operator op( \
        const Left &lhs, \
        const Element_ &rhs \
    ) { \
        Value value; \
        _forall (size_t, element, 0, Dimensions_) \
            value[element] = lhs[element] op rhs; \
        value.SetWeight(lhs.GetWeight() op rhs); \
        return value; \
    }

#define VectorMultiplicative_(args, Value, Left) \
    VectorMultiplicative__(*, _copy(args), _copy(Value), _copy(Left)) \
    VectorMultiplicative__(/, _copy(args), _copy(Value), _copy(Left)) \
    \
    template <args> \
    _finline Value operator *( \
        const Element_ &lhs, \
        const Left &rhs \
    ) { \
        return rhs * lhs; \
    }

VectorMultiplicative_(
    typename Element_ _comma size_t Dimensions_ _comma typename Sub_,
    LongVector<Element_ _comma Dimensions_>,
    Vector<Element_ _comma Dimensions_ _comma Sub_>
)

VectorMultiplicative_(
    typename Element_ _comma size_t Dimensions_,
    ShortVector<Element_ _comma Dimensions_ _comma false>,
    ShortVector<Element_ _comma Dimensions_ _comma false>
)

#undef VectorMultiplicative_
#undef VectorMultiplicative__
/* }}} */
/* Special Operations {{{ */
template <typename Element_, size_t Dimensions_, typename Sub_>
_finline Element_ Dot(
    const Vector<Element_, Dimensions_, Sub_> &lhs,
    const Vector<Element_, Dimensions_, Sub_> &rhs
) {
    _assert(lhs.GetWeight() == Zero<Element_>());
    _assert(rhs.GetWeight() == Zero<Element_>());
    Element_ value(0);
    _forall (size_t, element, 0, Dimensions_)
        value += lhs[element] * rhs[element];
    return value;
}

template <typename Element_, size_t Dimensions_, typename Sub_>
_finline Element_ Dot(const Element_ &lhs, const Vector<Element_, Dimensions_, Sub_> &rhs) {
    _assert(rhs.GetWeight() == Zero<Element_>());
    Element_ value(0);
    _forall (size_t, element, 0, Dimensions_)
        value += lhs * rhs[element];
    return value;
}

template <typename Element_, size_t Dimensions_, typename Sub_>
_finline Element_ Dot(const Vector<Element_, Dimensions_, Sub_> &lhs, const Element_ &rhs) {
    return Dot(rhs, lhs);
}

template <typename Element_>
ShortVector<Element_, 3, false> Cross(
    const ShortVector<Element_, 3, false> &lhs,
    const ShortVector<Element_, 3, false> &rhs
) {
    ShortVector<Element_, 3, false> value;
    _forall (size_t, element, 0, 3)
        value[element] =
            lhs[(element + 1) % 3] * rhs[(element + 2) % 3] -
            lhs[(element + 2) % 3] * rhs[(element + 1) % 3];
    return value;
}
/* }}} */

/* Type Definitions {{{ */
typedef ShortVector<float, 2, false> Offset2f;
typedef ShortVector<float, 2, true> Point2f;
typedef LongVector<float, 2> Vector2f;

typedef ShortVector<double, 2, false> Offset2d;
typedef ShortVector<double, 2, true> Point2d;
typedef LongVector<double, 2> Vector2d;

typedef ShortVector<float, 3, false> Offset3f;
typedef ShortVector<float, 3, true> Point3f;
typedef LongVector<float, 3> Vector3f;

typedef ShortVector<double, 3, false> Offset3d;
typedef ShortVector<double, 3, true> Point3d;
typedef LongVector<double, 3> Vector3d;

typedef Offset2d Offset2;
typedef Point2d Point2;
typedef Vector2d Vector2;

typedef Offset3d Offset3;
typedef Point3d Point3;
typedef Vector3d Vector3;
/* }}} */

template <typename Element_, unsigned Dimensions_, typename Sub_>
_finline Element_ Magnitude(const Vector<Element_, Dimensions_, Sub_> &value) {
    return SquareRoot(Dot(value, value));
}

template <typename Element_, unsigned Dimensions_>
_finline ShortVector<Element_, Dimensions_, false> Normalize(
    const ShortVector<Element_, Dimensions_, false> &value
) {
    _assert(value.GetWeight() == Zero<Element_>());
    return value / Magnitude(value);
}

}

#endif/*MENES_ALG_VECTOR_HPP*/
