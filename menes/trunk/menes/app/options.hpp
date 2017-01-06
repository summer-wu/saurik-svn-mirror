/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#ifndef MENES_APP_OPTIONS_HPP
#define MENES_APP_OPTIONS_HPP

#include "cxx/platform.hpp"
#include "app/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/lexical.hpp"
#include "cse/string.hpp"

#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/vector.hpp"

namespace app {

typedef ext::Vector<ext::Buffer> ArgumentList;

APP_DECLARE ArgumentList GetArguments();

namespace be {
    void SetArguments(const ArgumentList &arguments);
}

// XXX: should implement the ext::Map interface
// XXX: should actually work like a fucking ext::Map
class APP_DECLARE Options {
  private:
    ArgumentList args_;

    typedef ext::RedBlackMap<ext::Buffer, ext::Buffer> OptionMap_;
    OptionMap_ options_;

    typedef ext::ListSet<cse::CodePoint> SwitchSet_;
    SwitchSet_ extra_;

  public:
    Options();
    explicit Options(const ext::Buffer &extra);

    void Parse(const ArgumentList::ConstIterator &begin, const ArgumentList::ConstIterator &end);

    const ArgumentList &GetArguments() const;

    void RegisterExtra(const ext::Buffer &options);

    ext::Buffer GetOr(const ext::Buffer &option, const ext::Buffer &value) const {
        if (const ext::Buffer *arg = operator [](option))
            return *arg;
        return value;
    }

    template <typename Type_>
    Type_ GetAsOr(const ext::Buffer &option, const Type_ &value) const {
        if (const ext::Buffer *arg = operator [](option))
            // XXX: lexical_cast<>()
            return lexical_cast<Type_>(cse::String(*arg));
        else
            return value;
    }

    const ext::Buffer *operator [](const ext::Buffer &option) const;
    const ext::Buffer *operator [](unsigned index) const;
};

}

#endif//MENES_APP_OPTIONS_HPP
