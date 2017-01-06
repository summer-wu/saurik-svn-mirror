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

#ifndef MENES_EXT_BLOCK_XXH
#define MENES_EXT_BLOCK_XXH

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/traits.hpp"

#include "ext/algorithm.hpp"
#include "ext/allocator.hpp"
#include "ext/pointer.hpp"
#include "ext/types.hpp"

namespace ext {

enum ShareMode {
    Protection       = 0x01,
    ReadOnly         = 0x00,
    Writable         = 0x01,

    Possession       = 0x02,
    External         = 0x00,
    Internal         = 0x02,

    Copy             = 0x04,

    ReadOnlyExternal = 0x00,
    WritableExternal = 0x01,
    ReadOnlyInternal = 0x02,
    WritableInternal = 0x03
};

template <typename Element_, typename Allocator_>
class Block {
  private:
    typedef Block<Element_, Allocator_> This_;

  public:
    typedef const Element_ *ConstIterator;
    typedef Element_ *Iterator;

    typedef typename Allocator_::template Template<Element_>::Result Allocator;

  public:
    static const bool Sharable = false;

  private:
    typedef Pointer< Element_, Allocator_ > Data_;
    Data_ data_;

    size_t size_;

  public:
    explicit Block(const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
    }

    explicit Block(size_t size, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(size)
    {
        Reallocate(size);
    }

    void Assign(const Element_ *data, size_t size, ShareMode mode) {
        _assert(false);
    }

    Iterator Begin() {
        return data_;
    }

    Iterator End() {
        return Begin() + size_;
    }

    void Clear() {
        Delete();
    }

    void Delete() {
        data_.Clear();
        size_ = 0;
    }

    _finline bool IsWritable() {
        return true;
    }

    operator Element_ *() {
        return Begin();
    }

  public:
    ConstIterator Begin() const {
        return data_;
    }

    ConstIterator End() const {
        return Begin() + size_;
    }

    const Allocator &GetAllocator() const {
        return data_.GetAllocator();
    }

    size_t GetSize() const {
        return size_;
    }

    bool IsEmpty() const {
        return GetSize() == 0;
    }

    operator const Element_ *() const {
        return Begin();
    }

    void Reallocate(size_t size) {
        data_.Mend(data_.GetAllocator().AllocateEx(size));
        size_ = size;
    }

  public:
    _finline static bool SupportsMode(ShareMode mode) {
        return mode == Copy;
    }

    _finline static bool IsSharable() {
        return false;
    }
};

}

namespace etl {

template <typename Element_, typename Allocator_>
struct IsMemMovable< ext::Block< Element_, Allocator_ > > {
  public:
    static bool const value = true;
};

}

#endif//MENES_EXT_BLOCK_XXH
