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

#ifndef MENES_EXT_ARRAY_HPP
#define MENES_EXT_ARRAY_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/traits.hpp"

namespace ext {

template <size_t Size_, typename Element_>
class Array {
  public:
    typedef const Element_ *ConstIterator;
    typedef Element_ *Iterator;

  private:
    Element_ data_[Size_];

  public:
    Array() {
    }

    template <typename Type_>
    Array(const Type_ value[Size_]) {
        ext::CopyAssign(data_, value, value + Size_);
    }

    template <typename Type_>
    explicit Array(const Type_ &value) {
        for (Iterator i(Begin()); i != End(); ++i)
            *i = value;
    }

    const Element_ &operator [](size_t index) const {
        _assert(index >= 0 && index < Size_);
        return data_[index];
    }

    Element_ &operator [](size_t index) {
        _assert(index >= 0 && index < Size_);
        return data_[index];
    }

    ConstIterator Begin() const {
        return data_;
    }

    Iterator Begin() {
        return data_;
    }

    ConstIterator End() const {
        return data_ + Size_;
    }

    Iterator End() {
        return data_ + Size_;
    }

    bool IsEmpty() const {
        return false;
    }

    size_t GetSize() const {
        return Size_;
    }
};

}

#endif//MENES_EXT_ARRAY_HPP
