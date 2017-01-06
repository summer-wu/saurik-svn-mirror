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

#ifndef MENES_NET_HTTP_CHUNKED_HPP
#define MENES_NET_HTTP_CHUNKED_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/digits.hpp"

#include "ios/helpers.hpp"
#include "ios/streams.hpp"

namespace net {
namespace Http {

class ChunkedReader :
    public ios::ReaderReader
{
  private:
    size_t limit_;

  public:
    ChunkedReader(const _R<ios::Reader> &reader) :
        ios::ReaderReader(reader),
        limit_(0)
    {
    }

    ~ChunkedReader() {
        if (!std::uncaught_exception())
            ios::Discard(*this);
    }

    // XXX: frowny face? :(
    virtual size_t Read(byte_t *data, size_t length) {
        if (limit_ == 0) {
            limit_ = cse::FromDigits<16, size_t>(ios::ReadLine(*reader_));

            if (limit_ == 0) {
                _assert(ios::ReadLine(*reader_).IsEmpty());
                limit_ = ~size_t();
            }
        }

        if (limit_ == ~size_t())
            return 0;

        size_t read(ios::ReaderReader::Read(data, ext::min(length, limit_)));

        limit_ -= read;
        if (limit_ == 0)
            _assert(ios::ReadLine(*reader_).IsEmpty());

        return read;
    }
};

class ChunkedWriter :
    public ios::WriterWriter
{
  public:
    ChunkedWriter(const _R<ios::Writer> &writer) :
        ios::WriterWriter(writer)
    {
    }

    ~ChunkedWriter() {
        if (!std::uncaught_exception()) {
            writer_->WriteFully("0\r\n", 3);
            writer_->WriteFully("\r\n", 2);
        }
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        if (length == 0)
            return 0;

        // XXX: this is so inefficient it hurts
        cse::String size(cse::ToDigits<16>(length));
        writer_->WriteFully(size.GetData().Begin(), size.GetData().GetSize());
        writer_->WriteFully("\r\n", 2);
        writer_->WriteFully(data, length);
        writer_->WriteFully("\r\n", 2);
        return length;
    }
};

} }

#endif//MENES_NET_HTTP_CHUNKED_HPP
