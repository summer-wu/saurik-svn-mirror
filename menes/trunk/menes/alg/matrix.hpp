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

#ifndef MENES_ALG_MATRIX_HPP
#define MENES_ALG_MATRIX_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"

#include "ios/helpers.hpp"
#include "ios/streams.hpp"

namespace alg {

template <typename Element_>
class Matrix {
  private:
    size_t rows_;
    size_t columns_;

    ext::Vector< Element_, ext::SharedBlock<Element_> > data_;

  public:
    Matrix() :
        rows_(0),
        columns_(0)
    {
    }

    Matrix(size_t rows, size_t columns) {
        SetDimensions(rows, columns);
    }

    size_t GetColumns() const {
        return columns_;
    }

    size_t GetRows() const {
        return rows_;
    }

    void SetDimensions(size_t rows, size_t columns) {
        rows_ = rows;
        columns_ = columns;
        data_.SetSize(rows * columns);
    }

    Element_ operator ()(size_t row, size_t column) const {
        return data_[row * columns_ + column];
    }

    Element_ &operator ()(size_t row, size_t column) {
        return data_[row * columns_ + column];
    }
};

template <typename Element_>
class TriangularMatrix :
    public Matrix<Element_>
{
  private:
    typedef Matrix<Element_> Base_;

  public:
    TriangularMatrix() {
    }

    TriangularMatrix(unsigned size) :
        Base_(size, size)
    {
    }

    Element_ operator ()(size_t row, size_t column) const {
        return row < column ? Base_::operator()(row, column) : Base_::operator()(column, row);
    }

    Element_ &operator ()(size_t row, size_t column) {
        return row < column ? Base_::operator()(row, column) : Base_::operator()(column, row);
    }
};

template <typename Element_>
Matrix<Element_> operator +(const Matrix<Element_> &lhs, const Matrix<Element_> &rhs) {
    _assert(lhs.GetRows() == rhs.GetRows() && lhs.GetColumns() == rhs.GetColumns());

    unsigned rows(lhs.GetRows());
    unsigned columns(lhs.GetColumns());

    Matrix<Element_> result(rows, columns);

    _forall (unsigned, row, 0, rows)
        _forall (unsigned, column, 0, columns)
            result(row, column) = lhs(row, column) + rhs(row, column);

    return result;
}

template <typename Element_>
Matrix<Element_> operator -(const Matrix<Element_> &lhs, const Matrix<Element_> &rhs) {
    _assert(lhs.GetRows() == rhs.GetRows() && lhs.GetColumns() == rhs.GetColumns());

    unsigned rows(lhs.GetRows());
    unsigned columns(lhs.GetColumns());

    Matrix<Element_> result(rows, columns);

    _forall (unsigned, row, 0, rows)
        _forall (unsigned, column, 0, columns)
            result(row, column) = lhs(row, column) - rhs(row, column);

    return result;
}

template <typename Element_>
Matrix<Element_> operator *(const Matrix<Element_> &lhs, const Matrix<Element_> &rhs) {
    _assert(lhs.GetColumns() == rhs.GetRows());

    unsigned rows(lhs.GetRows());
    unsigned columns(rhs.GetColumns());
    unsigned stride(lhs.GetColumns());

    Matrix<Element_> result(rows, columns);

    _forall (unsigned, row, 0, rows)
        _forall (unsigned column(0); column != columns; ++column) {
            double value(0);
            _forall (unsigned, i, 0, stride)
                value += lhs(row, i) * rhs(i, column);
            result(row, column) = value;
        }

    return result;
}

template <typename Element_>
ios::PrintReader &operator >>(ios::PrintReader &reader, Matrix<Element_> &matrix) {
    _L<cse::String> dimensions(ext::Split(ios::ReadLine(reader), _B(" ")));
    size_t rows(lexical_cast<size_t>(dimensions[0]));
    size_t columns(lexical_cast<size_t>(dimensions[1]));
    matrix.SetDimensions(rows, columns);
    _forall (size_t, row, 0, rows) {
        _L<cse::String> data(ext::Split(ios::ReadLine(reader), _B(" ")));
        _assert(data.GetSize() == columns || data.GetSize() == columns + 1 && data[columns].IsEmpty());
        _forall (size_t, column, 0, columns)
            matrix(row, column) = lexical_cast<double>(data[column]);
    }

    return reader;
}

template <typename Element_>
ios::PrintWriter &operator <<(ios::PrintWriter &writer, const Matrix<Element_> &matrix) {
    size_t rows(matrix.GetRows());
    size_t columns(matrix.GetColumns());
    writer << rows << " " << columns;
    _forall (size_t, row, 0, rows) {
        writer << ios::NewLine;
        _forall (size_t, column, 0, columns) {
            if (column != 0)
                writer << " ";
            writer << matrix(row, column);
        }
    }
    return writer;
}

}

#endif//MENES_ALG_MATRIX_HPP
