/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#include "dbi/mysql/connection.hpp"
#include "dbi/mysql/error.hpp"
#include "dbi/mysql/resultset.hpp"
#include "dbi/mysql/statement.hpp"

// XXX: this is only supported by bleeding-edge MySql
namespace dbi {
namespace MySql {

Statement::Statement(const _R<Connection> &connection, const cse::String &statement) :
    connection_(connection)
{
    statement_ = ::mysql_stmt_init(connection_->GetHandle());
    if (statement_ == NULL)
        throw Error(connection_->GetHandle());

    if (::mysql_stmt_prepare(statement_, statement.GetData().Begin(), statement.GetData().GetSize()) != 0)
        throw Error(connection_->GetHandle());

    binds_.Reallocate(::mysql_stmt_param_count(statement_));
    // XXX: check the error on this puppy
}

Statement::~Statement() {
    _forall (unsigned, index, 0, binds_.GetSize())
        Clear(index);

    if (::mysql_stmt_close(statement_) != 0)
        throw Error(connection_->GetHandle());
}

void Statement::Clear(unsigned index) {
    if (binds_[index].buffer != NULL)
        switch (binds_[index].buffer_type)
        {
            case MYSQL_TYPE_LONGLONG:
                delete reinterpret_cast<int64_t *>(binds_[index].buffer);
                break;
            case MYSQL_TYPE_TINY:
                delete reinterpret_cast<bool *>(binds_[index].buffer);
                break;
            case MYSQL_TYPE_VAR_STRING:
                delete [] reinterpret_cast<char *>(binds_[index].buffer);
                delete binds_[index].length;
                break;
            _nodefault
        }
}

void Statement::Set(unsigned index, const ext::Buffer &value) {
    Clear(index);

    binds_[index].buffer_type = MYSQL_TYPE_VAR_STRING;
    binds_[index].buffer = new char[value.GetSize()];
    binds_[index].buffer_length = value.GetSize();
    binds_[index].length = new unsigned long(value.GetSize());

    ::memcpy(binds_[index].buffer, value.Begin(), value.GetSize());
}

void Statement::Set(unsigned index, int64_t value) {
    Clear(index);

    binds_[index].buffer_type = MYSQL_TYPE_LONGLONG;
    binds_[index].buffer = new int64_t(value);
    binds_[index].is_unsigned = false;
}

void Statement::Set(unsigned index, bool value) {
    Clear(index);

    binds_[index].buffer_type = MYSQL_TYPE_TINY;
    binds_[index].buffer = new bool(value);
    binds_[index].is_unsigned = false;
}

void Statement::Set(unsigned index, const cse::String &value) {
    Set(index, value.GetData());
}

void Statement::Set(unsigned index, const ext::Uuid &value) {
    Clear(index);

    throw ext::NotImplementedException();
}

void Statement::Set(unsigned index, const dbi::Blob &value) {
    Clear(index);

    throw ext::NotImplementedException();
}

void Statement::Set(unsigned index, const Null &value) {
    Clear(index);

    throw ext::NotImplementedException();
}


_R<dbi::ResultSet> Statement::Execute() const {
    if (::mysql_stmt_bind_param(statement_, const_cast<MYSQL_BIND *>(binds_.Begin())) != 0)
        throw StatementError(statement_);

    if (::mysql_stmt_execute(statement_) != 0)
        throw StatementError(statement_);

    ::MYSQL_RES *result(::mysql_stmt_result_metadata(statement_));
    if (result == NULL) {
        if (::mysql_field_count(connection_->GetHandle()) == 0)
            return _R<dbi::ResultSet>();
        else
            throw Error(connection_->GetHandle());
    }

    return new _H<ResultSet>(result);
}

} }
