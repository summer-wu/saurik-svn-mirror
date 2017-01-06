/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#include "api/uuid.hpp"

#include "api/zlib/deflate.hpp"
#include "api/zlib/inflate.hpp"

#include "dbi/blob.hpp"
#include "dbi/resultset.hpp"

#include "enc/compress/zlib.hpp"

#include "enc/hash/md5.hpp"
#include "enc/hash/sha1.hpp"

#include "ext/uuid.hpp"

#include "ios/filter.hpp"
#include "ios/fork.hpp"
#include "ios/helpers.hpp"
#include "ios/stats.hpp"
#include "ios/streambuf.hpp"

#include "vfs/system.hpp"

namespace vfs {

System::System(const _R<dbi::Connection> &connection) :
    connection_(connection)
{
}

_R<ios::Reader> System::ReadFile(const ext::Uuid &id) const {
    _R<dbi::ResultSet> files(connection_->Parse("SELECT data FROM data WHERE id = ?")->Execute(id));

    // XXX: should this just throw anyway?
    if (!files->MoveNext())
        return _R<ios::Reader>();

    return ios::FilterAsReader<api::Z::InflateWriter>(files->GetBlob(0)->GetReader());
}

void System::ReadFileInto(const ext::Uuid &id, ios::Writer &writer) const {
    _R<dbi::ResultSet> files(connection_->Parse("SELECT data FROM data WHERE id = ?")->Execute(id));

    if (!files->MoveNext())
        throw;

    _S<ios::ReaderBuffer> buffer(files->GetBlob(0)->GetReader());

    OPT_TIMER("vfs::System::ReadFileInto()[Inflate]")

#if 0
    _S<api::Z::InflateWriter> inflate(&writer);
    ios::ReadToWrite(buffer, inflate);
#else
    enc::ZLibInflate(buffer, writer);
#endif
}

ext::Uuid System::WriteFileFrom(ios::Reader &reader) {
    dbi::Savepoint lock(*connection_);

    _R<dbi::Blob> data(connection_->CreateBlob());
    size_t length;
    _S<ios::Buffer> sha1hash;
    _S<ios::Buffer> md5hash;

    {
        _S<api::Z::DeflateWriter> deflate(data->GetWriter());
        _S<enc::Sha1Hasher> sha1(&sha1hash);
        _S<enc::Md5Hasher> md5(&md5hash);

        _S<ios::ForkedWriter> fork;
        fork.Insert(&sha1);
        fork.Insert(&md5);
        fork.Insert(&deflate);

        _S<ios::StatsWriter> stats(fork);
        ios::ReadToWrite(reader, stats);

        length = stats.GetWritten();
    }

    ext::Uuid id;

    {
        _R<dbi::ResultSet> files(connection_->Parse("SELECT id FROM data WHERE length = ? AND sha1hash = ? AND md5hash = ?")->Execute(length, sha1hash, md5hash));

        if (files->MoveNext()) {
            id = files->Get<ext::Uuid>(0);
            lock.Cancel();
        } else {
            id = api::Uuid::CreateSequential();

            connection_->Parse("INSERT INTO entity (id) VALUES (?)")->Execute(id);
            connection_->Parse("INSERT INTO data (id, data, length, sha1hash, md5hash) VALUES (?, ?, ?, ?, ?)")->Execute(id, *data, length, sha1hash, md5hash);
        }
    }

    return id;
}

}
