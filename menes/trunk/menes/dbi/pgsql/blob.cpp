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

#include "ext/exception.hpp"

#include "libpq/libpq-fs.h"

namespace dbi {
namespace PgSql {

BlobReader::BlobReader(const _R<Connection> &connection, ::Oid oid) :
    connection_(connection)
{
    fd_ = ::lo_open(connection_->GetHandle(), oid, INV_READ);
}

BlobReader::~BlobReader() {
    if (::lo_close(connection_->GetHandle(), fd_) != 0)
        throw ext::Exception();
}

size_t BlobReader::Read(char *data, size_t length) {
    OPT_TIMER("dbi::PgSql::BlobReader::Read()")

    int size(::lo_read(connection_->GetHandle(), fd_, data, length));
    if (size < 0)
        throw ext::Exception();
    return size;
}

BlobWriter::BlobWriter(const _R<Connection> &connection, ::Oid oid) :
    connection_(connection)
{
    fd_ = ::lo_open(connection_->GetHandle(), oid, INV_WRITE);
}

BlobWriter::~BlobWriter() {
    if (::lo_close(connection_->GetHandle(), fd_) != 0)
        throw ext::Exception();
}

size_t BlobWriter::Write(const byte_t *data, size_t length) {
    OPT_TIMER("dbi::PgSql::BlobWriter::Write()")

    int size(::lo_write(connection_->GetHandle(), fd_, const_cast<char *>(data), length));
    if (size < 0)
        throw ext::Exception();
    return size;
}

void BlobWriter::Flush() {
}

Blob::Blob(const _R<Connection> &connection) :
    connection_(connection)
{
    oid_ = ::lo_creat(connection_->GetHandle(), INV_READ | INV_WRITE);
}

Blob::Blob(const _R<Connection> &connection, ::Oid oid) :
    connection_(connection),
    oid_(oid)
{
}

_R<dbi::BlobReader> Blob::GetReader() {
    return new _H<BlobReader>(connection_, oid_);
}

_R<dbi::BlobWriter> Blob::GetWriter() {
    return new _H<BlobWriter>(connection_, oid_);
}

unsigned Blob::GetId() const {
    return oid_;
}

} }
