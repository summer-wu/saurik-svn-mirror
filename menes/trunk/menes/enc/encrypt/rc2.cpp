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

#include "enc/encrypt/rc2.hpp"

#include "ext/byteorder.hpp"
#include "ext/shift.hpp"

namespace enc {

void Rc2Crypter::MixRound_(unsigned &used, uint16_t text[4]) const {
    static const unsigned shifts[] = {1, 2, 3, 5};

    _forall (unsigned, i, 0, 4)
        text[i] = ext::LeftCircleShift<uint16_t>(
            text[i] +
            key16_[used++] +
            (text[(i - 1) % 4] & text[(i - 2) % 4]) +
            (~text[(i - 1) % 4] & text[(i - 3) % 4]),
            shifts[i]
        );
}

void Rc2Crypter::MashRound_(unsigned &used, uint16_t text[4]) const {
    _forall (unsigned, i, 0, 4)
        text[i] += key16_[text[(i - 1) % 4] & 63];
}

void Rc2Crypter::ReverseMixRound_(unsigned &used, uint16_t text[4]) const {
    static const unsigned shifts[] = {1, 2, 3, 5};

    _rforall (unsigned, i, 0, 4) {
        text[i] = ext::RightCircleShift<uint16_t>(text[i], shifts[i]);
        text[i] -=
            key16_[used--] +
            (text[(i - 1) % 4] & text[(i - 2) % 4]) +
            (~text[(i - 1) % 4] & text[(i - 3) % 4]);
    }
}

void Rc2Crypter::ReverseMashRound_(unsigned &used, uint16_t text[4]) const {
    _rforall (unsigned, i, 0, 4)
        text[i] -= key16_[text[(i - 1) % 4] & 63];
}

Rc2Crypter::Rc2Crypter(const ext::Buffer &key, unsigned bits) {
    static const uint8_t pitable_[256] = {
        0xd9, 0x78, 0xf9, 0xc4, 0x19, 0xdd, 0xb5, 0xed, 0x28, 0xe9, 0xfd, 0x79, 0x4a, 0xa0, 0xd8, 0x9d,
        0xc6, 0x7e, 0x37, 0x83, 0x2b, 0x76, 0x53, 0x8e, 0x62, 0x4c, 0x64, 0x88, 0x44, 0x8b, 0xfb, 0xa2,
        0x17, 0x9a, 0x59, 0xf5, 0x87, 0xb3, 0x4f, 0x13, 0x61, 0x45, 0x6d, 0x8d, 0x09, 0x81, 0x7d, 0x32,
        0xbd, 0x8f, 0x40, 0xeb, 0x86, 0xb7, 0x7b, 0x0b, 0xf0, 0x95, 0x21, 0x22, 0x5c, 0x6b, 0x4e, 0x82,
        0x54, 0xd6, 0x65, 0x93, 0xce, 0x60, 0xb2, 0x1c, 0x73, 0x56, 0xc0, 0x14, 0xa7, 0x8c, 0xf1, 0xdc,
        0x12, 0x75, 0xca, 0x1f, 0x3b, 0xbe, 0xe4, 0xd1, 0x42, 0x3d, 0xd4, 0x30, 0xa3, 0x3c, 0xb6, 0x26,
        0x6f, 0xbf, 0x0e, 0xda, 0x46, 0x69, 0x07, 0x57, 0x27, 0xf2, 0x1d, 0x9b, 0xbc, 0x94, 0x43, 0x03,
        0xf8, 0x11, 0xc7, 0xf6, 0x90, 0xef, 0x3e, 0xe7, 0x06, 0xc3, 0xd5, 0x2f, 0xc8, 0x66, 0x1e, 0xd7,
        0x08, 0xe8, 0xea, 0xde, 0x80, 0x52, 0xee, 0xf7, 0x84, 0xaa, 0x72, 0xac, 0x35, 0x4d, 0x6a, 0x2a,
        0x96, 0x1a, 0xd2, 0x71, 0x5a, 0x15, 0x49, 0x74, 0x4b, 0x9f, 0xd0, 0x5e, 0x04, 0x18, 0xa4, 0xec,
        0xc2, 0xe0, 0x41, 0x6e, 0x0f, 0x51, 0xcb, 0xcc, 0x24, 0x91, 0xaf, 0x50, 0xa1, 0xf4, 0x70, 0x39,
        0x99, 0x7c, 0x3a, 0x85, 0x23, 0xb8, 0xb4, 0x7a, 0xfc, 0x02, 0x36, 0x5b, 0x25, 0x55, 0x97, 0x31,
        0x2d, 0x5d, 0xfa, 0x98, 0xe3, 0x8a, 0x92, 0xae, 0x05, 0xdf, 0x29, 0x10, 0x67, 0x6c, 0xba, 0xc9,
        0xd3, 0x00, 0xe6, 0xcf, 0xe1, 0x9e, 0xa8, 0x2c, 0x63, 0x16, 0x01, 0x3f, 0x58, 0xe2, 0x89, 0xa9,
        0x0d, 0x38, 0x34, 0x1b, 0xab, 0x33, 0xff, 0xb0, 0xbb, 0x48, 0x0c, 0x5f, 0xb9, 0xb1, 0xcd, 0x2e,
        0xc5, 0xf3, 0xdb, 0x47, 0xe5, 0xa5, 0x9c, 0x77, 0x0a, 0xa6, 0x20, 0x68, 0xfe, 0x7f, 0xc1, 0xad
    };

    _assert(!key.IsEmpty());
    _assert(key.GetSize() <= 128);
    _assert(key.GetSize() == (bits + 7) / 8);

    unsigned bytes(key.GetSize());
    ext::CopyAssign(key8_, key.Begin(), bytes);

    // XXX: analyze efficiency of this algorithm vs my standard ~(~ << ) trick
    uint8_t mask(255 % (1 << (8 + bits - 8 * bytes)));

    _forall (unsigned, i, bytes, 128)
        key8_[i] = pitable_[(key8_[i - 1] + key8_[i - bytes]) % 256];

    key8_[128 - bytes] = pitable_[key8_[128 - bytes] & mask];

    _rforall (unsigned, i, 0, 128 - bytes)
        key8_[i] = pitable_[key8_[i + 1] ^ key8_[i + bytes]];

    _forall (unsigned, i, 0, 64)
        key16_[i] = ext::LittleEndianOrder::Decode(key16_[i]);
}

void Rc2Crypter::Encrypt(byte_t text[4]) const {
    unsigned used(0);

    uint16_t * const text16(reinterpret_cast<uint16_t *>(text));

    _forall (unsigned, i, 0, 4)
        text16[i] = ext::LittleEndianOrder::Decode(text16[i]);

    _repeat(5) MixRound_(used, text16);
    MashRound_(used, text16);
    _repeat(6) MixRound_(used, text16);
    MashRound_(used, text16);
    _repeat(5) MixRound_(used, text16);

    _forall (unsigned, i, 0, 4)
        text16[i] = ext::LittleEndianOrder::Encode(text16[i]);
}

void Rc2Crypter::Decrypt(byte_t text[4]) const {
    unsigned used(63);

    uint16_t * const text16(reinterpret_cast<uint16_t *>(text));

    _forall (unsigned, i, 0, 4)
        text16[i] = ext::LittleEndianOrder::Decode(text16[i]);

    _repeat(5) ReverseMixRound_(used, text16);
    ReverseMashRound_(used, text16);
    _repeat(6) ReverseMixRound_(used, text16);
    ReverseMashRound_(used, text16);
    _repeat(5) ReverseMixRound_(used, text16);

    _forall (unsigned, i, 0, 4)
        text16[i] = ext::LittleEndianOrder::Encode(text16[i]);
}

Rc2Encrypter::Rc2Encrypter(const _R<ios::Writer> &writer, const ext::Buffer &key, unsigned bits) :
    writer_(writer),
    crypter_(key, bits)
{
}

void Rc2Encrypter::Write_(const byte_t *data) {
    byte_t text[8];
    ext::CopyAssign(text, data, 8);
    crypter_.Encrypt(text);
    writer_->WriteFully(text, 8);
}

void Rc2Encrypter::Close_(const byte_t *data, size_t length) {
    _assert(length == 0);
}

Rc2Decrypter::Rc2Decrypter(const _R<ios::Writer> &writer, const ext::Buffer &key, unsigned bits) :
    writer_(writer),
    crypter_(key, bits)
{
}

void Rc2Decrypter::Write_(const byte_t *data) {
    byte_t text[8];
    ext::CopyAssign(text, data, 8);
    crypter_.Decrypt(text);
    writer_->WriteFully(text, 8);
}

void Rc2Decrypter::Close_(const byte_t *data, size_t length) {
    _assert(length == 0);
}

}
