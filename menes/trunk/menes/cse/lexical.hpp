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
 * LIABILITY, WHETHER IN CASTS, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_CSE_LEXICAL_HPP
#define MENES_CSE_LEXICAL_HPP

#include "cxx/platform.hpp"
#include "cse/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/traits.hpp"

#include "ext/exception.hpp"
#include "cse/string.hpp"

#include "ios/buffer.hpp"

namespace be {

template <typename Target_, typename Source_, bool IsSame_ = etl::IsSame<Target_, Source_>::value>
struct LexicalHelper;

template <typename Target_>
struct LexicalHelper<Target_, cse::String, false> {
    static Target_ Cast(const cse::String &source) {
        _S<ios::Buffer> buffer(source);
        Target_ target;

        try {
            buffer >> target;
        } catch (const ext::EosException &) {
            throw ext::InvalidArgumentException();
        }

        if (!buffer.IsEmpty())
            throw ext::InvalidArgumentException();

        return target;
    }
};

template <typename Source_>
struct LexicalHelper<cse::String, Source_, false> {
    static cse::String Cast(const Source_ &source) {
        return _S<ios::Buffer>() << source;
    }
};

// XXX: this is because I can't stream bools yet
template <>
struct LexicalHelper<cse::String, bool, false> {
    static cse::String Cast(const bool &source) {
        return source ? _B("true") : _B("false");
    }
};

template <>
struct LexicalHelper<bool, cse::String, false> {
    static bool Cast(const cse::String &source) {
        if (
            source == _B("true") ||
            source == _B("1")
        )
            return true;
        else if (
            source == _B("false") ||
            source == _B("0")
        )
            return false;
        else
            throw ext::InvalidArgumentException();
    }
};

}

template <typename Target_, typename Source_>
Target_ lexical_cast(const Source_ &source) {
    return be::LexicalHelper<Target_, Source_>::Cast(source);
}

#endif//MENES_CSE_LEXICAL_HPP
