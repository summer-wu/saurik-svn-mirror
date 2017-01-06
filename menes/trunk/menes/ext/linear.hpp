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

#ifndef MENES_EXT_LINEAR_HPP
#define MENES_EXT_LINEAR_HPP

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

class LinearAllocator :
    public Allocator
{
  private:
    static const unsigned Size_ = 16384;

    struct Entry_ {
        Entry_ *last_;
        bool deleted_;

        Entry_(Entry_ *last) :
            last_(last),
            deleted_(false)
        {
        }
    };

    typedef ext::Vector<char *> Planes_;
    Planes_ planes_;

    size_t level_;
    size_t index_;

    Entry_ *last_;
    unsigned outstanding_;

  public:
    LinearAllocator() :
        level_(~size_t()),
        index_(0),
        last_(NULL),
        outstanding_(0)
    {
    }

    ~LinearAllocator() {
        _assert(outstanding_ == 0);
        /*          ^^^^^^^^^^^^^^^^^
              If you hit this assert, then you have managed to
            leak an object allocated from this LinearAllocator. The
            value of outstanding_ is the number of objects leaked.
        */

        if (outstanding_ != 0)
            LOG_THREAD() << "LinearAllocator Leaked " << outstanding_ << ios::NewLine;

        _foreach (Planes_, plane, planes_)
            delete [] *plane;
    }

    virtual void *Allocate(size_t size) {
        OPT_TIMER("ext::LinearAllocator::Allocate()")

        _assert(size < Size_);
        if (level_ == ~size_t() || Size_ - index_ < size + sizeof(Entry_)) {
            if (++level_ == planes_.GetSize())
                planes_.InsertLast(new char[Size_]);
            index_ = 0;
        }

        last_ = new (planes_[level_] + index_) Entry_(last_);
        index_ += etl::AlignValue(sizeof(Entry_) + size, sizeof(void *));

        ++outstanding_;
        return last_ + 1;
    }

    virtual void Deallocate(void *address) {
        if (address == NULL)
            return;

        OPT_TIMER("ext::LinearAllocator::Deallocate()")

        Entry_ *entry(reinterpret_cast<Entry_ *>(address) - 1);

        if (entry != last_)
            entry->deleted_ = true;
        else {
            Entry_ *previous;

            do {
                previous = last_;
                last_ = last_->last_;
            } while (last_ != NULL && last_->deleted_);

            char *local(reinterpret_cast<char *>(previous));

            _forall (size_t, plane, 0, planes_.GetSize())
                if (local >= planes_[plane] && local < planes_[plane] + Size_) {
                    level_ = plane;
                    index_ = local - planes_[plane];
                    break;
                }
        }

        --outstanding_;
    }
};

}

#endif//MENES_EXT_LINEAR_HPP
