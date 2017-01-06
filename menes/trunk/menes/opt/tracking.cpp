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

#include "opt/tracking.hpp"

namespace opt {

void TrackingAllocator::Allocate_(size_t size, size_t *extra) {
    _assert(overhead_ <= size);
    size -= overhead_;
    *extra = size;

    Increase_(&Register_::all_, 0, size);
    if (size < Exact_)
        Increase_(&Register_::size_, size, size);
    else
        Increase_(&Register_::log_, alg::ilog2(size), size);

    //_assert(false);
}

void TrackingAllocator::Deallocate_(size_t *extra) {
    size_t size(*extra);

    Decrease_(&Register_::all_, 0, size);
    if (size < Exact_)
        Decrease_(&Register_::size_, size, size);
    else
        Decrease_(&Register_::log_, alg::ilog2(size), size);
}

TrackingAllocator::TrackingAllocator(size_t overhead, const _R<ext::Allocator> &allocator) :
    Base_(_S<ext::ThunkedIndirectAllocator>(allocator)),
    overhead_(overhead)
{
}

void TrackingAllocator::Report(ios::PrintWriter &out) {
    out << "+-------------+------------+------------+------------+------------+------------+------------+" << ios::NewLine;
    out << "  Lower Bound |  End Used     Max Used    Total Used |  End Size     Max Size    Total Size  " << ios::NewLine;
    out << "+-------------+------------+------------+------------+------------+------------+------------+" << ios::NewLine;

    out << "|      <All>  | ";

    out << ios::SetWidth(10) << used_.all_[0] << " | ";
    out << ios::SetWidth(10) << used_.all_[0].GetMaximum() << " | ";
    out << ios::SetWidth(10) << used_.all_[0].GetTotal() << " | ";

    out << ios::SetWidth(10) << size_.all_[0] << " | ";
    out << ios::SetWidth(10) << size_.all_[0].GetMaximum() << " | ";
    out << ios::SetWidth(10) << size_.all_[0].GetTotal() << " |" << ios::NewLine;

    out << "+-------------+------------+------------+------------+------------+------------+------------+" << ios::NewLine;

    for (size_t i(0); i != Exact_; ++i)
        Output_(out, i, false, &Register_::size_, i);

    out << "+-------------+------------+------------+------------+------------+------------+------------+" << ios::NewLine;

    for (size_t i(alg::ilog2(Exact_)); i != 31; ++i)
        Output_(out, 1 << i, true, &Register_::log_, i);

    out << "+-------------+------------+------------+------------+------------+------------+------------+" << ios::NewLine;
}

}
