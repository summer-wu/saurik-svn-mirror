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

#ifndef MENES_OPT_GARBAGE_HPP
#define MENES_OPT_GARBAGE_HPP

#include "cxx/platform.hpp"
#include "opt/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/alignment.hpp"
#include "etl/bind.hpp"

#include "ext/allocator.hpp"
#include "ext/stack.hpp"

#include "opt/profile.hpp"

namespace opt {

struct Range {
    void *begin_;
    void *end_;

    Range() :
        begin_(NULL),
        end_(NULL)
    {
    }

    Range(void *begin, void *end) :
        begin_(begin),
        end_(end)
    {
    }

    template <typename Type_>
    _finline Type_ *GetBegin() {
        return reinterpret_cast<Type_ *>(begin_);
    }

    template <typename Type_>
    _finline Type_ *GetEnd() {
        return reinterpret_cast<Type_ *>(end_);
    }

    _finline bool Contains(void *address) const {
        return begin_ <= address && address < end_;
    }
};

template <typename Allocator_ = ext::NewAllocator>
class GarbageCollector {
  private:
    struct Region_ {
        Allocator_ allocator_;
        bool marked_;

        Range range_;
        bool scanned_;

        Region_(const Allocator_ &allocator = Allocator_()) :
            allocator_(allocator),
            scanned_(false)
        {
        }

        Region_(void *begin, void *end, bool scanned, const Allocator_ &allocator = Allocator_()) :
            allocator_(allocator),
            range_(begin, end),
            scanned_(scanned)
        {
        }
    };

    typedef ext::TypedAllocator<ext::MallocAllocator> Malloc_;

    typedef ext::Vector< Region_, ext::Block<Region_, Malloc_> > RegionSet_;
    RegionSet_ regions_;

    typedef ext::Vector< Range, ext::Block<Range, Malloc_> > DirectSet_;
    DirectSet_ directs_;

    typedef ext::Vector< void *, ext::Block<void *, Malloc_> > IndirectSet_;
    IndirectSet_ indirects_;

    void Mark_(void *address, ext::Stack<Range> &ranges) {
        OPT_TIMER("opt::GarbageCollector::Mark_()")

        for (typename RegionSet_::Iterator region(regions_.Begin()); region != regions_.End(); ++region)
            if (!region->marked_ && region->range_.Contains(address)) {
                region->marked_ = true;
                if (region->scanned_)
                    ranges.Push(region->range_);
            }
    }

  public:
    void Insert(void *begin, void *end, bool scanned = true, const Allocator_ &allocator = Allocator_()) {
        OPT_TIMER("opt::GarbageCollector::Insert()")

        Region_ region(begin, end, scanned, allocator);
        regions_.InsertLast(region);
    }

    void Remove(void *begin, void *end) {
        OPT_TIMER("opt::GarbageCollector::Remove()")

        for (typename RegionSet_::Iterator region(regions_.Begin()); region != regions_.End(); ++region)
            if (region->range_.begin_ == begin /* XXX: && region->range_.end_*/) {
                regions_.RemoveAt(region);
                return;
            }

        _assert(false);
    }

    void Collect() {
        OPT_TIMER("opt::GarbageCollector::Collect()")

        ext::Stack<Range> ranges_;

        {
            OPT_TIMER("opt::GarbageCollector::Collect()[Clear]")
            for (typename RegionSet_::Iterator region(regions_.Begin()); region != regions_.End(); ++region)
                region->marked_ = false;
        }

        {
            OPT_TIMER("opt::GarbageCollector::Collect()[Direct]")
            for (typename DirectSet_::Iterator direct(directs_.Begin()); direct != directs_.End(); ++direct)
                ranges_.Push(*direct);
        }

        {
            OPT_TIMER("opt::GarbageCollector::Collect()[Indir.]")
            for (typename IndirectSet_::Iterator indirect(indirects_.Begin()); indirect != indirects_.End(); ++indirect)
                Mark_(*indirect, ranges_);
        }

        {
            OPT_TIMER("opt::GarbageCollector::Collect()[Mark]")
            for (Range range; ranges_.Pop(range); )
                for (void **address = etl::AlignPointer(range.template GetBegin<void *>()); address < range.template GetEnd<void *>(); ++address)
                    Mark_(*address, ranges_);
        }

        {
            OPT_TIMER("opt::GarbageCollector::Collect()[Sweep]")

            bool sweeping(false);

            do for (typename RegionSet_::Iterator region(regions_.Begin()); region != regions_.End(); )
                if (region->marked_)
                    ++region;
                else {
                    region->allocator_.Deallocate(region->range_.begin_);
                    sweeping = true;
                    break;
               }
            while (ext::GetAndSet(sweeping, false));
        }
    }
};

}

#endif//MENES_OPT_GARBAGE_HPP
