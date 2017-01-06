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

#ifndef MENES_EXT_SHARED_HPP
#define MENES_EXT_SHARED_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/atomic.hpp"
#include "etl/bind.hpp"

#include "ext/allocator.hpp"
#include "ext/algorithm.hpp"
#include "ext/block.hpp"
#include "ext/refcount.hpp"
#include "ext/traits.hpp"

namespace ext {

template <typename Element_, typename Allocator_>
class SharedBlock {
  private:
    typedef SharedBlock<Element_, Allocator_> This_;

  public:
    typedef Element_ Element;
    typedef Allocator_ Allocator;

    typedef const Element *ConstIterator;
    typedef Element *Iterator;

    static const bool Sharable = true;

  private:
    EmptyBase<Allocator, Element *> data_;
    size_t size_;
    ShareMode mode_;

    api::Atomic &GetAtomic_() {
        return *reinterpret_cast<api::Atomic *>(GetBase_());
    }

    Element *GetBase_() {
        return reinterpret_cast<Element *>(reinterpret_cast<char *>(data_.member_) - sizeof(api::Atomic));
    }

    Allocator &GetAllocator_() {
        return data_.GetBase();
    }

  public:
    SharedBlock(const Allocator &allocator = Allocator()) :
        data_(allocator, NULL),
        size_(0),
        mode_(ReadOnlyExternal)
    {
    }

    explicit SharedBlock(size_t size, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(size)
    {
        if (size == 0) {
            data_.member_ = NULL;
            mode_ = ReadOnlyExternal;
        } else {
            char *data(reinterpret_cast<char *>(GetAllocator_().Allocate(sizeof(Element_) * size + sizeof(api::Atomic))));
            data_.member_ = reinterpret_cast<Element_ *>(data + sizeof(api::Atomic));
            mode_ = WritableInternal;
            new (&GetAtomic_()) api::Atomic(1);
        }
    }

    SharedBlock(const Element *data, size_t size, ShareMode mode, const Allocator &allocator = Allocator()) :
        data_(allocator, const_cast<Element *>(data)),
        size_(size),
        mode_(mode)
    {
        _assert((mode & Protection) == ReadOnly);
    }

    SharedBlock(Element *data, size_t size, ShareMode mode, const Allocator &allocator = Allocator()) :
        data_(allocator, data),
        size_(size),
        mode_(mode)
    {
    }

    SharedBlock(const This_ &rhs) :
        data_(rhs.data_),
        size_(rhs.size_),
        mode_(rhs.mode_)
    {
        if ((mode_ & Possession) == Internal)
            ++GetAtomic_();
    }

    ~SharedBlock() {
        Clear();
    }

    This_ &operator =(const This_ &rhs) {
        Clear();

        data_ = rhs.data_;
        size_ = rhs.size_;
        mode_ = rhs.mode_;

        if ((mode_ & Possession) == Internal)
            ++GetAtomic_();

        return *this;
    }

    bool IsWritable() {
        return IsEmpty() || mode_ == WritableExternal || mode_ == WritableInternal && GetAtomic_() == 1;
    }

    void Assign(const Element *data, size_t size, ShareMode mode) {
        _assert((mode & Protection) == ReadOnly);
        Assign(const_cast<Element *>(data), size, mode);
    }

    void Assign(Element *data, size_t size, ShareMode mode) {
        Clear();

        data_.member_ = data;
        size_ = size;
        mode_ = mode;
    }

    void Clear() {
        if ((mode_ & Possession) == Internal && --GetAtomic_() == 0) {
            // XXX: this typedef is due to a gcc bug
            typedef api::Atomic Atomic;
            GetAtomic_().~Atomic();
            GetAllocator_().Deallocate(GetBase_());
        }

        data_.member_ = NULL;
        size_ = 0;
        mode_ = ReadOnlyExternal;
    }

    void Delete() {
        return Clear();
    }

    ConstIterator Begin() const {
        return data_.member_;
    }

    Iterator Begin() {
        return data_.member_;
    }

    ConstIterator End() const {
        return Begin() + GetSize();
    }

    Iterator End() {
        return Begin() + GetSize();
    }

    const Allocator &GetAllocator() const {
        return data_.GetBase();
    }

    size_t GetSize() const {
        return size_;
    }

    bool IsEmpty() const {
        return GetSize() == 0;
    }

    operator const Element *() const {
        return Begin();
    }

    const Element &operator [](size_t index) const {
        _assert(index < size_);
        return Begin()[index];
    }

    Element &operator [](size_t index) {
        _assert(index < size_);
        return Begin()[index];
    }

    _finline static bool SupportsMode(ShareMode mode) {
        return true;
    }

    _finline static bool IsSharable() {
        return true;
    }
};

template < typename Element_, typename Block_ = SharedBlock<Element_> >
class CopyBlock :
    public Block_
{
  private:
    typedef CopyBlock<Element_, Block_> This_;

    // XXX: this looks fishy... think allocators?
    void Copy_() {
        Block_ data(Block_::size_);
        ext::CopyAssign(data, Block_::Begin(), Block_::End());
        ext::Swap(Block_::data_, data);
    }

  public:
    CopyBlock() {
    }

    explicit CopyBlock(size_t size) :
        Block_(size)
    {
    }

    CopyBlock(const Element_ *data, size_t size, ShareMode mode = Copy) :
        Block_(data, size, mode)
    {
        if ((mode & Copy) != 0)
            Copy_();
    }

    CopyBlock(Element_ *data, size_t size, ShareMode mode = Copy) :
        Block_(data, size, mode)
    {
        if ((mode & Copy) != 0)
            Copy_();
    }

    CopyBlock(const This_ &rhs) :
        Block_(rhs)
    {
    }

    void Touch() {
        if (!Block_::IsWritable())
            Copy_();
    }

    This_ &operator =(const This_ &rhs) {
        Block_::operator =(rhs);
        return *this;
    }
};

typedef SharedBlock<char> SharedByteBlock;

}

namespace etl {
template <typename Element_, typename Allocator_>
struct IsMemMovable< ext::SharedBlock<Element_, Allocator_> > {
    static const bool value = true;
}; }

#endif//MENES_EXT_SHARED_HPP
