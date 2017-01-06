/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
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

#ifndef NINETJER_UTILS_HPP
#define NINETJER_UTILS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/getput.hpp"

namespace clr {

inline uint32_t MakeToken(uint32_t table, uint32_t row) {
    _assert((table & ~0xff) == 0);
    _assert((row & ~0xffffff) == 0);
    return (table << 24) | row;
}

inline uint32_t NilToken(uint32_t table) {
    _assert((table & ~0xff) == 0);
    return table << 24;
}

inline uint32_t TokenTable(uint32_t token) {
    return token >> 24;
}

inline uint32_t TokenRow(uint32_t token) {
    return token & 0x00ffffff;
}

template <typename Type_, typename Reader_>
inline Type_ Uncompress(Reader_ &data) {
    uint32_t b1(ios::Get(data));
    if ((b1 & 0xf0) == 0xf0) {
        _assert(false);
        return static_cast<Type_>(0);
    } else if ((b1 & 0xe0) == 0xe0) {
        _assert(false);
        return static_cast<Type_>(0);
    } else if ((b1 & 0xc0) == 0xc0) {
        uint32_t b2(ios::Get(data));
        uint32_t b3(ios::Get(data));
        uint32_t b4(ios::Get(data));
        return static_cast<Type_>((b1 & ~0xc0) << 24 | b2 << 16 | b3 << 8 | b4);
    } else if ((b1 & 0x80) == 0x80) {
        return static_cast<Type_>((b1 & ~0x80) << 8 | ios::Get(data));
    } else {
        return static_cast<Type_>(b1);
    }
}

}

#endif//NINETJER_UTILS_HPP
