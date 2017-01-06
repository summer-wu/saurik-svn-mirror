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

#ifndef MENES_CSE_UTF8_HPP
#define MENES_CSE_UTF8_HPP

#include "cxx/platform.hpp"
#include "cse/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cxx/for.hpp"
#include "cse/encoding.hpp"
#include "ios/getput.hpp"

namespace ext {
    class InvalidArgumentException;
}

namespace cse {

struct Utf8Encoding :
    public Encoding<Utf8Encoding>
{
    static const unsigned MaxUnits = 4;

    typedef byte_t CodeUnit;

    template <typename Iterator_>
    static _finline Iterator_ Encode(CodePoint value, Iterator_ &out) {
        if (value < 0x80)
            ios::Put(out, value);
        else {
            // XXX: this overload resolution thing is stupid
            unsigned levels(alg::ilog2(value) / 6);
            ios::Put(out, 0xff << 8 - levels - 1 | value >> levels * 6);
            _forall (unsigned, i, 0, levels)
                ios::Put(out, value >> (levels - i - 1) * 6 & 0x3f | 0x80);
        }

        return out;
    }

    template <typename Iterator_>
    static _finline void Skip(Iterator_ &i) {
        CodeUnit first(ios::Get(i));

        if ((first & 0x80) != 0) {
            size_t total(1);
            do ++i;
            while ((first & (0x80 >> ++total)) != 0);
        }
    }

    template <typename Iterator_>
    static _finline void Retreat(Iterator_ &i) {
        while ((*--i & 0xc0) == 0x80);
    }

    template <typename Iterator_>
    static _finline cse::CodePoint Advance(Iterator_ &i, size_t *size = NULL) {
        size_t total(1);

        CodeUnit first(ios::Get(i));
        if ((first & 0x80) != 0)
            while ((first & (0x80 >> ++total)) != 0);

        if (size != NULL)
            if (total > *size)
                throw ext::InvalidArgumentException();
            else
                *size -= total;

        uint32_t value(first & (0xff >> total));
        while (--total != 0)
            value = (value << 6) | (ios::Get(i) & ~0xc0);

        return value;
    }
};

}

#endif//MENES_CSE_UTF8_HPP
