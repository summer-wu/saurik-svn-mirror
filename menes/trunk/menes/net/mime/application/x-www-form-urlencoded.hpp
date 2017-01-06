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

#ifndef MENES_NET_MIME_APPLICATION_X_WWW_FORM_URLENCODED_HPP
#define MENES_NET_MIME_APPLICATION_X_WWW_FORM_URLENCODED_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/console.hpp"

#include "ext/map.hpp"
#include "ext/redblackset.hpp"

#include "ios/filter.hpp"
#include "ios/helpers.hpp"
#include "ios/reader.hpp"

#include "net/http/queryencode.hpp"
#include "net/mime/document.hpp"

namespace net {
namespace Mime {
namespace Application {

class XWwwFormUrlencoded :
    public net::Mime::Document,
    public ext::RedBlackMap< cse::String, _L<cse::String> >
{
  private:
    void Parse_(const cse::String &value) {
        if (!value.IsEmpty())
            _foreach (const _L<cse::String>, pair, ext::SplitAll(value, _B("&"))) {
                cse::String::ConstIterator equal(ext::FindFirst(*pair, '='));
                cse::String name(pair->Begin(), equal);
                cse::String value(equal + 1, pair->End());

                name = ios::Filter<net::Http::QueryDecoder>(name);
                value = ios::Filter<net::Http::QueryDecoder>(value);

                operator [](name).InsertLast(value);
            }
    }

  public:
    explicit XWwwFormUrlencoded(const cse::String &value) {
        Parse_(value);
    }

    explicit XWwwFormUrlencoded(ios::Reader &reader) {
        Parse_(ios::ReadAll(reader));
    }
};

} } }

#endif//MENES_NET_MIME_APPLICATION_X_WWW_FORM_URLENCODED_HPP
