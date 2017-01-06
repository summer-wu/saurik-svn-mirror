/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_EXT_SORTING_HPP
#define MENES_EXT_SORTING_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace ext {

template <typename Compare_, typename Type_, typename Iterator_>
void QuickSort(Iterator_ begin, size_t size) {
    const size_t middle(size / 2);
    size_t median;

    if (Compare_::Execute(begin[0], begin[middle]))
        if (Compare_::Execute(begin[middle], begin[size - 1]))
            median = middle;
        else if (Compare_::Execute(begin[0], begin[size - 1]))
            median = size - 1;
        else
            median = 0;
    else if (Compare_::Execute(begin[0], begin[size - 1]))
        median = 0;
    else if (Compare_::Execute(begin[middle], begin[size - 1]))
        median = size - 1;
    else
        median = middle;

    size_t i(0);
    size_t end(size);

    _forall (size_t, i, 0, end)
        if (Compare_::Execute(*begin[median], begin[i])) {
            Swap(begin[i], begin[--end]);
        }
}

template <typename Compare_, typename Type_, typename Iterator_>
void QuickSort(Iterator_ begin, Iterator_ end) {
    return QuickSort<Compare_>(begin, end - begin);
}

}

#endif//MENES_EXT_SORTING_HPP
