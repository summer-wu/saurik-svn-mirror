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

#include "enc/hash/adler32.hpp"
#include "ios/getput.hpp"

namespace enc {

Adler32Hasher::Adler32Hasher() :
    ios::WriterWriter(_R<ios::Writer>()),
    adler_(1)
{
}

Adler32Hasher::Adler32Hasher(const _R<ios::Writer> &writer) :
    ios::WriterWriter(writer),
    adler_(1)
{
}

Adler32Hasher::~Adler32Hasher() {
    if (!writer_.IsEmpty())
        ios::PutBig(*writer_, adler_);
}

size_t Adler32Hasher::Write(const byte_t *data, size_t length) {
    OPT_TIMER("enc::Adler32Writer::Write()")

    static const uint32_t Base_(65521);

    uint32_t low(adler_ & 0xffff);
    uint32_t high(adler_ >> 16);

    _forall (size_t, i, 0, length) {
        low = (low + data[i]) % Base_;
        high = (high + low) % Base_;
    }

    adler_ = (high << 16) | low;

    return length;
}

uint32_t Adler32Hasher::GetHash() const {
    return adler_;
}

}
