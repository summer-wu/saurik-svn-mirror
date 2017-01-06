/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003  Jay Freeman (saurik)
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

#ifndef MENES_EXT_SINGULARPTR_HPP
#define MENES_EXT_SINGULARPTR_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace ext {

template <typename Type_>
class SingularPointer {
    template <typename TypeF_>
    friend void Swap(SingularPointer<TypeF_> &lhs, SingularPointer<TypeF_> &rhs);

  private:
    typedef SingularPointer<Type_> This_;

    Type_ *pointer_;

    struct Reference_ {
        Type_ *pointer_;
    };

  public:
    explicit SingularPointer(Type_ *rhs = NULL) :
        pointer_(rhs)
    {
    }

    SingularPointer(SingularPointer &rhs) :
        pointer_(rhs.Release())
    {
    }

    SingularPointer(Reference_ rhs) :
        pointer_(rhs.pointer_)
    {
    }

    ~SingularPointer() {
        delete pointer_;
    }

    This_ &operator =(This_ &rhs) {
        delete pointer_;
        pointer_ = rhs.Release();
    }

    template <typename Type2_>
    SingularPointer<Type2_> &operator =(SingularPointer<Type2_> &rhs) {
        delete pointer_;
        pointer_ = rhs.Release();
    }

    SingularPointer &operator =(Type_ *rhs) {
        delete pointer_;
        pointer_ = rhs;
    }

    Type_ *Get() const {
        return pointer_;
    }

    void Assign(Type_ *pointer) const {
        pointer_ = pointer;
    }

    Type_ *Release() {
        Type_ *pointer(pointer_);
        pointer_ = NULL;
        return pointer;
    }

    bool operator ==(const This_ &rhs) const {
        return pointer_ == rhs.pointer_;
    }

    bool operator !=(const This_ &rhs) const {
        return pointer_ != rhs.pointer_;
    }

    template <typename Type2_>
    bool operator ==(const Type2_ *rhs) const {
        return pointer_ == rhs;
    }

    template <typename Type2_>
    bool operator !=(const Type2_ *rhs) const {
        return pointer_ != rhs;
    }

    Type_ &operator *() const {
        return *pointer_;
    }

    Type_ *operator ->() const {
        return pointer_;
    }

    operator Type_ *() const {
        return pointer_;
    }

    template <typename Type2_>
    operator typename SingularPointer<Type2_>::Reference_() {
        typename SingularPointer<Type2_>::Reference_ reference;
        reference.pointer_ = Release();
        return reference;
    }

    template <typename Type2_>
    operator SingularPointer<Type2_>() {
        return SingularPointer<Type2_>(Release());
    }
};

template <typename TypeL_, typename TypeR_>
bool operator ==(const TypeL_ *lhs, const SingularPointer<TypeR_> &rhs) {
    return rhs == lhs;
}

template <typename TypeL_, typename TypeR_>
bool operator !=(const TypeL_ *lhs, const SingularPointer<TypeR_> &rhs) {
    return rhs != lhs;
}

template <typename Type_>
void Swap(SingularPointer<Type_> &lhs, SingularPointer<Type_> &rhs) {
    ext::Swap(lhs.pointer_, rhs.pointer_);
}

}

#endif//MENES_EXT_SINGULARPTR_HPP
