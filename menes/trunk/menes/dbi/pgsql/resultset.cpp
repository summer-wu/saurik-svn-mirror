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
#include "dbi/pgsql/error.hpp"
#include "dbi/pgsql/resultset.hpp"

#include "libpq-fe.h"

namespace dbi {
namespace PgSql {

ResultSet::ResultSet(const _R<Connection> &connection, PGresult *result) :
    connection_(connection),
    result_(result),
    row_(~unsigned()),
    rows_(::PQntuples(result_))
{
}

ResultSet::~ResultSet() {
    ::PQclear(result_);
}

cse::String ResultSet::GetNameByIndex(unsigned column) const {
    return ::PQfname(result_, column);
}

unsigned ResultSet::GetIndexByName(const cse::String &name) const {
    // XXX: returns -1 on failure
    return ::PQfnumber(result_, name.NullTerminate());
}

bool ResultSet::IsNull(unsigned column) const {
    return ::PQgetisnull(result_, row_, column) == 1;
}

_R<dbi::Blob> ResultSet::GetBlob(unsigned column) const {
    return new _H<Blob>(connection_, Get< ::Oid >(column));
}

cse::String ResultSet::GetString(unsigned column) const {
    return ::PQgetvalue(result_, row_, column);
}

bool ResultSet::MoveNext() {
    return row_ != rows_ && ++row_ != rows_;
}

} }
