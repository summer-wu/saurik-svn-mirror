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

#ifndef MENES_OPT_TRACKING_HPP
#define MENES_OPT_TRACKING_HPP

#include "cxx/platform.hpp"
#include "opt/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "alg/maximal.hpp"
#include "alg/logbase2.hpp"

#include "ext/allocatorex.hpp"
#include "ios/printwriter.hpp"

namespace opt {

class TrackingAllocator :
    public ext::ExtraAllocator< size_t, TrackingAllocator, ext::ThunkedIndirectAllocator >
{
    friend struct ext::ExtraAllocator< size_t, TrackingAllocator, ext::ThunkedIndirectAllocator >;

  private:
    typedef ext::ExtraAllocator< size_t, TrackingAllocator, ext::ThunkedIndirectAllocator > Base_;

    static const unsigned Exact_ = 128;

    struct Register_ {
        alg::Maximal<size_t> all_[1];
        alg::Maximal<size_t> log_[31];
        alg::Maximal<size_t> size_[Exact_];
    } used_, size_;

    size_t overhead_;

    // XXX: alg::Maximal has a horrible stupidity in it's +=/-= optimization that caused these two functions

    // XXX: I think this template requirement is a bug in gcc
    template <size_t Size_>
    _finline void Increase_(alg::Maximal<size_t> (Register_::*field)[Size_], size_t index, size_t size) {
        ++(used_.*field)[index];
        (size_.*field)[index] += size;
    }

    template <size_t Size_>
    _finline void Decrease_(alg::Maximal<size_t> (Register_::*field)[Size_], size_t index, size_t size) {
        --(used_.*field)[index];
        (size_.*field)[index] -= size;
    }

    void Allocate_(size_t size, size_t *extra);
    void Deallocate_(size_t *extra);

    template <size_t Size_>
    void Output_(ios::PrintWriter &out, size_t size, bool plus, alg::Maximal<size_t> (Register_::*field)[Size_], size_t index) {
        if ((used_.*field)[index].GetTotal() == 0)
            return;

        out << "| " << ios::SetWidth(10) << size << (plus ? "+" : " ") << " | ";

        out << ios::SetWidth(10) << (used_.*field)[index] << " | ";
        out << ios::SetWidth(10) << (used_.*field)[index].GetMaximum() << " | ";
        out << ios::SetWidth(10) << (used_.*field)[index].GetTotal() << " | ";

        out << ios::SetWidth(10) << (size_.*field)[index] << " | ";
        out << ios::SetWidth(10) << (size_.*field)[index].GetMaximum() << " | ";
        out << ios::SetWidth(10) << (size_.*field)[index].GetTotal() << " |" << ios::NewLine;
    }

  public:
    TrackingAllocator(size_t overhead = 0, const _R<ext::Allocator> &allocator = _R<ext::Allocator>());

    void Report(ios::PrintWriter &out);
};

}

#endif//MENES_OPT_TRACKING_HPP
