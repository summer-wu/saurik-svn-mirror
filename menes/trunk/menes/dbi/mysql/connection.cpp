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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "dbi/mysql/connection.hpp"
#include "dbi/mysql/error.hpp"
#include "dbi/mysql/resultset.hpp"
#include "dbi/mysql/statement.hpp"

namespace dbi {
namespace MySql {

Connection::Connection(const cse::String &hostname, const cse::String &username, const cse::String &password, const cse::String &dbname) {
    MYSQL *result;
    result = ::mysql_init(&conn_);
    _assert(result == &conn_);

    result = ::mysql_real_connect(
        &conn_,
        hostname.IsEmpty() ? NULL : hostname.NullTerminate(),
        username.IsEmpty() ? NULL : username.NullTerminate(),
        password.IsEmpty() ? NULL : password.NullTerminate(),
        dbname.IsEmpty() ? NULL : dbname.NullTerminate(),
        0,
        NULL,
        0
    );

    if (result != NULL)
        _assert(result == &conn_);
    else {
        throw ext::StringException();
    }
}

Connection::~Connection() {
    ::mysql_close(&conn_);
}

_R<dbi::ResultSet> Connection::Execute(const cse::String &statement) {
    if (0 != ::mysql_real_query(&conn_, statement.GetData().Begin(), statement.GetData().GetSize()))
        throw Error(&conn_);

    ::MYSQL_RES *result(::mysql_store_result(&conn_));
    if (result == NULL) {
        if (::mysql_field_count(&conn_) == 0)
            return _R<dbi::ResultSet>();
        else
            throw Error(&conn_);
    }

    return new _H<ResultSet>(result);
}

_R<dbi::Statement> Connection::Parse(const cse::String &statement) {
    // XXX: this is only supported by bleeding-edge MySql
    return new _H<Statement>(this, statement);
}

_R<dbi::Blob> Connection::CreateBlob() {
    throw ext::NotImplementedException();
}

cse::String Connection::Escape(const ext::Buffer &value) const {
    char *escaped_(new char[value.GetSize() * 2 + 1]);
    ::mysql_real_escape_string(const_cast<MYSQL *>(&conn_), escaped_, value.Begin(), value.GetSize());
    ext::Buffer escaped(escaped_, ::strlen(escaped_));
    delete [] escaped_;
    return escaped;
}

cse::String Connection::Escape(const cse::String &value) const {
    return Escape(value.GetData());
}

void Connection::Begin() {
    Execute("BEGIN");
}

void Connection::Commit() {
    Execute("COMMIT");
}

void Connection::Abort() {
    Execute("ROLLBACK");
}

cse::String Connection::Save() {
    throw ext::NotImplementedException();
}

void Connection::Commit(const cse::String &point) {
    throw ext::NotImplementedException();
}

void Connection::Abort(const cse::String &point) {
    throw ext::NotImplementedException();
}

::MYSQL *Connection::GetHandle() {
    return &conn_;
}

} }
