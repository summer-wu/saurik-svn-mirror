/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#include "api/zlib/inflate.hpp"

namespace api {
namespace Z {

InflateWriter::InflateWriter(const _R<ios::Writer> &writer) :
    writer_(writer)
{
    OPT_TIMER("api::Z::InflateWriter::InflateWriter()")

    ::memset(&stream_, 0, sizeof(z_stream));

    int zerr(::inflateInit(&stream_));
    if (zerr != Z_OK)
        throw Error(zerr);
}

InflateWriter::~InflateWriter() {
    OPT_TIMER("api::Z::InflateWriter::~InflateWriter()")

    int zerr(::inflateEnd(&stream_));
    if (zerr != Z_OK)
        throw Error(zerr);
}

size_t InflateWriter::Write(const byte_t *data, size_t length) {
    OPT_TIMER("api::Z::InflateWriter::Write()")

    buffer_.InsertAllLast(data, length);
    stream_.next_in = reinterpret_cast<uint8_t *>(buffer_.Begin());
    stream_.avail_in = static_cast<uInt>(buffer_.GetSize());

    _S<ios::Buffer> buffer(1024);

    for (;;) {
        stream_.next_out = reinterpret_cast<uint8_t *>(buffer.Begin());
        stream_.avail_out = static_cast<uInt>(buffer.GetSize());

        int zerr(::inflate(&stream_, Z_SYNC_FLUSH));
        if (zerr != Z_OK && zerr != Z_STREAM_END && !(zerr == Z_BUF_ERROR && stream_.avail_in == 0))
            throw Error(zerr);

        if (stream_.avail_out == buffer.GetSize())
            break;

        writer_->WriteFully(buffer.Begin(), buffer.GetSize() - stream_.avail_out);
    }

    size_t written(buffer_.GetSize() - stream_.avail_in);
    buffer_.RemoveAllAt(buffer_.Begin(), written);

    return length;
}

void InflateWriter::Flush() {
}

} }
