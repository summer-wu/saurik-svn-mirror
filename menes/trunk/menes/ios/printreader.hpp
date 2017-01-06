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

#ifndef MENES_IOS_PRINTREADER_HPP
#define MENES_IOS_PRINTREADER_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/traits.hpp"
#include "ios/streams.hpp"

namespace ios {

class PrintReader :
    public Reader
{
};

namespace be {
template <typename Type_>
void ParseInteger_(PrintReader &lhs, byte_t next, Type_ &rhs) {
    // XXX: if this doesn't use the last byte it should like... return it or something
    for (rhs = 0; next >= '0' && next <= '9';) {
        rhs = rhs * 10 + (next - '0');
        if (!lhs.Get(next))
            break;
    }
} }

template <typename Type_>
typename etl::IfTrue<etl::IsNotUnsigned<Type_>::value, PrintReader &>::Result
operator >>(PrintReader &lhs, Type_ &rhs) {
    be::ParseInteger_(lhs, ios::Get(lhs), rhs);
    return lhs;
}

template <typename Type_>
typename etl::IfTrue<etl::IsUnsigned<Type_>::value, PrintReader &>::Result
operator >>(PrintReader &lhs, Type_ &rhs) {
    byte_t next(ios::Get(lhs));
    bool negative(false);

    if (next == '-') {
        negative = true;
        next = ios::Get(lhs);
    }

    be::ParseInteger_(lhs, next, rhs);

#ifdef __ICC
#pragma warning(push)
// #68: integer conversion resulted in a change of sign
#pragma warning(disable: 68)
#endif

    if (negative)
        rhs *= -1;

#ifdef __ICC
#pragma warning(pop)
#endif

    return lhs;
}

IOS_DECLARE PrintReader &operator >>(PrintReader &lhs, float &rhs);
IOS_DECLARE PrintReader &operator >>(PrintReader &lhs, double &rhs);

}

#endif//MENES_IOS_PRINTREADER_HPP
