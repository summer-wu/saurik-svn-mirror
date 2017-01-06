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

#ifndef MENES_EXT_SUBALLOC_HPP
#define MENES_EXT_SUBALLOC_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/alignment.hpp"

#include "ext/allocatorex.hpp"
#include "ext/foreach.hpp"
#include "ext/scopes.hpp"

#include "log/logging.hpp"
#include "opt/profile.hpp"

namespace ext {

// XXX: tags should be formalized and coheazed
// XXX: is this backwards?
namespace be {
inline Buffer TagToBuffer_(uintptr_t tag) {
    Buffer result;
    _forall (size_t, i, 0, sizeof(tag))
        result.InsertLast((tag >> (i * 8)) & 0xff);
    return result;
} }

template <typename Allocator_, typename Lock_>
class SubAllocator :
    public Allocator
{
  private:
    static const unsigned Size_ = 1024;

  public:
    typedef typename Allocator_::template Template<char>::Result Allocator;

  private:
    Allocator allocator_;

    //typedef Pointer<char, Allocator_> Pointer_;
    typedef Vector< char *, Block<char *, Allocator_> > Planes_;
    Planes_ planes_;

    // XXX: if this isn't true then this class will destroy your heap
    //etl::StaticAssert<etl::IsMemMovable<Pointer_>::value> prereq_;

    size_t unit_;
    char *free_;

    Lock_ lock_;
    unsigned outstanding_;

    uintptr_t tag_;

    _finline char *&GetNext_(char *entry) {
        return *reinterpret_cast<char **>(entry);
    }

    void AllocatePlane_() {
        size_t size(unit_ + sizeof(char *));
        size_t block(size > 256 ? 256 * Size_ / unit_ : Size_);

        char *begin(allocator_.AllocateEx(block * size));
        char *end(begin + (block - 1) * size);

        for (char *i(begin); i != end; i += size)
            GetNext_(i) = i + size;

        ext::ScopeLock<Lock_> guard(lock_);
        planes_.InsertLast(begin);
        GetNext_(end) = free_;
        free_ = begin;
    }

    char *GetEntry_() {
        if (free_ != NULL) {
            ext::ScopeLock<Lock_> guard(lock_);

            char *first(free_);
            free_ = GetNext_(first);
            GetNext_(first) = NULL;

            ++outstanding_;
            return first + sizeof(char *);
        } else {
            AllocatePlane_();
            return GetEntry_();
        }
    }

  public:
    SubAllocator(uintptr_t tag = 0, const Allocator &allocator = Allocator()) :
        allocator_(allocator),
        planes_(allocator),
        unit_(0),
        free_(NULL),
        outstanding_(0),
        tag_(tag)
    {
    }

    ~SubAllocator() {
        _assert(outstanding_ == 0);
        /*          ^^^^^^^^^^^^^^^^^
              If you hit this assert, then you have managed to
            leak an object allocated from this SubAllocator. The
            value of outstanding_ is the number of objects leaked.
        */

        if (outstanding_ != 0)
            LOG_THREAD() << "SubAllocator['" << be::TagToBuffer_(tag_) << "'] Leaked " << outstanding_ << " Objects (Size: " << unit_ << ")" << ios::NewLine;

        // XXX: this used to be automatic :(
        _tforeach (Planes_, plane, planes_)
            allocator_.DeallocateEx(*plane);
    }

    virtual void *Allocate(size_t size) {
        OPT_TIMER("ext::SubAllocator::Allocate()")

        if (unit_ != size) {
            if (unit_ == 0)
                unit_ = size;
            else {
                _assert(false);
                throw ext::NotImplementedException();
            }
        }

        return GetEntry_();
    }

    virtual void Deallocate(void *address) {
        if (address == NULL)
            return;

        OPT_TIMER("ext::SubAllocator::Deallocate()")

        char *entry(reinterpret_cast<char *>(address) - sizeof(char **));

        _assert(GetNext_(entry) == NULL);
        /*         ^^^^^^^^^^^^^^^^^^^^^^^
              If you hit this assert, then you have attempted to
            deallocate an object that is already on the free list.
        */

        ext::ScopeLock<Lock_> guard(lock_);
        GetNext_(entry) = free_;
        free_ = entry;

        --outstanding_;
    }

    uintptr_t GetTag() const {
        return tag_;
    }

    void SetTag(uintptr_t tag) {
        tag_ = tag;
    }

    unsigned GetOutstanding() const {
        return outstanding_;
    }
};

}

#endif//MENES_EXT_SUBALLOC_HPP
