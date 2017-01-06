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

#ifndef MENES_DBI_CONNECTION_HPP
#define MENES_DBI_CONNECTION_HPP

#include "cxx/platform.hpp"
#include "dbi/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "dbi/statement.hpp"

#include "ext/refcount.hpp"
#include "cse/string.hpp"
#include "ext/types.hpp"

#include "ios/types.hpp"

namespace dbi {

class Blob;
EXT_HANDLESTUB(class ResultSet)

class Connection :
    public ext::ReferenceCounted
{
  public:
    virtual _R<ResultSet> Execute(const cse::String &statement) = 0;
    virtual _R<Statement> Parse(const cse::String &statement) = 0;
    virtual _R<Blob> CreateBlob() = 0;

    virtual cse::String Escape(const ext::Buffer &value) const = 0;
    virtual cse::String Escape(const cse::String &value) const = 0;

    virtual void Begin() = 0;
    virtual void Commit() = 0;
    virtual void Abort() = 0;

    virtual cse::String Save() = 0;
    virtual void Commit(const cse::String &point) = 0;
    virtual void Abort(const cse::String &point) = 0;
};

class Transaction {
  private:
    Connection &connection_;
    bool cancelled_;

  public:
    _finline Transaction(Connection &connection) :
        connection_(connection),
        cancelled_(false)
    {
        connection_.Begin();
    }

    _finline ~Transaction() {
        if (cancelled_ || std::uncaught_exception())
            connection_.Abort();
        else
            connection_.Commit();
    }

    void Cancel() {
        cancelled_ = true;
    }
};

class Savepoint {
  private:
    Connection &connection_;
    bool cancelled_;
    cse::String point_;

  public:
    _finline Savepoint(Connection &connection) :
        connection_(connection),
        cancelled_(false)
    {
        point_ = connection_.Save();
    }

    _finline ~Savepoint() {
        if (cancelled_ || std::uncaught_exception())
            connection_.Abort(point_);
        else
            connection_.Commit(point_);
    }

    void Cancel() {
        cancelled_ = true;
    }
};

}

#endif//MENES_DBI_CONNETION_HPP
