/* Menes Database - Database Abstraction Layer
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *·
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
 *·
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

#ifndef MENES_DBI_RESULTSET_HPP
#define MENES_DBI_RESULTSET_HPP

#include "cxx/platform.hpp"
#include "dbi/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/lexical.hpp"
#include "cse/string.hpp"

#include "ext/refcount.hpp"

namespace dbi {

class Connection;
EXT_HANDLESTUB(class Blob)

class ResultSet :
    public ext::ReferenceCounted
{
  public:
    virtual cse::String GetNameByIndex(unsigned column) const = 0;
    virtual unsigned GetIndexByName(const cse::String &name) const = 0;

    virtual bool IsNull(unsigned column) const = 0;

    virtual bool IsNull(const cse::String &column) const {
        return IsNull(GetIndexByName(column));
    }

    virtual _R<Blob> GetBlob(unsigned column) const = 0;
    virtual cse::String GetString(unsigned column) const = 0;

    virtual cse::String GetString(const cse::String &column) const {
        return GetString(GetIndexByName(column));
    }

    virtual bool MoveNext() = 0;

    template <typename Type_>
    Type_ Get(unsigned column) const {
        return lexical_cast<Type_>(GetString(column));
    }

    template <typename Type_>
    Type_ Get(const cse::String &column) const {
        return lexical_cast<Type_>(GetString(GetIndexByName(column)));
    }

    cse::String GetString() {
        _assert(MoveNext());
        return GetString(0);
    }

    template <typename Type_>
    Type_ Get() {
        return lexical_cast<Type_>(GetString());
    }
};

}

#endif//MENES_DBI_RESULTSET_HPP
