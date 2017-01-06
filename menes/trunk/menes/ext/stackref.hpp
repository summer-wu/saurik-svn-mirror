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

#ifndef MENES_EXT_STACKREF_HPP
#define MENES_EXT_STACKREF_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/refcount.hpp"
#include "ext/workarounds.hpp"

namespace ext {

template <typename Counted_>
class StackReference :
    public Counted_
{
  private:
    typedef StackReference<Counted_> This_;

    virtual ext::Allocation GetAllocation_() {
        return ext::StackAllocated;
    }

    virtual void _initialize() {
        return Counted_::_initialize();
    }

  public:
    EXT_PASSTHRU_CONSTRUCT(StackReference, Counted_)

    StackReference(const This_ &rhs) :
        Counted_(rhs)
    {
        _initialize();
    }

    ~StackReference() {
        _assert(this->ReferenceCounted::count_ == 1);
        /*      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
              If you hit this assert then you have allocated a ReferenceCounted object on the
            stack (note that "on the stack includes inheriting from one") utilizing using the
            ext::StackReference<> template and someone Release()d the object one too many times
            before it went out of it's scope and it deleted itself before the stack unwound or
            it got deleted in the more usual fashion.
        */

        static_cast<ReferenceCounted *>(this)->_finalize();
    }
};

}

// XXX: this is a horrible "fix"
#ifdef _MSC_VER
#define _S _V
#include <iterator>
#undef _S
#endif

#define _S ext::StackReference

#endif//MENES_EXT_STACKREF_HPP
