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

#ifndef MENES_IOS_STREAMBUF_HPP
#define MENES_IOS_STREAMBUF_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/block.hpp"
#include "ios/streams.hpp"

namespace ios {

class ReaderBuffer :
    public ReaderReader
{
  private:
    ext::Block<byte_t> buffer_;

    size_t begin_;
    size_t end_;

  public:
    ReaderBuffer(const _R<Reader> &reader, size_t storage = 1024) :
        ReaderReader(reader),
        buffer_(storage),
        begin_(0),
        end_(0)
    {
    }

    ~ReaderBuffer() {
        if (!std::uncaught_exception())
            _assert(begin_ == end_);
    }

    virtual size_t Read(byte_t *data, size_t length) {
        //OPT_TIMER("ios::ReaderBuffer::Read()")

        if (begin_ == end_) {
            //OPT_TIMER("ios::ReaderBuffer::Read()[Grow]")

            begin_ = 0;
            end_ = reader_->Read(buffer_.Begin(), buffer_.GetSize());
        }

        length = ext::min(length, end_ - begin_);

        {
            //OPT_TIMER("ios::ReaderBuffer::Read()[Copy]")
            ext::CopyAssign(data, buffer_.Begin() + begin_, length);
        }

        begin_ += length;
        return length;
    }
};

class WriterBuffer :
    public WriterWriter
{
  private:
    ext::Buffer buffer_;
    size_t storage_;

  public:
    WriterBuffer(const _R<Writer> &writer, size_t storage = ~size_t()) :
        WriterWriter(writer),
        storage_(storage)
    {
    }

    ~WriterBuffer() {
        if (!std::uncaught_exception())
            Flush();
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        OPT_TIMER("ios::WriterBuffer::Write()");

        buffer_.InsertAllLast(data, length);

        if (buffer_.GetSize() > storage_) {
            size_t writ(writer_->Write(buffer_.Begin(), buffer_.GetSize()));

            OPT_TIMER("ios::WriterBuffer::Write()[Shift]")
            // XXX: this is lame
            _forall (size_t, i, 0, writ)
                buffer_.RemoveFirst();
        }

        return length;
    }

    virtual void Flush() {
        if (!buffer_.IsEmpty()) {
            writer_->WriteFully(buffer_.Begin(), buffer_.GetSize());
            buffer_.Clear();
        }

        return WriterWriter::Flush();
    }
};

}

#endif//MENES_IOS_STREAMBUF_HPP
