/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2007-2009  Jay Freeman (saurik)
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

#ifndef MENES_EXE_MACH_O_POINTER_HPP
#define MENES_EXE_MACH_O_POINTER_HPP

#include "cxx/platform.hpp"
#include "exe/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace exe {

template <typename Type_>
class Pointer {
  private:
    typedef Pointer<Type_> This_;

  private:
    const Binary *binary_;
    const Type_ *value_;

  public:
    Pointer(const Binary *binary = NULL, const Type_ *value = NULL) :
        binary_(binary),
        value_(value)
    {
    }

    operator const Type_ *() const {
        return value_;
    }

    const Type_ *operator ->() const {
        return value_;
    }

    This_ &operator ++() {
        ++value_;
        return *this;
    }

    This_ operator +(size_t offset) const {
        return This_(binary_, value_ + offset);
    }

    This_ operator -(size_t offset) const {
        return This_(binary_, value_ - offset);
    }

    template <typename Value_>
    Value_ Swap(Value_ value) {
        return binary_->Swap(value);
    }
};

}

#endif//MENES_EXE_MACH_O_POINTER_HPP
