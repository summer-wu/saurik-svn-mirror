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

#ifndef MENES_EXT_HEAPOBJECT_HPP
#define MENES_EXT_HEAPOBJECT_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/pointer.hpp"
#include "ext/scopes.hpp"

namespace ext {

// XXX: I hate this class... please don't use it

template <
    typename Type_,
    typename Allocator_ = ConstructorAllocator,
    typename Lock_ = FreeLock
>
class LazyHeapObject :
    public SmartPointer< Type_, Type_ *, LazyHeapObject<Type_, Allocator_> >
{
    friend class SmartPointer< Type_, Type_ *, LazyHeapObject<Type_, Allocator_> >;

  private:
    typedef LazyHeapObject<Type_, Allocator_, Lock_> This_;
    typedef SmartPointer< Type_, Type_ *, LazyHeapObject<Type_, Allocator_> > Base_;

  public:
    typedef typename Allocator_::template Template<Type_>::Result Allocator;

  private:
    typedef ext::Pointer<Type_, Allocator_> Pointer_;
    mutable Pointer_ instance_;
    mutable Lock_ lock_;

    void Create_() const {
        ext::ScopeLock<Lock_> guard(lock_);
        if (instance_ == NULL) {
            Pointer_ temp(instance_.GetAllocator().AllocateEx(), true, instance_.GetAllocator());
            ext::Swap(instance_, temp);
        }
    }

    inline Type_ *GetSmartPointer_() const {
        if (instance_ == NULL)
            Create_();
        _assert(instance_ != NULL);
        return instance_;
    }

  public:
    explicit LazyHeapObject(const Allocator &allocator = Allocator()) :
        instance_(allocator)
    {
    }

    LazyHeapObject(const This_ &rhs) :
        instance_(rhs.allocator)
    {
        *this = rhs;
    }

    bool IsEmpty() const {
        return instance_.IsEmpty();
    }

    void Create() {
        Create_();
    }

    void Clear() {
        ext::ScopeLock<Lock_> guard(lock_);
        instance_.Clear();
    }
};

}

#endif//MENES_EXT_HEAPOBJECT_HPP
