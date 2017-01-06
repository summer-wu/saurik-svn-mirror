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

#ifndef MENES_ALG_RECTANGLE_HPP
#define MENES_ALG_RECTANGLE_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "alg/tensor.hpp"

namespace alg {

template <typename Type_>
class Rectangle2 {
  public:
    typedef typename Vector<2, Type_>::Result Vector2;

  private:
    Type_ top_, left_;
    Type_ bottom_, right_;

  public:
    Rectangle2() {
    }

    void SetT(Type_ top) { top_ = top; }
    void SetL(Type_ left) { left_ = left; }

    void SetB(Type_ bottom) { bottom_ = bottom; }
    void SetR(Type_ right) { right_ = right; }

    void SetW(Type_ width) { SetR(left_ + width); }
    void SetH(Type_ height) { SetB(top_ + height); }

    void SetTL(Type_ top, Type_ left) {
        SetT(top); SetL(left);
    }

    void SetBR(Type_ bottom, Type_ right) {
        SetB(bottom); SetR(right);
    }

    void SetTB(Type_ top, Type_ bottom) {
        SetT(top); SetB(bottom);
    }

    void SetLR(Type_ left, Type_ right) {
        SetL(left); SetR(right);
    }

    void SetWH(Type_ width, Type_ height) {
        SetW(width); SetH(height);
    }

    void SetTLBR(Type_ top, Type_ left, Type_ bottom, Type_ right) {
        SetTL(top, left); SetBR(bottom, right);
    }

    void SetTLWH(Type_ top, Type_ left, Type_ width, Type_ height) {
        SetTL(top, left); SetWH(width, height);
    }

    const Type_ &Top() const    { return top_; }
          Type_ &Top()          { return top_; }

    const Type_ &Left() const   { return left_; }
          Type_ &Left()         { return left_; }

    const Type_ &Bottom() const { return bottom_; }
          Type_ &Bottom()       { return bottom_; }

    const Type_ &Right() const  { return right_; }
          Type_ &Right()        { return right_; }

    const Type_ Width() const   { return right_ - left_; }
    const Type_ Height() const  { return bottom_ - top_; }

    Vector2 GetLT() const { return Vector2(left_, top_); }
    Vector2 GetRT() const { return Vector2(right_, top_); }
    Vector2 GetRB() const { return Vector2(right_, bottom_); }
    Vector2 GetLB() const { return Vector2(left_, bottom_); }
};

typedef Rectangle2<float> Rectangle2f;

}

#endif//MENES_ALG_RECTANGLE_HPP
