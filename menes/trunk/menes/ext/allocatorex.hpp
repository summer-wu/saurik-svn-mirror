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

#ifndef MENES_EXT_ALLOCATOREX_HPP
#define MENES_EXT_ALLOCATOREX_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/bind.hpp"

#include "ext/allocator.hpp"
#include "ext/refcount.hpp"

#include <new>

namespace ext {

struct Allocator :
    public ext::ReferenceCounted
{
    virtual void *Allocate(size_t size) = 0;
    virtual void Deallocate(void *address) = 0;

    template <typename Type_>
    _finline Type_ *Allocate(size_t size) {
        return reinterpret_cast<Type_ *>(Allocate(size));
    }
};

template <typename Type_>
struct AllocatorEx :
    public Allocator
{
    virtual void *Allocate(size_t size) {
        _assert(size % sizeof(Type_) == 0);
        return reinterpret_cast<void *>(AllocateEx(size / sizeof(Type_)));
    }

    virtual void Deallocate(void *address) {
        DeallocateEx(reinterpret_cast<Type_ *>(address));
    }

    virtual Type_ *AllocateEx() {
        return AllocateEx(1);
    }

    virtual Type_ *AllocateEx(size_t size) {
        return Allocator::Allocate<Type_>(size * sizeof(Type_));
    }

    virtual void DeallocateEx(Type_ *address) = 0;
};

class IndirectAllocator {
  private:
    _R<Allocator> allocator_;

  public:
    IndirectAllocator() {
    }

    IndirectAllocator(Allocator *allocator) :
        allocator_(allocator)
    {
    }

    IndirectAllocator(const _R<Allocator> &allocator) :
        allocator_(allocator)
    {
    }

    inline void *Allocate(size_t size) {
        return allocator_->Allocate(size);
    }

    inline void Deallocate(void *address) {
        allocator_->Deallocate(address);
    }
};

template <typename Type_>
class IndirectAllocatorEx {
  private:
    typedef _R< AllocatorEx<Type_> > Handle_;
    Handle_ allocator_;

  public:
    IndirectAllocatorEx() {
    }

    IndirectAllocatorEx(AllocatorEx<Type_> *allocator) :
        allocator_(allocator)
    {
    }

    IndirectAllocatorEx(const Handle_ &allocator) :
        allocator_(allocator)
    {
    }

    inline Type_ *AllocateEx() {
        return allocator_->AllocateEx();
    }

    inline Type_ *AllocateEx(size_t size) {
        return allocator_->AllocateEx(size);
    }

    inline void DeallocateEx(Type_ *address) {
        allocator_->DeallocateEx(address);
    }
};

template <typename Allocator_>
struct ThunkedAllocator :
    public Allocator
{
  public:
    typedef Allocator_ Allocator;

  private:
    Allocator allocator_;

  public:
    ThunkedAllocator(const Allocator &allocator = Allocator()) :
        allocator_(allocator)
    {
    }

    virtual void *Allocate(size_t size) {
        return allocator_.Allocate(size);
    }

    virtual void Deallocate(void *address) {
        allocator_.Deallocate(address);
    }
};

template <typename Allocator_>
struct ThunkedAllocatorEx {
    template <typename Type_>
    struct Template {
        struct Result :
            public AllocatorEx<Type_>
        {
          public:
            typedef typename Allocator_::template Template<Type_>::Result Allocator;

          private:
            Allocator allocator_;

          public:
            Result(const Allocator &allocator = Allocator()) :
                allocator_(allocator)
            {
            }

            virtual Type_ *AllocateEx() {
                return allocator_.AllocateEx();
            }

            virtual Type_ *AllocateEx(size_t size) {
                return allocator_.AllocateEx(size);
            }

            virtual void DeallocateEx(Type_ *address) {
                allocator_.DeallocateEx(address);
            }
        };
    };
};

typedef ThunkedAllocator<IndirectAllocator> ThunkedIndirectAllocator;
typedef ThunkedAllocatorEx< etl::Bind1<IndirectAllocatorEx> >::Template<char>::Result ThunkedIndirectAllocatorEx;

template <typename Type_, typename Subclass_, typename Allocator_ = NewAllocator>
struct ExtraAllocator :
    public Allocator_
{
    ExtraAllocator(const Allocator_ &allocator = Allocator_()) :
        Allocator_(allocator)
    {
    }

    void *Allocate(size_t size) {
        Type_ *extra(reinterpret_cast<Type_ *>(Allocator_::Allocate(sizeof(Type_) + size)));
        // XXX: maybe I should let the other guy do this
        new (extra) Type_();
        static_cast<Subclass_ *>(this)->Allocate_(size, extra);
        return reinterpret_cast<void *>(extra + 1);
    }

    void Deallocate(void *address) {
        if (address == NULL)
            return;

        Type_ *extra(reinterpret_cast<Type_ *>(address) - 1);
        // XXX: maybe I should let the other guy do this
        extra->~Type_();
        static_cast<Subclass_ *>(this)->Deallocate_(extra);
        Allocator_::Deallocate(reinterpret_cast<void *>(extra));
    }
};

}

#endif//MENES_EXT_ALLOCATOREX_HPP
