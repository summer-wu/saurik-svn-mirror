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

#include "api/fiber.hpp"
#include "ext/refcount.hpp"
#include "mfa/pipe.hpp"

namespace mfa {

namespace be {
struct PipeData_ :
    public ext::ReferenceCounted
{
    const byte_t *data_;
    size_t length_;

    api::Fiber *fiber_;

    PipeData_(api::Fiber *fiber) :
        data_(NULL),
        length_(0),
        fiber_(fiber)
    {
    }

    void Swap() volatile {
        if (api::Fiber *fiber = fiber_) {
            fiber_ = NULL;
            fiber->Call();
        } else {
            fiber_ = &api::Fiber::Current();
            api::Fiber::Return();
        }
    }
}; }

PipeReader::PipeReader(const _R<be::PipeData_> &data) :
    data_(data)
{
}

PipeReader::~PipeReader() {
    if (!std::uncaught_exception())
        _assert(data_->length_ == 0);
}

size_t PipeReader::Read(byte_t *data, size_t length) {
    if (length == 0)
        return 0;

    if (data_->length_ == 0)
        data_->Swap();

    if (data_->length_ == 0)
        return 0;

    if (data_->length_ < length)
        length = data_->length_;

    ext::CopyAssign(data, data_->data_, length);

    data_->data_ += length;
    data_->length_ -= length;

    return length;
}

PipeWriter::PipeWriter(const _R<be::PipeData_> &data) :
    data_(data)
{
}

PipeWriter::~PipeWriter() {
    if (api::Fiber *reader = data_->fiber_) {
        if (!reader->IsStopped())
            reader->Call();
    } else if (!std::uncaught_exception())
        _assert(false);
}

size_t PipeWriter::Write(const byte_t *data, size_t length) {
    if (length == 0)
        return 0;

    _assert(data_->length_ == 0);

    data_->data_ = data;
    data_->length_ = length;

    data_->Swap();

    _assert(data_->length_ == 0);
    return length;
}

Pipe::Pipe(api::Fiber *fiber) {
    _R<be::PipeData_> data(new _H<be::PipeData_>(fiber));
    reader_ = new _H<PipeReader>(data);
    writer_ = new _H<PipeWriter>(data);
}

}
