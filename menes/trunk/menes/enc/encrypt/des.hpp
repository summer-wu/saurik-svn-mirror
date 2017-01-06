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

#ifndef MENES_ENC_ENCRYPT_DES_HPP
#define MENES_ENC_ENCRYPT_DES_HPP

#include "cxx/platform.hpp"
#include "enc/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/block.hpp"

namespace enc {

class DesCrypter {
  private:
    uint64_t subkeys_[16];

    template <size_t Output_, size_t Input_>
    uint64_t Permute_(uint64_t value, const unsigned permutation[Output_]) {
        uint64_t permuted(0);
        _forall (size_t, i, 0, Output_)
            permuted |= ((value >> Input_ - permutation[i] - 1) & 0x1) << Output_ - i - 1;
        return permuted;
    }

    template <bool Encrypt_>
    uint64_t Crypt_(uint64_t data64) {
        static const uint64_t mask32(~uint64_t() >> 32);

        static const unsigned initial_[] = {
            57, 49, 41, 33, 25, 17, 9, 1,
            59, 51, 43, 35, 27, 19, 11, 3,
            61, 53, 45, 37, 29, 21, 13, 5,
            63, 55, 47, 39, 31, 23, 15, 7,
            56, 48, 40, 32, 24, 16, 8, 0,
            58, 50, 42, 34, 26, 18, 10, 2,
            60, 52, 44, 36, 28, 20, 12, 4,
            62, 54, 46, 38, 30, 22, 14, 6
        };

        static const unsigned final_[] = {
            39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30,
            37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28,
            35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26,
            33, 1, 41, 9, 49, 17, 57, 25,
            32, 0, 40, 8, 48, 16, 56, 24
        };

        static const unsigned selection_[] = {
            31, 0, 1, 2, 3, 4,
            3, 4, 5, 6, 7, 8,
            7, 8, 9, 10, 11, 12,
            11, 12, 13, 14, 15, 16,
            15, 16, 17, 18, 19, 20,
            19, 20, 21, 22, 23, 24,
            23, 24, 25, 26, 27, 28,
            27, 28, 29, 30, 31, 0
        };

        static const unsigned permutation_[] = {
            15, 6, 19, 20,
            28, 11, 27, 16,
            0, 14, 22, 25,
            4, 17, 30, 9,
            1, 7, 23, 13,
            31, 26, 2, 8,
            18, 12, 29, 5,
            21, 10, 3, 24
        };

        static const uint8_t boxes_[][4][16] = { {
            {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
            { 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
            { 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
            {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
        }, {
            {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
            { 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
            { 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
            {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
        }, {
            {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
            {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
            {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
            { 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
        }, {
            { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
            {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
            {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
            { 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
        }, {
            { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
            {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
            { 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
            {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
        }, {
            {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
            {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
            { 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
            { 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
        }, {
            { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
            {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
            { 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
            { 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
        }, {
            {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
            { 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
            { 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
            { 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
        } };

        data64 = Permute_<64, 64>(data64, initial_);

        uint32_t ldata, rdata;
        ldata = data64 >> 32;
        rdata = data64;

        _forall (unsigned, i, 0, 16) {
            uint64_t temp48(subkeys_[Encrypt_ ? i : 15 - i] ^ Permute_<48, 32>(rdata, selection_));

            uint32_t temp32(0);
            _forall (unsigned, s, 0, 8) {
                uint8_t temp6(uint8_t(temp48 >> 42 - 6 * s) & 0x3f);
                uint8_t temp4(boxes_[s][((temp6 >> 4) & 0x2) | (temp6 & 0x1)][(temp6 >> 1) & 0xf]);
                temp32 |= uint32_t(temp4) << 28 - s * 4;
            }

            uint32_t tdata(ldata);
            ldata = rdata;
            rdata = tdata ^ Permute_<32, 32>(temp32, permutation_);
        }

        data64 = ((rdata & mask32) << 32) | (ldata & mask32);
        return Permute_<64, 64>(data64, final_);
    }

  public:
    DesCrypter(const ext::Buffer &key) {
        _assert(key.GetSize() == 8);

        static const uint32_t mask28(~uint32_t() >> 4);

        static const unsigned permutation1_[] = {
            56, 48, 40, 32, 24, 16, 8,
            0, 57, 49, 41, 33, 25, 17,
            9, 1, 58, 50, 42, 34, 26,
            18, 10, 2, 59, 51, 43, 35,
            62, 54, 46, 38, 30, 22, 14,
            6, 61, 53, 45, 37, 29, 21,
            13, 5, 60, 52, 44, 36, 28,
            20, 12, 4, 27, 19, 11, 3
        };

        static const unsigned permutation2_[] = {
            13, 16, 10, 23, 0, 4,
            2, 27, 14, 5, 20, 9,
            22, 18, 11, 3, 25, 7,
            15, 6, 26, 19, 12, 1,
            40, 51, 30, 36, 46, 54,
            29, 39, 50, 44, 32, 47,
            43, 48, 38, 55, 33, 52,
            45, 41, 49, 35, 28, 31
        };

        static const unsigned shifts_[] = {
            1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
        };

        uint64_t key64(0);

        _forall (unsigned, i, 0, 8)
            key64 |= uint64_t(key[i]) << 56 - i * 8;

        key64 = Permute_<56, 64>(key64, permutation1_);

        _forall (unsigned, i, 0, 16) {
            uint32_t lhs(uint32_t(key64 >> 28) & mask28);
            uint32_t rhs(uint32_t(key64) & mask28);

            lhs = (lhs << shifts_[i]) | (lhs >> 28 - shifts_[i]);
            rhs = (rhs << shifts_[i]) | (rhs >> 28 - shifts_[i]);

            key64 = (uint64_t(lhs & mask28) << 28) | uint64_t(rhs & mask28);
            subkeys_[i] = Permute_<48, 56>(key64, permutation2_);
        }
    }

    void Encrypt(byte_t data[8]) {
        uint64_t data64(*reinterpret_cast<const uint64_t *>(data));
        data64 = Crypt_<true>(ext::BigEndianOrder::Decode(data64));
        *reinterpret_cast<uint64_t *>(data) = ext::BigEndianOrder::Encode(data64);
    }

    void Decrypt(byte_t data[8]) {
        uint64_t data64(*reinterpret_cast<const uint64_t *>(data));
        data64 = Crypt_<false>(ext::BigEndianOrder::Decode(data64));
        *reinterpret_cast<uint64_t *>(data) = ext::BigEndianOrder::Encode(data64);
    }
};

class DesEncrypter :
    public ios::BlockWriter<8>
{
  private:
    _R<ios::Writer> writer_;
    DesCrypter crypter_;

  protected:
    virtual void Write_(const byte_t *data) {
        byte_t result[8];
        ext::CopyAssign(result, data, 8);
        crypter_.Encrypt(result);
        writer_->WriteFully(result, 8);
    }

    virtual void Close_(const byte_t *data, size_t length) {
        _assert(length == 0);
    }

  public:
    DesEncrypter(const _R<ios::Writer> &writer, const ext::Buffer &key) :
        writer_(writer),
        crypter_(key)
    {
    }

    virtual void Flush() {
        writer_->Flush();
    }
};

class DesDecrypter :
    public ios::BlockWriter<8>
{
  private:
    _R<ios::Writer> writer_;
    DesCrypter crypter_;

  protected:
    virtual void Write_(const byte_t *data) {
        byte_t result[8];
        ext::CopyAssign(result, data, 8);
        crypter_.Decrypt(result);
        writer_->WriteFully(result, 8);
    }

    virtual void Close_(const byte_t *data, size_t length) {
        _assert(length == 0);
    }

  public:
    DesDecrypter(const _R<ios::Writer> &writer, const ext::Buffer &key) :
        writer_(writer),
        crypter_(key)
    {
    }

    virtual void Flush() {
        writer_->Flush();
    }
};

}

#endif//MENES_ENC_ENCRYPT_DES_HPP
