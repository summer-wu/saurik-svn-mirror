/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#ifndef MENES_EXT_HEAPREF_HPP
#define MENES_EXT_HEAPREF_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/refcount.hpp"
#include "ext/workarounds.hpp"

namespace ext {

template <typename Counted_>
class HeapReference :
    public Counted_
{
  private:
    typedef HeapReference<Counted_> This_;

    virtual ext::Allocation GetAllocation_() {
        return ext::HeapAllocated;
    }

    virtual void _initialize() {
        --this->ReferenceCounted::count_;
        return Counted_::_initialize();
    }

  public:
    EXT_PASSTHRU_CONSTRUCT(HeapReference, Counted_)

    HeapReference(const This_ &rhs) :
        Counted_(rhs)
    {
        _initialize();
    }

    ~HeapReference() {
        _assert(this->ReferenceCounted::count_ == 0);
        ++this->ReferenceCounted::count_;
        static_cast<ReferenceCounted *>(this)->_finalize();
    }
};

}

#define _H ext::HeapReference

#endif//MENES_EXT_HEAPREF_HPP
