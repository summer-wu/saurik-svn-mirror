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

#ifndef MENES_NET_HTTP_URLENCODE_HPP
#define MENES_NET_HTTP_URLENCODE_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/digits.hpp"
#include "ios/streams.hpp"

namespace net {
namespace Http {

class UrlEncoder :
    public ios::WriterWriter
{
  public:
    ext::Buffer reserved_;

  public:
    UrlEncoder(const _R<ios::Writer> &writer, const ext::Buffer &reserved) :
        ios::WriterWriter(writer),
        reserved_(reserved)
    {
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        static const char hex_[] = "0123456789abcdef";

        for (size_t i(0); i != length; ++i)
            // XXX: note that this doesn't do anything
            if (!ext::Contains(reserved_, data[i]))
                ios::Put(*writer_, data[i]);
            else {
                char escape[3];
                escape[0] = '%';
                escape[1] = hex_[data[i] >> 4];
                escape[2] = hex_[data[i] & 0xf];
                writer_->Write(escape, 3);
            }

        return length;
    }
};

class UrlDecoder :
    public ios::WriterWriter
{
  private:
    enum State_ {
        Normal,
        Begin,
        Wait
    } state_;

    char buffer_;

  public:
    UrlDecoder(const _R<ios::Writer> &writer) :
        ios::WriterWriter(writer),
        state_(Normal)
    {
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        for (size_t i(0); i != length; ++i)
            switch (state_) {
                case Normal:
                    if (data[i] == '%')
                        state_ = Begin;
                    else if (data[i] == '+')
                        ios::Put(*writer_, ' ');
                    else
                        ios::Put(*writer_, data[i]);
                break;

                case Begin:
                    state_ = Wait;
                    buffer_ = data[i];
                break;

                case Wait:
                    state_ = Normal;
                    ios::Put(*writer_, (cse::FromDigit(buffer_) << 4) + cse::FromDigit(data[i]));
                break;
            }

        return length;
    }
};

} }

#endif//MENES_NET_HTTP_URLENCODE_HPP
