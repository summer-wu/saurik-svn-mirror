/* Menes Optimization - C++ Profiling and Verification
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

#include "ext/byteorder.hpp"
#include "ios/streams.hpp"
#include "opt/profile.hpp"
#include "szf/binary.hpp"

namespace szf {

BinaryMethod::BinaryMethod() :
    in_(NULL),
    out_(NULL)
{
}

BinaryMethod::BinaryMethod(ios::Reader *in, ios::Writer *out) :
    in_(in),
    out_(out)
{
}

BinaryMethod::~BinaryMethod() {
}

void BinaryMethod::OpenScope(const char *name) {
}

void BinaryMethod::CloseScope() {
}

void BinaryMethod::SaveBoolean(bool value, const char *name) {
    _assert(out_ != NULL);
    ios::Put(*out_, value ? 't' : 'f');
}

void BinaryMethod::LoadBoolean(bool &value, const char *name) {
    _assert(in_ != NULL);
    char temp(ios::Get(*in_));
    _assert(temp == 't' || temp == 'f');
    value = (temp == 't');
}

void BinaryMethod::SaveSigned(int64_t value, const char *name) {
    SaveUnsigned(value, name);
}

void BinaryMethod::LoadSigned(int64_t &value, const char *name) {
    LoadUnsigned(reinterpret_cast<uint64_t &>(value), name);
}

void BinaryMethod::SaveUnsigned(uint64_t value, const char *name) {
    OPT_TIMER("szf::BinaryMethod::SaveUnsigned()")
    _assert(out_ != NULL);

    value = (value & UINT64_C(0x8000000000000000)) == 0 ? (value << 1) : ~(value << 1);

    unsigned remain;
    for (remain = 0; remain != 8; ++remain)
        if ((value & ((UINT64_C(0xFFFFFFFFFFFFFF00) | (0x7f80 >> remain)) << (remain * 8))) == 0) {
            ios::Put(*out_, static_cast<char>((0xff00 >> remain) | (value >> (8 * remain))));
            break;
        }

    if (remain == 8)
        // ERR: warning: passing negative value '-0x00000000000000001' for converting 1 of 'virtual void ios::Writer::Put(byte_t)'
        ios::Put(*out_, byte_t(~byte_t()));
    while (remain-- != 0)
        ios::Put(*out_, static_cast<char>(0xff & (value >> (8 * remain))));
}

void BinaryMethod::LoadUnsigned(uint64_t &value, const char *name) {
    OPT_TIMER("szf::BinaryMethod::LoadUnsigned()")
    _assert(in_ != NULL);

    static bool oldFormat(false);

    if (oldFormat) {
        in_->Read(reinterpret_cast<char *>(&value), sizeof(value));
    } else {

    uint32_t data(ios::Get(*in_));
    unsigned remain(0);

    for (;; ++remain) {
        uint32_t check((0xff << remain) & 0xff);
        if ((data & check) == check) {
            value = data & ~check & 0xff;
            break;
        }
    }

    while (remain++ != 8)
        value = (value << 8) | (0xff & ios::Get(*in_));
    value = ((value & 0x1) == 0 ? (value >> 1) : ~(value >> 1));

    }
}

void BinaryMethod::SaveFloating(double value, const char *name) {
    _assert(out_ != NULL);
    // XXX: this doesn't take into account different storage formats
    value = ext::NetworkOrder::Encode(value);
    out_->Write(reinterpret_cast<const char *>(&value), sizeof(value));
}

void BinaryMethod::LoadFloating(double &value, const char *name) {
    _assert(in_ != NULL);
    // XXX: this doesn't take into account different storage formats
    in_->Read(reinterpret_cast<char *>(&value), sizeof(value));
    value = ext::NetworkOrder::Decode(value);
}

void BinaryMethod::SaveData(const char *data, size_t length, const char *name) {
    _assert(out_ != NULL);
    out_->Write(data, length);
}

void BinaryMethod::LoadData(char *data, size_t length, const char *name) {
    _assert(in_ != NULL);
    in_->Read(data, length);
}

void BinaryMethod::Bind(ios::Reader *in, ios::Writer *out) {
    in_ = in;
    out_ = out;
}

}
