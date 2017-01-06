/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "ext/uuid.hpp"

namespace ext {

const Uuid Uuid::Null;

Uuid::Uuid() {
    ::memset(data_, 0, 16);
}

Uuid::Uuid(uint32_t data1, uint16_t data2, uint16_t data3, const uint8_t data4[8]) {
    data_[0] = (data1 & 0xff000000) >> 24;
    data_[1] = (data1 & 0x00ff0000) >> 16;
    data_[2] = (data1 & 0x0000ff00) >> 8;
    data_[3] = (data1 & 0x000000ff);

    data_[4] = (data2 & 0xff00) >> 8;
    data_[5] = (data2 & 0x00ff);

    data_[6] = (data3 & 0xff00) >> 8;
    data_[7] = (data3 & 0x00ff);

    data_[8] = data4[0];
    data_[9] = data4[1];
    data_[10] = data4[2];
    data_[11] = data4[3];
    data_[12] = data4[4];
    data_[13] = data4[5];
    data_[14] = data4[6];
    data_[15] = data4[7];
}

Uuid::Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint32_t data4a, uint32_t data4b) {
    data_[ 0] = (data1 & 0xff000000) >> 24;
    data_[ 1] = (data1 & 0x00ff0000) >> 16;
    data_[ 2] = (data1 & 0x0000ff00) >> 8;
    data_[ 3] = (data1 & 0x000000ff);

    data_[ 4] = (data2 & 0xff00) >> 8;
    data_[ 5] = (data2 & 0x00ff);

    data_[ 6] = (data3 & 0xff00) >> 8;
    data_[ 7] = (data3 & 0x00ff);

    data_[ 8] = static_cast<uint8_t>((data4a & 0xff000000) >> 24);
    data_[ 9] = static_cast<uint8_t>((data4a & 0x00ff0000) >> 16);
    data_[10] = static_cast<uint8_t>((data4a & 0x0000ff00) >> 8);
    data_[11] = static_cast<uint8_t>((data4a & 0x000000ff));

    data_[12] = static_cast<uint8_t>((data4b & 0xff000000) >> 24);
    data_[13] = static_cast<uint8_t>((data4b & 0x00ff0000) >> 16);
    data_[14] = static_cast<uint8_t>((data4b & 0x0000ff00) >> 8);
    data_[15] = static_cast<uint8_t>((data4b & 0x000000ff));
}

Uuid::Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint64_t data4) {
    data_[ 0] = (data1 & 0xff000000) >> 24;
    data_[ 1] = (data1 & 0x00ff0000) >> 16;
    data_[ 2] = (data1 & 0x0000ff00) >> 8;
    data_[ 3] = (data1 & 0x000000ff);

    data_[ 4] = (data2 & 0xff00) >> 8;
    data_[ 5] = (data2 & 0x00ff);

    data_[ 6] = (data3 & 0xff00) >> 8;
    data_[ 7] = (data3 & 0x00ff);

    data_[ 8] = static_cast<uint8_t>((data4 & UINT64_C(0xff00000000000000)) >> 56);
    data_[ 9] = static_cast<uint8_t>((data4 & UINT64_C(0x00ff000000000000)) >> 48);
    data_[10] = static_cast<uint8_t>((data4 & UINT64_C(0x0000ff0000000000)) >> 40);
    data_[11] = static_cast<uint8_t>((data4 & UINT64_C(0x000000ff00000000)) >> 32);
    data_[12] = static_cast<uint8_t>((data4 & UINT64_C(0x00000000ff000000)) >> 24);
    data_[13] = static_cast<uint8_t>((data4 & UINT64_C(0x0000000000ff0000)) >> 16);
    data_[14] = static_cast<uint8_t>((data4 & UINT64_C(0x000000000000ff00)) >> 8);
    data_[15] = static_cast<uint8_t>((data4 & UINT64_C(0x00000000000000ff)));
}

