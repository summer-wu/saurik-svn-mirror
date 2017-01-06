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

#ifndef MENES_NET_HTTP_URI_HPP
#define MENES_NET_HTTP_URI_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/pcre/regex.hpp"
#include "ios/printwriter.hpp"

namespace net {
namespace Http {

class Uri {
  private:
    cse::String uri_;

    cse::String scheme_;
    cse::String authority_;

    _L<cse::String> paths_;

    cse::String query_;
    cse::String fragment_;

  public:
    Uri() {
    }

    Uri(const cse::String &uri) :
        uri_(uri)
    {
        static api::Pcre::RegEx regex("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");

        api::Pcre::RegEx::Match match(regex(uri));
        _assert(match);

        scheme_ = match[2];
        authority_ = match[4];
        query_ = match[7];
        fragment_ = match[9];

        cse::String path(match[5]);
        paths_ = ext::SplitAll(path, _B("/"));
    }

    const cse::String &GetUri() const {
        return uri_;
    }

    const cse::String &GetScheme() const {
        return scheme_;
    }

    const cse::String &GetAuthority() const {
        return authority_;
    }

    const _L<cse::String> &GetPaths() const {
        return paths_;
    }

    const cse::String &GetQuery() const {
        return query_;
    }

    const cse::String &GetFragment() const {
        return fragment_;
    }
};

inline ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const Uri &rhs) {
    return lhs << rhs.GetUri();
}

} }

#endif//MENES_NET_HTTP_URI_HPP
