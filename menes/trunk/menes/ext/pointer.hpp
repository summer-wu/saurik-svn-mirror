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

#ifndef MENES_EXT_POINTER_HPP
#define MENES_EXT_POINTER_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

// XXX: this is only included for ext::Swap() and should be fixed
#include "ext/algorithm.hpp"
#include "ext/allocator.hpp"
#include "ext/utility.hpp"

namespace ext {

template <typename Type_, typename Allocator_>
class Pointer :
    public SmartPointer< Type_, Type_ *, Pointer<Type_, Allocator_> >
{
    friend class SmartPointer< Type_, Type_ *, Pointer<Type_, Allocator_> >;

    template <typename TypeF_, typename AllocatorF_>
    friend void Swap(Pointer<TypeF_, AllocatorF_> &lhs, Pointer<TypeF_, AllocatorF_> &rhs);

  private:
    typedef Pointer<Type_, Allocator_> This_;

  public:
    typedef typename Allocator_::template Template<Type_>::Result Allocator;

  private:
    EmptyBase<Allocator, Type_ *> pointer_;

    _finline Type_ *GetSmartPointer_() const {
        return GetValue();
    }

    Pointer(const This_ &rhs);
    This_ &operator =(const This_ &rhs);

  public:
    Pointer(const Allocator &allocator = Allocator()) :
        pointer_(allocator, NULL)
    {
    }

    Pointer(Type_ *pointer, bool mended = false, const Allocator &allocator = Allocator()) :
        pointer_(allocator, pointer)
    {
        _assert(mended);
    }

    ~Pointer() {
        Clear();
    }

    const Allocator &GetAllocator() const {
        return pointer_.GetBase();
    }

    Allocator &GetAllocator() {
        return pointer_.GetBase();
    }

    bool IsEmpty() const {
        return pointer_.member_ == NULL;
    }

    Type_ *Release() {
        Type_ *pointer(pointer_.member_);
        pointer_.member_ = NULL;
        return pointer;
    }

    void Clear() {
        pointer_.DeallocateEx(pointer_.member_);
        Release();
    }

    void Mend(Type_ *pointer) {
        Clear();
        pointer_.member_ = pointer;
    }

    inline Type_ *GetValue() const {
        return pointer_.member_;
    }

    inline operator Type_ *() const {
        return GetValue();
    }

    inline void Swap(Type_ *&pointer) {
        ext::Swap(pointer_.member_, pointer);
    }
};

template <typename Type_, typename Allocator_>
inline void Swap(Pointer<Type_, Allocator_> &lhs, Pointer<Type_, Allocator_> &rhs) {
    ext::Swap(lhs.pointer_, rhs.pointer_);
}

template <typename Type_, typename Allocator_ = ConstructorAllocator>
class AutoPointer :
    public Pointer<Type_, Allocator_>
{
  private:
    typedef Pointer<Type_, Allocator_> Base_;

  public:
    typedef typename Allocator_::template Template<Type_>::Result Allocator;

    AutoPointer() :
        Base_(Allocator::AllocateEx(), true)
    {
    }

    AutoPointer(size_t size) :
        Base_(Allocator::AllocateEx(size), true)
    {
    }
};

}

namespace etl {
template <typename Type_, typename Allocator_>
struct IsMemMovable< ext::Pointer<Type_, Allocator_> > {
    static const bool value = IsMemMovable<typename ext::Pointer<Type_, Allocator_>::Allocator>::value;
}; }

#endif//MENES_EXT_POINTER_HPP
