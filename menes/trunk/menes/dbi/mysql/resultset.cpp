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

#include "dbi/mysql/error.hpp"
#include "dbi/mysql/resultset.hpp"

namespace dbi {
namespace MySql {

ResultSet::ResultSet(MYSQL_RES *result) :
    result_(result),
    row_(NULL)
{
}

ResultSet::~ResultSet() {
    ::mysql_free_result(result_);
}

cse::String ResultSet::GetNameByIndex(unsigned column) const {
    return ::mysql_fetch_field_direct(result_, column)->name;
}

unsigned ResultSet::GetIndexByName(const cse::String &name) const {
    unsigned number(::mysql_num_fields(result_));
    MYSQL_FIELD *fields(::mysql_fetch_fields(result_));
    for (unsigned i(0); i != number; ++i)
        if (fields[i].name == name)
            return i;
    // XXX: maybe this should throw an exception?
    return ~unsigned();
}

bool ResultSet::IsNull(unsigned column) const {
    return row_[column] == NULL;
}

_R<dbi::Blob> ResultSet::GetBlob(unsigned column) const {
    throw ext::NotImplementedException();
}

cse::String ResultSet::GetString(unsigned column) const {
    return row_[column] == NULL ? cse::String() : cse::String(row_[column], ::mysql_fetch_lengths(result_)[column]);
}

bool ResultSet::MoveNext() {
    row_ = ::mysql_fetch_row(result_);
    return row_ != NULL;
}

} }
