/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_OPT_DEBUGGED_HPP
#define MENES_OPT_DEBUGGED_HPP

#include "cxx/platform.hpp"
#include "opt/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/exename.hpp"
#include "api/locks.hpp"

#include "ext/allocatorex.hpp"

#include <memory>

namespace opt {

namespace be {
    struct Block_ {
        size_t size_;

        Block_ **prev;
        Block_ *next;

        std::auto_ptr<api::StackTrace> trace_;

        _finline size_t GetSize() {
            return size_;
        }

        template <typename Type_>
        _finline Type_ *GetBegin() {
            return reinterpret_cast<Type_ *>(this + 1);
        }

        template <typename Type_>
        _finline Type_ *GetEnd() {
            return reinterpret_cast<Type_ *>(GetBegin<char>() + size_);
        }
    };
}

class DebuggedAllocator :
    public ext::ExtraAllocator< be::Block_, DebuggedAllocator, ext::ThunkedIndirectAllocator >
{
    friend struct ext::ExtraAllocator< be::Block_, DebuggedAllocator, ext::ThunkedIndirectAllocator >;

  private:
    typedef ext::ExtraAllocator< be::Block_, DebuggedAllocator, ext::ThunkedIndirectAllocator > Base_;

    api::CriticalSection lock_;

    be::Block_ *blocks_;

    bool online_;
    bool busy_;

    bool trackleaks_;
    int stackdepth_;
    bool catchzombies_;

    void Allocate_(size_t size, be::Block_ *extra);
    void Deallocate_(be::Block_ *extra);

  public:
    DebuggedAllocator(const _R<ext::Allocator> &allocator);

    void Report(ios::PrintWriter &out);
};

}

#endif//MENES_OPT_DEBUGGED_HPP
