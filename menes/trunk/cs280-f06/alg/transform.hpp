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

#ifndef MENES_ALG_MATRIX_HPP
#define MENES_ALG_MATRIX_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "alg/vector.hpp"

namespace alg {

template <
    typename Element_,
    unsigned Dimensions_,
    typename Sub_
> class Transform {
    typedef Transform<Element_, Dimensions_, Sub_> This_;

  public:
    typedef typename Sub_::Basis Basis;
    typedef typename Sub_::Translation Translation;

  public:
    _finline const Basis &operator [](unsigned base) const {
        return static_cast<const Sub_ *>(this)->operator [](base);
    }

    _finline Translation GetTranslation() const {
        return static_cast<const Sub_ *>(this)->GetTranslation();
    }

    template <typename Sub_>
    Vector_<Element_, Dimensions_, Sub_> operator ()(
        const Vector_<Element_, Dimensions_, Sub_> &rhs
    ) {
        Vector<Element_, Dimensions_, VectorType_> value;

        _forall (size_t, i, 0, Dimensions_) {
            Element_ &element(value[i]);
            element = Zero<Element_>();
            _forall (size_t, j, 0, Dimensions_)
                element += rhs[j] * operator [](j)[i];
        }

        Element_ weight(Zero<Element_>());
        _forall (size_t, i, 0, Dimensions_)
            weight += rhs.GetWeight() * GetTranslation()[i];
        value.SetWeight(weight);

        return value;
    }
};

template <
    typename Element_,
    unsigned Dimensions_
> class Linear :
    public Transform<
        Element_,
        Dimensions_,
        Linear<Element_, Dimensions_>
    >
{
  public:
    typedef ShortVector<Element_, Dimensions_, false> Basis;
    typedef ShortVector<Element_, Dimensions_, true> Translation;

  private:
    union {
        Basis bases_[Dimensions_];
        Element_ data_[];
    };

  public:
    _finline const Basis &operator [](unsigned base) const {
        return bases_[base];
    }

    _finline Translation GetTranslation() const {
        return Translation(Zero<Element_>());
    }
};

template <
    typename Element_,
    unsigned Dimensions_
> class Affine :
    public Transform<
        Element_,
        Dimensions_,
        Affine<Element_, Dimensions_>
    >
{
  public:
    typedef ShortVector<Element_, Dimensions_, false> Basis;
    typedef ShortVector<Element_, Dimensions_, true> Translation;

  private:
    union {
        struct {
            Basis bases_[Dimensions_];
            Translation translation_;
        };

        Element_ data_[];
    };

  public:
    _finline const Basis &operator [](unsigned base) const {
        return bases_[base];
    }

    _finline const Translation &GetTranslation() const {
        return translation_;
    }
};

template <
    typename Element_,
    unsigned Dimensions_
> class Perspective :
    public Transform<
        Element_,
        Dimensions_,
        Perspective<Element_, Dimensions_>
    >
{
  public:
    typedef LongVector<Element_, Dimensions_> Basis;
    typedef LongVector<Element_, Dimensions_> Translation;

  private:
    union {
        struct {
            Basis bases_[Dimensions_];
            Translation translation_;
        };

        Element_ data_[];
    };

  public:
    _finline const Basis &operator [](unsigned base) const {
        return bases_[base];
    }

    _finline const Translation &GetTranslation() const {
        return translation_;
    }
};

/* Type Definitions {{{ */
typedef Linear<float, 2> Linear2f;
typedef Affine<float, 2> Affine2f;
typedef Perspective<float, 2> Perspective2f;

typedef Linear<double, 2> Linear2d;
typedef Affine<double, 2> Affine2d;
typedef Perspective<double, 2> Perspective2d;

typedef Linear<float, 3> Linear3f;
typedef Affine<float, 3> Affine3f;
typedef Perspective<float, 3> Perspective3f;

typedef Linear<double, 3> Linear3d;
typedef Affine<double, 3> Affine3d;
typedef Perspective<double, 3> Perspective3d;

typedef Linear2d Linear2;
typedef Affine2d Affine2;
typedef Perspective2d Perspective2;

typedef Linear3d Linear3;
typedef Affine3d Affine3;
typedef Perspective3d Perspective3;
/* }}} */

#if 0
    template <VectorType VectorType_>
    _finline Vector<Element_, Dimensions_, VectorType_> operator () (
        const Vector<Element_, Dimensions_, VectorType_> &rhs
    ) {
        typedef Vector<Element_, Dimensions_, VectorType_> Vector_;
        Vector_ value;
        _forall (size_t, i, 0, Vector_::Elements)
            value[i] = Dot(rhs[i], operator [](i));
        return value;
    }
};

/* be::Transform_ {{{ */
/*namespace be {
    template <
        typename Element_,
        unsigned Dimensions_,
        VectorType VectorType_
    > struct Transform_<
        Element_,
        Dimensions_,
        VectorType_
    > {
        typedef Transform<Element_, Dimensions_> Transform_;
        typedef Vector<Element_, Dimensions_, VectorType_> Vector_;

        Vector_ Execute(const Transform_ &lhs, const Vector_ &rhs) {
        }
    };
}*/
/* }}} */

/*template <typename Type_, unsigned Rows_, unsigned Pivot_, unsigned Cols_>
Matrix<Type_, Rows_, Cols_> operator *(
    const Matrix<Type_, Rows_, Pivot_> &lhs,
    const Matrix<Type_, Pivot_, Cols_> &rhs
) {
    Matrix<Type_, Rows_, Cols_> tmp;

    for (unsigned r(0); r != Rows_; ++r)
    for (unsigned c(0); c != Cols_; ++c) {
        Type_ &value(tmp[c][r]);
        value = 0;

        for (unsigned p(0); p != Pivot_; ++p)
            value += lhs[p][r] * rhs[c][p];
    }

    return tmp;
}

template <typename Type_, unsigned Rows_, unsigned Cols_>
Vector<Type_, Cols_> operator *(
    const Vector<Type_, Rows_> &lhs,
    const Matrix<Type_, Rows_, Cols_> &rhs
) {
    Vector<Type_, Cols_> tmp;

    for (unsigned c(0); c != Cols_; ++c) {
        Type_ &value(tmp[c]);
        value = 0;

        for (unsigned r(0); r != Rows_; ++r)
            value += lhs[r] * rhs[c][r];
    }

    return tmp;
}

template <typename Type_, unsigned Rows_, unsigned Cols_>
Vector<Type_, Rows_> operator *(
    const Matrix<Type_, Rows_, Cols_> &lhs,
    const Vector<Type_, Cols_> &rhs
) {
    Vector<Type_, Rows_> tmp;

    for (unsigned r(0); r != Rows_; ++r) {
        Type_ &value(tmp(r));
        value = 0;

        for (unsigned c(0); c != Cols_; ++c)
            value += lhs[c][r] * rhs(c);
    }

    return tmp;
}

template <typename Type_, unsigned Rows_, unsigned Cols_>
Matrix<Type_, Rows_, Cols_> Invert(Matrix<Type_, Rows_, Cols_> &value) {
    Matrix<Type_, Rows_, Cols_> tmp;
    _assert(false);
    return tmp;
}

typedef Matrix<float, 3, 3> Matrix33f;
typedef Matrix<float, 3, 4> Matrix34f;
typedef Matrix<float, 4, 4> Matrix44f;

typedef Matrix<double, 3, 3> Matrix33d;
typedef Matrix<double, 3, 4> Matrix34d;
typedef Matrix<double, 4, 4> Matrix44d;*/
#endif

}

#endif/*MENES_ALG_MATRIX_HPP*/
