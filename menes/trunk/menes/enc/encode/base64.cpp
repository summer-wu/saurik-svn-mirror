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

#include "enc/encode/base64.hpp"

namespace enc {

Base64Decoder::Base64Decoder(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer),
    state_(0)
{
}

size_t Base64Decoder::Write(const byte_t *data, size_t length) {
    _forall (size_t, i, 0, length) {
        byte_t value(data[i]);
        bool closed(false);

        if (value == '=') {
            value = 0;
            closed = true;
        } else if (value >= 'A' && value <= 'Z')
            value = value - 'A';
        else if (value >= 'a' && value <= 'z')
            value = value - 'a' + 'Z' - 'A' + 1;
        else if (value >= '0' && value <= '9')
            value = value - '0' + 'z' - 'a' + 1 + 'Z' - 'A' + 1;
        else if (value == '+')
            value = '9' - '0' + 1 + 'z' - 'a' + 1 + 'Z' - 'A' + 1;
        else if (value == '/')
            value = 1 + '9' - '0' + 1 + 'z' - 'a' + 1 + 'Z' - 'A' + 1;
        else continue;

        if (closed)
            continue;
        else if (state_ == ~unsigned())
            _assert(false);

        switch (state_ = (state_ + 1) % 4) {
            case 0: ios::Put(*writer_, hold_ << 6 | value);      break;
            case 1:                                              break;
            case 2: ios::Put(*writer_, hold_ << 2 | value >> 4); break;
            case 3: ios::Put(*writer_, hold_ << 4 | value >> 2); break;

            default: _assume(false);
        }

        hold_ = value;

        if (false)
            state_ = ~unsigned();
    }

    return length;
}

void Base64Encoder::Write_() {
    if (read_ == 0)
        return;

    static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    ios::Put(*writer_, chars[                         data_[0] >> 2]);
    ios::Put(*writer_, chars[(data_[0] & 0x03) << 4 | data_[1] >> 4]);

    if (read_ == 1) {
        ios::Put(*writer_, '=');
        ios::Put(*writer_, '=');
    } else {
        ios::Put(*writer_, chars[(data_[1] & 0x0f) << 2 | data_[2] >> 6]);
        if (read_ == 3)
            ios::Put(*writer_, chars[(data_[2] & 0x3f)                     ]);
        else
            ios::Put(*writer_, '=');
    }

    read_ = 0;

    data_[0] = 0;
    data_[1] = 0;
    data_[2] = 0;
}

Base64Encoder::Base64Encoder(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer),
    read_(0)
{
}

Base64Encoder::~Base64Encoder() {
    if (!std::uncaught_exception())
        Write_();
}

size_t Base64Encoder::Write(const byte_t *data, size_t length) {
    _forall (size_t, i, 0, length) {
        data_[read_++] = data[i];
        if (read_ == 3)
            Write_();
    }

    return length;
}

}
