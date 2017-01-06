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

#ifndef MENES_EXT_HEAPVALUE_HPP
#define MENES_EXT_HEAPVALUE_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/pointer.hpp"
#include "ext/refcount.hpp"

namespace ext {

template <typename Type_>
class HeapValueManager :
    public _nullReferenceHelper<Type_ *>
{
  private:
    typedef _nullReferenceHelper<Type_ *> Base_;

  public:
    static void Take(typename Base_::Value &lhs, const typename Base_::Value &rhs) {
        lhs = new Type_(*rhs);
    }

    static bool Drop(const typename Base_::Value &value) {
        delete value;
        return true;
    }
};

template <typename Type_>
class HeapValue :
    public Handle< Type_, HeapValueManager<Type_> >
{
  private:
    typedef HeapValue<Type_> This_;
    typedef Handle< Type_, HeapValueManager<Type_> > Base_;

  public:
    HeapValue() :
        Base_()
    {
    }

    HeapValue(Type_ *rhs, bool mended = false) :
        Base_(rhs, mended)
    {
    }

    This_ &operator =(const This_ &rhs) {
        Base_::operator =(rhs);
        return *this;
    }

    This_ &operator =(Type_ *rhs) {
        Base_::operator =(rhs);
        return *this;
    }
};

}

#endif//MENES_EXT_HEAPVALUE_HPP
