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

#ifndef MENES_CSE_UTF16_HPP
#define MENES_CSE_UTF16_HPP

#include "cxx/platform.hpp"
#include "cse/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/encoding.hpp"
#include "ios/getput.hpp"

namespace ext {
    class InvalidArgumentException;
}

namespace cse {

struct Utf16Encoding :
    public Encoding<Utf16Encoding>
{
    static const unsigned MaxUnits = 2;

#ifdef _MSC_VER
    typedef wchar_t CodeUnit;
#else
    typedef uint16_t CodeUnit;
#endif

    template <typename Iterator_>
    static _finline Iterator_ Encode(CodePoint value, Iterator_ &out) {
        if (value < 0x10000)
            ios::Put(out, value);
        else {
            ios::Put(out, (value - 0x10000) / 0x400 + 0xd800);
            ios::Put(out, (value - 0x10000) % 0x400 + 0xdc00);
        }

        return out;
    }

    template <typename Iterator_>
    static _finline void Skip(Iterator_ &i) {
        i += (*i >= 0xd800 && *i < 0xe000 ? 2 : 1);
    }

    template <typename Iterator_>
    static _finline void Retreat(Iterator_ &i) {
        do --i;
        while (*i >= 0xd800 && *i < 0xe000);
    }

    template <typename Iterator_>
    static _finline cse::CodePoint Advance(Iterator_ &i, size_t *size = NULL) {
        uint32_t value(ios::Get(i));
        if (value < 0xd800 || value >= 0xdc00)
            return value;
        else
            return (value - 0xd800) * 0x400 + (ios::Get(i) - 0xdc00) + 0x10000;
    }
};

}

#endif//MENES_CSE_UTF16_HPP
