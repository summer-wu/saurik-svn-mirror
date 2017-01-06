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

#include "enc/hash/sha1.hpp"
#include "ext/shift.hpp"

namespace enc {

namespace {
_finline void Rotate_(uint32_t t[5], uint32_t temp) {
    t[4] = t[3];
    t[3] = t[2];
    t[2] = ext::LeftCircleShift(t[1], 30);
    t[1] = t[0];
    t[0] = ext::LeftCircleShift(t[0], 5) + temp;
} }

void Sha1Hasher::Write_(const char *data) {
    OPT_TIMER("enc::Sha1Hasher::Write_()")

    uint32_t t[5];
    uint32_t w[80];

    _forall (unsigned, i, 0, 16)
        w[i] = (data[i * 4] << 24) | (data[i * 4 + 1] << 16) | (data[i * 4 + 2] << 8) | (data[i * 4 + 3]);

    _forall (unsigned, i, 16, 80)
        w[i] = ext::LeftCircleShift(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    _forall (unsigned, i, 0, 5)
        t[i] = hash_[i];

    _forall (unsigned, i, 0, 20)
        Rotate_(t, ((t[1] & t[2]) | ((~t[1]) & t[3])) + t[4] + w[i] + 0x5a827999);

    _forall (unsigned, i, 20, 40)
        Rotate_(t, (t[1] ^ t[2] ^ t[3]) + t[4] + w[i] + 0x6ed9eba1);

    _forall (unsigned, i, 40, 60)
        Rotate_(t, ((t[1] & t[2]) | (t[1] & t[3]) | (t[2] & t[3])) + t[4] + w[i] + 0x8f1bbcdc);

    _forall (unsigned, i, 60, 80)
        Rotate_(t, (t[1] ^ t[2] ^ t[3]) + t[4] + w[i] + 0xca62c1d6);

    _forall (unsigned, i, 0, 5)
        hash_[i] += t[i];
}

Sha1Hasher::Sha1Hasher(const _R<ios::Writer> &writer) :
    Base_(writer)
{
    hash_[0] = 0x67452301;
    hash_[1] = 0xefcdaB89;
    hash_[2] = 0x98badCfe;
    hash_[3] = 0x10325476;
    hash_[4] = 0xc3d2e1f0;
}

}
