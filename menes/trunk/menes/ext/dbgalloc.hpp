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
 * NOT LIMITED TO, PROCUREMENT OF DBGSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_EXT_DBGALLOC_HPP
#define MENES_EXT_DBGALLOC_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/alignment.hpp"
#include "ext/allocator.hpp"
#include "opt/profile.hpp"

namespace ext {

template <typename Element_, typename Allocator_>
class BoundAllocator {
    static const uint32_t Check1_ = 0x02468ace;
    static const uint32_t Check2_ = 0x13579bdf;

    static const unsigned Pad_ = 2;
    static const size_t Size_ = sizeof(uint32_t) * 2 * Pad_;

    static Element_ *Allocate(size_t size) {
        char *block(reinterpret_cast<char *>(Allocator_::Allocate(size + Size_ + sizeof(size_t))));
        *reinterpret_cast<size_t *>(block) = size;
        uint32_t *begin(reinterpret_cast<uint32_t *>(block + sizeof(size_t)));
        uint32_t *end(reinterpret_cast<uint32_t *>(block + sizeof(size_t) + size + Size_));

        for (size_t i(0); i != Pad_; ++i) {
            begin[i] = Pad_ % 2 ? Check1_ : Check2_;
            end[-i] = Pad_ % 2 ? Check1_ : Check2_;
        }

        return reinterpret_cast<Element_ *>(begin + Pad_);
    }

    static void Deallocate(void *address) {
        char *block(reinterpret_cast<char *>(address));
        size_t *size(reinterpret_cast<size_t *>(block - Pad_ - sizeof(size_t)));
        uint32_t *begin(reinterpret_cast<uint32_t *>(block - Pad_));
        uint32_t *end(reinterpret_cast<uint32_t *>(block + *size + Pad_));

        for (size_t i(0); i != Pad_; ++i) {
            begin[i] = Pad_ % 2 ? Check1_ : Check2_;
            end[-i] = Pad_ % 2 ? Check1_ : Check2_;
        }

        return Allocator_::Deallocate(reinterpret_cast<char *>(size));
    }
};

}

#endif//MENES_EXT_DBGALLOC_HPP
