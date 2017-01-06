/* Menes Database - Database Abstraction Layer
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#ifndef MENES_DBI_MYSQL_CONNECTION_HPP
#define MENES_DBI_MYSQL_CONNECTION_HPP

#include "cxx/platform.hpp"
#include "dbi/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "dbi/connection.hpp"

#include "mysql_version.h"
#include "mysql.h"
#include "dbi/mysql/undefine.hpp"

namespace dbi {
namespace MySql {

class Connection :
    public dbi::Connection
{
  private:
    ::MYSQL conn_;

  public:
    Connection(const cse::String &hostname, const cse::String &username, const cse::String &password, const cse::String &db);
    ~Connection();

    virtual _R<dbi::ResultSet> Execute(const cse::String &statement);
    virtual _R<dbi::Statement> Parse(const cse::String &statement);
    virtual _R<dbi::Blob> CreateBlob();

    virtual cse::String Escape(const ext::Buffer &value) const;
    virtual cse::String Escape(const cse::String &value) const;

    virtual void Begin();
    virtual void Commit();
    virtual void Abort();

    virtual cse::String Save();
    virtual void Commit(const cse::String &point);
    virtual void Abort(const cse::String &point);

    ::MYSQL *GetHandle();
};

} }

#endif//MENES_DBI_MYSQL_CONNETION_HPP
