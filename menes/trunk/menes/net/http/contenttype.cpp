/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#include "ios/helpers.hpp"
#include "net/http/contenttype.hpp"

namespace net {
namespace Http {

ContentType::ContentType() {
}

ContentType::ContentType(const cse::String &value) {
    _L<cse::String> values(ext::SplitAll(value, _B(";")));

    _foreach (_L<cse::String>, value, values) {
        cse::String trimmed(value->Trim<cse::IsWhiteSpace>());

        if (_index == 0) {
            cse::String::Iterator slash(ext::FindFirst(trimmed, '/'));
            _assert(slash != trimmed.End());

            type_ = cse::String(trimmed.Begin(), slash);
            subtype_ = cse::String(slash + 1, trimmed.End());
        } else {
            cse::String::Iterator equals(ext::FindFirst(trimmed, '='));
            _assert(equals != trimmed.End());

            cse::String name(cse::String(trimmed.Begin(), equals).Trim<cse::IsWhiteSpace>());
            cse::String value(cse::String(equals + 1, trimmed.End()).Trim<cse::IsWhiteSpace>());

            // XXX: technically this should be using "quoted-string" encoding filter
            parameters_[name] = value;
        }
    }
}

const cse::String &ContentType::GetType() const {
    return type_;
}

const cse::String &ContentType::GetSubtype() const {
    return subtype_;
}

const ContentType::Parameters_ &ContentType::GetParameters() const {
    return parameters_;
}

} }
