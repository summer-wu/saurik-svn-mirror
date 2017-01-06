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

#include "dbi/pgsql/blob.hpp"
#include "dbi/pgsql/connection.hpp"
#include "dbi/pgsql/error.hpp"
#include "dbi/pgsql/resultset.hpp"
#include "dbi/pgsql/statement.hpp"

namespace dbi {
namespace PgSql {

EXT_HANDLESHIM(Connection)

Connection::Connection(const cse::String &username, const cse::String &dbname) :
    point_(~unsigned())
{
    conn_ = ::PQconnectdb((_S<ios::String>() << "dbname = " << dbname << " user = " << username).NullTerminate());
    if (::PQstatus(conn_) != CONNECTION_OK) {
        cse::String error(::PQerrorMessage(conn_));
        ::PQfinish(conn_);
        throw ext::StringException(error);
    }
}

Connection::~Connection() {
    ::PQfinish(conn_);
}

_R<dbi::ResultSet> Connection::Execute(const cse::String &statement) {
    OPT_TIMER("dbi::PgSql::Connection::Execute()")

    ::PGresult *result(::PQexec(conn_, statement.NullTerminate()));

    ExecStatusType err(::PQresultStatus(result));
    if (err == PGRES_COMMAND_OK)
        return _R<dbi::ResultSet>();
    else if (err == PGRES_TUPLES_OK)
        return new _H<ResultSet>(this, result);
    else {
        cse::String error(::PQresultErrorMessage(result));
        ::PQclear(result);
        throw ext::StringException(error);
    }
}

_R<dbi::Statement> Connection::Parse(const cse::String &statement) {
    return new _H<Statement>(this, statement);
}

_R<dbi::Blob> Connection::CreateBlob() {
    return new _H<Blob>(this);
}

cse::String Connection::Escape(const ext::Buffer &value) const {
    OPT_TIMER("dbi::PgSql::Connection::Escape(ext::Buffer)")

    size_t length;
    uint8_t *data(::PQescapeBytea(reinterpret_cast<const uint8_t *>(value.Begin()), value.GetSize(), &length));
    _assert(data[length - 1] == 0);
    // XXX: technically I should be able to share this memory and return it
    ext::Buffer escaped(reinterpret_cast<char *>(data), length - 1);
    // XXX: this should be wrapped into an allocator
    ::PQfreemem(data);
    return escaped;
}

cse::String Connection::Escape(const cse::String &value) const {
    OPT_TIMER("dbi::PgSql::Connection::Escape(cse::String)")

    ext::Buffer escaped(value.GetSize() * 2 + 1);
    escaped.SetSize(::PQescapeString(escaped.Begin(), value.GetData().Begin(), value.GetData().GetSize()));
    return escaped;
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
    _S<ios::String> point;
    point << "save" << ++point_;
    Execute(_S<ios::String>() << "SAVEPOINT " << point);
    return point;
}

void Connection::Commit(const cse::String &point) {
    Execute(_S<ios::String>() << "RELEASE SAVEPOINT " << point);
}

void Connection::Abort(const cse::String &point) {
    Execute(_S<ios::String>() << "ROLLBACK TO SAVEPOINT " << point);
}

PGconn *Connection::GetHandle() const {
    return conn_;
}

} }
