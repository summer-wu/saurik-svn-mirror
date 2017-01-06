/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "dbi/pgsql/blob.hpp"
#include "dbi/pgsql/connection.hpp"
#include "dbi/pgsql/statement.hpp"

#include "ext/list.hpp"
#include "ext/uuid.hpp"

#include "ios/buffer.hpp"

namespace dbi {
namespace PgSql {

Statement::Statement(const _R<Connection> &connection, const cse::String &statement) :
    connection_(connection),
    statement_(ext::SplitAll(statement, _B("?"))),
    arguments_(statement_.GetSize() - 1)
{
}

void Statement::Set(unsigned index, const ext::Buffer &value) {
    arguments_[index] = (_S<ios::Buffer>() << "'" << connection_->Escape(value) << "'");
}

void Statement::Set(unsigned index, int64_t value) {
    arguments_[index] = lexical_cast<cse::String>(value);
}

void Statement::Set(unsigned index, bool value) {
    arguments_[index] = value ? _B("true") : _B("false");
}

void Statement::Set(unsigned index, const cse::String &value) {
    arguments_[index] = (_S<ios::Buffer>() << "'" << connection_->Escape(value) << "'");
}

void Statement::Set(unsigned index, const ext::Uuid &value) {
    Set(index, lexical_cast<cse::String>(value));
}

void Statement::Set(unsigned index, const dbi::Blob &value) {
    Set(index, dynamic_cast<const dbi::PgSql::Blob &>(value).GetId());
}

void Statement::Set(unsigned index, const Null &value) {
    arguments_[index] = _B("null");
}

_R<dbi::ResultSet> Statement::Execute() const {
    _S<ios::Buffer> statement;
    for (size_t i(0); i != arguments_.GetSize(); ++i)
        statement << statement_[i] << arguments_[i];
    statement << statement_.Last();
    return connection_->Execute(statement);
}

} }
