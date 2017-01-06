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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "cse/digits.hpp"
#include "enc/encode/hex.hpp"

namespace enc {

HexDecoder::HexDecoder(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer),
    state_(false)
{
}

HexDecoder::~HexDecoder() {
    if (!std::uncaught_exception())
        _assert(!state_);
}

size_t HexDecoder::Write(const byte_t *data, size_t length) {
    _forall (size_t, i, 0, length)
        if (state_ = !state_)
            hold_ = data[i];
        else
            ios::Put(*writer_, (cse::FromDigit(hold_) << 4) | cse::FromDigit(data[i]));

    return length;
}

HexEncoder::HexEncoder(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer)
{
}

size_t HexEncoder::Write(const byte_t *data, size_t length) {
    static const char hex[] = "0123456789abcdef";

    _forall (size_t, i, 0, length) {
        byte_t value(data[i]);
        ios::Put(*writer_, hex[value >> 4]);
        ios::Put(*writer_, hex[value & 0xf]);
    }

    return length;
}

}
