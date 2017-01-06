/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#include "enc/hash/md5.hpp"
#include "ext/shift.hpp"

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

namespace enc {

namespace {

void Round1(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t w, uint32_t s, uint32_t ac) {
    a = ext::LeftCircleShift(a + (b & c | ~b & d) + w + ac, s) + b;
}

void Round2(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t w, uint32_t s, uint32_t ac) {
    a = ext::LeftCircleShift(a + (b & d | c & ~d) + w + ac, s) + b;
}

void Round3(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t w, uint32_t s, uint32_t ac) {
    a = ext::LeftCircleShift(a + (b ^ c ^ d) + w + ac, s) + b;
}

void Round4(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t w, uint32_t s, uint32_t ac) {
    a = ext::LeftCircleShift(a + (c ^ (b | ~d)) + w + ac, s) + b;
}

}

void Md5Hasher::Write_(const char *data) {
    OPT_TIMER("enc::Md5Hasher::Write_()")

    uint32_t a, b, c, d;
    uint32_t w[16];

    const uint32_t *temp(reinterpret_cast<const uint32_t *>(data));
    for (unsigned i(0); i != 16; ++i)
        w[i] = ext::LittleEndianOrder::Decode(temp[i]);

    a = hash_[0];
    b = hash_[1];
    c = hash_[2];
    d = hash_[3];

    /* Round 1 */
    Round1(a, b, c, d, w[ 0], S11, 0xd76aa478); /*  1 */
    Round1(d, a, b, c, w[ 1], S12, 0xe8c7b756); /*  2 */
    Round1(c, d, a, b, w[ 2], S13, 0x242070db); /*  3 */
    Round1(b, c, d, a, w[ 3], S14, 0xc1bdceee); /*  4 */
    Round1(a, b, c, d, w[ 4], S11, 0xf57c0faf); /*  5 */
    Round1(d, a, b, c, w[ 5], S12, 0x4787c62a); /*  6 */
    Round1(c, d, a, b, w[ 6], S13, 0xa8304613); /*  7 */
    Round1(b, c, d, a, w[ 7], S14, 0xfd469501); /*  8 */
    Round1(a, b, c, d, w[ 8], S11, 0x698098d8); /*  9 */
    Round1(d, a, b, c, w[ 9], S12, 0x8b44f7af); /* 10 */
    Round1(c, d, a, b, w[10], S13, 0xffff5bb1); /* 11 */
    Round1(b, c, d, a, w[11], S14, 0x895cd7be); /* 12 */
    Round1(a, b, c, d, w[12], S11, 0x6b901122); /* 13 */
    Round1(d, a, b, c, w[13], S12, 0xfd987193); /* 14 */
    Round1(c, d, a, b, w[14], S13, 0xa679438e); /* 15 */
    Round1(b, c, d, a, w[15], S14, 0x49b40821); /* 16 */

   /* Round 2 */
    Round2(a, b, c, d, w[ 1], S21, 0xf61e2562); /* 17 */
    Round2(d, a, b, c, w[ 6], S22, 0xc040b340); /* 18 */
    Round2(c, d, a, b, w[11], S23, 0x265e5a51); /* 19 */
    Round2(b, c, d, a, w[ 0], S24, 0xe9b6c7aa); /* 20 */
    Round2(a, b, c, d, w[ 5], S21, 0xd62f105d); /* 21 */
    Round2(d, a, b, c, w[10], S22, 0x02441453); /* 22 */
    Round2(c, d, a, b, w[15], S23, 0xd8a1e681); /* 23 */
    Round2(b, c, d, a, w[ 4], S24, 0xe7d3fbc8); /* 24 */
    Round2(a, b, c, d, w[ 9], S21, 0x21e1cde6); /* 25 */
    Round2(d, a, b, c, w[14], S22, 0xc33707d6); /* 26 */
    Round2(c, d, a, b, w[ 3], S23, 0xf4d50d87); /* 27 */
    Round2(b, c, d, a, w[ 8], S24, 0x455a14ed); /* 28 */
    Round2(a, b, c, d, w[13], S21, 0xa9e3e905); /* 29 */
    Round2(d, a, b, c, w[ 2], S22, 0xfcefa3f8); /* 30 */
    Round2(c, d, a, b, w[ 7], S23, 0x676f02d9); /* 31 */
    Round2(b, c, d, a, w[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    Round3(a, b, c, d, w[ 5], S31, 0xfffa3942); /* 33 */
    Round3(d, a, b, c, w[ 8], S32, 0x8771f681); /* 34 */
    Round3(c, d, a, b, w[11], S33, 0x6d9d6122); /* 35 */
    Round3(b, c, d, a, w[14], S34, 0xfde5380c); /* 36 */
    Round3(a, b, c, d, w[ 1], S31, 0xa4beea44); /* 37 */
    Round3(d, a, b, c, w[ 4], S32, 0x4bdecfa9); /* 38 */
    Round3(c, d, a, b, w[ 7], S33, 0xf6bb4b60); /* 39 */
    Round3(b, c, d, a, w[10], S34, 0xbebfbc70); /* 40 */
    Round3(a, b, c, d, w[13], S31, 0x289b7ec6); /* 41 */
    Round3(d, a, b, c, w[ 0], S32, 0xeaa127fa); /* 42 */
    Round3(c, d, a, b, w[ 3], S33, 0xd4ef3085); /* 43 */
    Round3(b, c, d, a, w[ 6], S34, 0x04881d05); /* 44 */
    Round3(a, b, c, d, w[ 9], S31, 0xd9d4d039); /* 45 */
    Round3(d, a, b, c, w[12], S32, 0xe6db99e5); /* 46 */
    Round3(c, d, a, b, w[15], S33, 0x1fa27cf8); /* 47 */
    Round3(b, c, d, a, w[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    Round4(a, b, c, d, w[ 0], S41, 0xf4292244); /* 49 */
    Round4(d, a, b, c, w[ 7], S42, 0x432aff97); /* 50 */
    Round4(c, d, a, b, w[14], S43, 0xab9423a7); /* 51 */
    Round4(b, c, d, a, w[ 5], S44, 0xfc93a039); /* 52 */
    Round4(a, b, c, d, w[12], S41, 0x655b59c3); /* 53 */
    Round4(d, a, b, c, w[ 3], S42, 0x8f0ccc92); /* 54 */
    Round4(c, d, a, b, w[10], S43, 0xffeff47d); /* 55 */
    Round4(b, c, d, a, w[ 1], S44, 0x85845dd1); /* 56 */
    Round4(a, b, c, d, w[ 8], S41, 0x6fa87e4f); /* 57 */
    Round4(d, a, b, c, w[15], S42, 0xfe2ce6e0); /* 58 */
    Round4(c, d, a, b, w[ 6], S43, 0xa3014314); /* 59 */
    Round4(b, c, d, a, w[13], S44, 0x4e0811a1); /* 60 */
    Round4(a, b, c, d, w[ 4], S41, 0xf7537e82); /* 61 */
    Round4(d, a, b, c, w[11], S42, 0xbd3af235); /* 62 */
    Round4(c, d, a, b, w[ 2], S43, 0x2ad7d2bb); /* 63 */
    Round4(b, c, d, a, w[ 9], S44, 0xeb86d391); /* 64 */

    hash_[0] += a;
    hash_[1] += b;
    hash_[2] += c;
    hash_[3] += d;
}

Md5Hasher::Md5Hasher(const _R<ios::Writer> &writer) :
    Base_(writer)
{
    hash_[0] = 0x67452301;
    hash_[1] = 0xefcdab89;
    hash_[2] = 0x98badcfe;
    hash_[3] = 0x10325476;
}

}
