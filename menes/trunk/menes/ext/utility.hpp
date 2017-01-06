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

#ifndef MENES_EXT_UTILITY_HPP
#define MENES_EXT_UTILITY_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cxx/assert.hpp"

namespace ext {

template <typename Base2_, typename Member_>
class EmptyBase :
    public Base2_
{
  public:
    Member_ member_;

    EmptyBase() {
    }

    explicit EmptyBase(const Base2_ &base) :
        Base2_(base)
    {
    }

    EmptyBase(const Base2_ &base, const Member_ &member) :
        Base2_(base),
        member_(member)
    {
    }

    const Base2_ &GetBase() const {
        return *this;
    }

    Base2_ &GetBase() {
        return *this;
    }
};

template <typename Direct_, typename Indirect_, typename Derived_>
class SmartPointer {
  public:
    Direct_ &operator *() const {
        return *static_cast<const Derived_ *>(this)->GetSmartPointer_();
    }

    Indirect_ operator ->() const {
        Indirect_ indirect(static_cast<const Derived_ *>(this)->GetSmartPointer_());
        // XXX: this assert may fail to compile someday and may have to be removed
        _assert(indirect != NULL);
        return indirect;
    }

    /* // XXX: most things shouldn't have had this
    operator Indirect_() const {
        return static_cast<const Derived_ *>(this)->GetSmartPointer_();
    } */
};

template <typename Indirect_, typename Derived_>
class SmartPointer<void, Indirect_, Derived_> {
  public:
    Indirect_ operator ->() const {
        Indirect_ indirect(static_cast<const Derived_ *>(this)->GetSmartPointer_());
        // XXX: this assert may fail to compile someday and may have to be removed
        _assert(indirect != NULL);
        return indirect;
    }

    /* // XXX: most things shouldn't have had this
    operator Indirect_() const {
        return static_cast<const Derived_ *>(this)->GetSmartPointer_();
    } */
};

}

#endif//MENES_EXT_UTILITY_HPP