Uuid::Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint8_t data4a, uint8_t data4b, uint8_t data4c, uint8_t data4d, uint8_t data4e, uint8_t data4f, uint8_t data4g, uint8_t data4h) {
    data_[0] = (data1 & 0xff000000) >> 24;
    data_[1] = (data1 & 0x00ff0000) >> 16;
    data_[2] = (data1 & 0x0000ff00) >> 8;
    data_[3] = (data1 & 0x000000ff);

    data_[4] = (data2 & 0xff00) >> 8;
    data_[5] = (data2 & 0x00ff);

    data_[6] = (data3 & 0xff00) >> 8;
    data_[7] = (data3 & 0x00ff);

    data_[8] = data4a;
    data_[9] = data4b;
    data_[10] = data4c;
    data_[11] = data4d;
    data_[12] = data4e;
    data_[13] = data4f;
    data_[14] = data4g;
    data_[15] = data4h;
}

bool Uuid::operator <(const Uuid &rhs) const {
    _forall (unsigned, i, 0, 16)
        if (data_[i] != rhs.data_[i])
            return data_[i] < rhs.data_[i];
    return false;
}

bool Uuid::operator ==(const Uuid &rhs) const {
    _forall (unsigned, i, 0, 16)
        if (data_[i] != rhs.data_[i])
            return false;
    return true;
}

bool Uuid::operator !=(const Uuid &rhs) const {
    return !operator ==(rhs);
}

Uuid &Uuid::operator =(const Uuid &rhs) {
    _forall (unsigned, i, 0, 16)
        data_[i] = rhs.data_[i];
    return *this;
}

const uint8_t *Uuid::GetData() const {
    return data_;
}

uint8_t *Uuid::GetData() {
    return data_;
}

namespace {

uint8_t unHex(char c) {
    switch (c) {
        case '0': return  0;
        case '1': return  1;
        case '2': return  2;
        case '3': return  3;
        case '4': return  4;
        case '5': return  5;
        case '6': return  6;
        case '7': return  7;
        case '8': return  8;
        case '9': return  9;

        case 'a': return 10;
        case 'b': return 11;
        case 'c': return 12;
        case 'd': return 13;
        case 'e': return 14;
        case 'f': return 15;

        case 'A': return 10;
        case 'B': return 11;
        case 'C': return 12;
        case 'D': return 13;
        case 'E': return 14;
        case 'F': return 15;

        default:
            _assume(false);
            return 0;
    }
}

uint8_t unHex(wchar_t c) {
    switch (c) {
        case L'0': return  0;
        case L'1': return  1;
        case L'2': return  2;
        case L'3': return  3;
        case L'4': return  4;
        case L'5': return  5;
        case L'6': return  6;
        case L'7': return  7;
        case L'8': return  8;
        case L'9': return  9;

        case L'a': return 10;
        case L'b': return 11;
        case L'c': return 12;
        case L'd': return 13;
        case L'e': return 14;
        case L'f': return 15;

        case L'A': return 10;
        case L'B': return 11;
        case L'C': return 12;
        case L'D': return 13;
        case L'E': return 14;
        case L'F': return 15;

        default:
            _assume(false);
            return 0;
    }
}

}

ios::PrintReader &operator >>(ios::PrintReader &lhs, Uuid &rhs) {
    uint8_t *data(rhs.GetData());

    _forall (unsigned, i, 0, 16) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            _assert(ios::Get(lhs) == '-');

        char temp[2];
        lhs.ReadFully(temp, 2);

        data[i] = (unHex(temp[0]) << 4) | unHex(temp[1]);
    }

    return lhs;
}

ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const Uuid &rhs) {
    static const char hex[] = "0123456789abcdef";
    const uint8_t *data(rhs.GetData());

    _S<ios::FormatWriter> out(lhs);
    out << ios::SetBase(16) << ios::SetFill('0');

    _forall (unsigned, i, 0, 4)
        out << ios::SetWidth(2) << data[i];

    out << "-";
    _forall (unsigned, i, 4, 6)
        out << ios::SetWidth(2) << data[i];

    out << "-";
    _forall (unsigned, i, 6, 8)
        out << ios::SetWidth(2) << data[i];

    out << "-";
    _forall (unsigned, i, 8, 10)
        out << ios::SetWidth(2) << data[i];

    out << "-";
    _forall (unsigned, i, 10, 16)
        out << ios::SetWidth(2) << data[i];

    return lhs;
}

}
