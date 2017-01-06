/* Menes Optimization - C++ Profiling and Verification
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "app/configuration.hpp"
#include "ios/printwriter.hpp"

#include "opt/debugged.hpp"
#include "opt/memory.hpp"

namespace opt {

namespace be {

void ZombieCall_(void *this_) {
    _assert(false);
    /*          ^^^^^
          Hitting this assert means you tried to call a method of an object that has already
        been deleted. I decided that VC++ was a little too harsh about that (to the point of
        making it difficult to debug what happened sometimes, or figure out which pointer it
        was that caused the problem), so I did some horrendous hacks and caused this function
        to be called instead. I hope you appreciate it :).
    */
}

void (*ZombieVTable_[])(void *) = {
    &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_,
    &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_,
    &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_,
    &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_,
    &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_,
    &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_, &ZombieCall_
};

}

void DebuggedAllocator::Allocate_(size_t size, be::Block_ *extra) {
    extra->size_ = size;

    if (!online_) {
        ext::ScopeLock<api::CriticalSection> guard(lock_);
        trackleaks_ = _C("menes/opt/memory/@track-leaks", false);
        stackdepth_ = _C("menes/opt/memory/@stack-depth", 0);
        catchzombies_ = _C("menes/opt/memory/@catch-zombies", false);
        online_ = true;
    }

    if (trackleaks_) {
        if (stackdepth_ != 0)
            extra->trace_.reset(new api::StackTrace(stackdepth_));

        ext::ScopeLock<api::CriticalSection> guard(lock_);
        extra->next = blocks_;
        extra->prev = &blocks_;

        if (extra->next != NULL)
            extra->next->prev = &extra->next;
        blocks_ = extra;
    }
}

void DebuggedAllocator::Deallocate_(be::Block_ *extra) {
    if (catchzombies_ && extra->size_ % sizeof(void *) == 0)
        ext::FillAssign(extra->GetBegin<void *>(), extra->GetEnd<void *>(), &be::ZombieVTable_);

    if (trackleaks_) {
        ext::ScopeLock<api::CriticalSection> guard(lock_);
        *extra->prev = extra->next;
        if (extra->next != NULL)
            extra->next->prev = extra->prev;
    }
}

DebuggedAllocator::DebuggedAllocator(const _R<ext::Allocator> &allocator) :
    Base_(_S<ext::ThunkedIndirectAllocator>(allocator)),
    blocks_(NULL),
    online_(false),
    busy_(false)
{
}

void DebuggedAllocator::Report(ios::PrintWriter &out) {
    ext::ScopeLock<api::CriticalSection> guard(lock_);

    ext::ScopeSet<LocalAllocator_, ext::Allocator *> allocator(opt::GetAllocatorIndirect(), &opt::GetGlobalMalloc());

    size_t total(0);
    unsigned leaks(0);

    for (be::Block_ *block(blocks_); block != NULL; block = block->next) {
        int hex(static_cast<int>(block->size_));
        char *data(block->GetBegin<char>());

        out << ios::NewLine << "--- " << block->size_ << " bytes ===============================" << ios::NewLine;

        total += block->size_;
        ++leaks;

        if (hex > 80)
            hex = 80;

        out << ios::SetFill('0') << ios::SetBase(16);

        for (unsigned b; hex > 0; hex -= 16, data += 16) {
            unsigned part(hex > 16 ? 16 : hex);

            out << ios::SetWidth(8) << reinterpret_cast<uintptr_t>(data) << ": ";
            for (b = 0; b < part; ++b)
                out << ios::SetWidth(2) << (data[b] & 0xff) << " ";
            for (; b < 16; ++b)
                out << "   ";
            out << " ";

            for (b = 0; b < part; ++b) {
                if (b == 8)
                    out << " ";
                ios::Put(out, data[b] < 32 ? '.' : data[b]);
            }

            out << ios::NewLine;
        }

        out << ios::SetFill(' ') << ios::SetBase(10);

        out << ios::NewLine;

        if (block->trace_.get() != NULL) {
            out << *block->trace_;
            out << ios::NewLine;
        }
    }

    if (leaks == 0)
        out << "No memory leaks detected." << ios::NewLine;
    else {
        out << ios::NewLine << "===============================" << ios::NewLine;
        out << ios::NewLine << " " << total << " bytes in " << leaks << " leak(s)" << ios::NewLine;
    }
}

}
