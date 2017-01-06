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

#ifndef MENES_IOS_FILTER_HPP
#define MENES_IOS_FILTER_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/stackref.hpp"
#include "ios/buffer.hpp"

namespace ios {

class FilterReader :
    public ios::Reader
{
  private:
    _R<Reader> reader_;
    _R<Writer> writer_;
    _R<ios::Buffer> buffer_;

  public:
    FilterReader(const _R<Reader> reader, const _R<Writer> &writer, const _R<ios::Buffer> &buffer) :
        reader_(reader),
        writer_(writer),
        buffer_(buffer)
    {
    }

    virtual size_t Read(byte_t *data, size_t length) {
        _forever if (size_t read = buffer_->Read(data, length))
            return read;
        else if (writer_.IsEmpty())
            return 0;
        else {
            byte_t data[1024];

            if (size_t read = reader_->Read(data, sizeof(data)))
                // XXX: this should buffer this data here (yes, I know that's insane)
                writer_->WriteFully(data, read);
            else
                writer_.Clear();
        }
    }
};

template <typename Filter_>
ext::Buffer Filter(const ext::Buffer &data) {
    _S<Buffer> result;

    {
        _S<Filter_> filter(&result);
        filter.WriteFully(data.Begin(), data.GetSize());
    }

    return result;
}

template <typename Filter_, typename Type_>
ext::Buffer Filter(const ext::Buffer &data, const Type_ &arg) {
    _S<Buffer> result;

    {
        _S<Filter_> filter(&result, arg);
        filter.WriteFully(data.Begin(), data.GetSize());
    }

    return result;
}

template <typename Filter_>
_R<ios::Reader> FilterAsReader(const _R<ios::Reader> &reader) {
    _R<Buffer> buffer(new _H<Buffer>());
    _R<Writer> filter(new _H<Filter_>(buffer));
    return new _H<FilterReader>(reader, filter, buffer);
}

}

#endif//MENES_IOS_FILTER_HPP
