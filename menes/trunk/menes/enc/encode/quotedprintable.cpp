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

#include "cse/digits.hpp"
#include "enc/encode/quotedprintable.hpp"

namespace enc {

QuotedPrintableDecoder::QuotedPrintableDecoder(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer),
    state_(0)
{
}

QuotedPrintableDecoder::~QuotedPrintableDecoder() {
    if (!std::uncaught_exception())
        _assert(!state_);
}

size_t QuotedPrintableDecoder::Write(const byte_t *data, size_t length) {
    _forall (size_t, i, 0, length) {
        byte_t value(data[i]);

        switch (state_) {
            case 0:
                if (value == '=')
                    state_ = 1;
                else
                    ios::Put(*writer_, value);
            break;

            case 1:
                if (value == '\n')
                    state_ = 0;
                else if (value != '\r') {
                    hold_ = value;
                    state_ = 2;
                }
            break;

            case 2:
                ios::Put(*writer_, (cse::FromDigit(hold_) << 4) | cse::FromDigit(value));
                state_ = 0;
            break;
        }
    }

    return length;
}

QuotedPrintableEncoder::QuotedPrintableEncoder(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer)
{
}

size_t QuotedPrintableEncoder::Write(const byte_t *data, size_t length) {
    static const char hex[] = "0123456789ABCDEF";
    throw ext::NotImplementedException();

    /*_forall (size_t, i, 0, length) {
        writer_->Put(hex[data[i] >> 4]);
        writer_->Put(hex[data[i] & 0xf]);
    }*/
}

}
