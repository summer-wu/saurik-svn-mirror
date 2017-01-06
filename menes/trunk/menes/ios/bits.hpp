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

#ifndef MENES_IOS_BITS_HPP
#define MENES_IOS_BITS_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/streams.hpp"

namespace ios {

class BitReader :
    public ext::ReferenceCounted
{
  private:
    _R<ios::Reader> reader_;

    unsigned count_;
    byte_t buffer_;

    _finline static byte_t GetMask_(unsigned bits) {
        return ~(~byte_t() << bits);
    }

  public:
    BitReader(const _R<ios::Reader> &reader) :
        reader_(reader),
        count_(0)
    {
    }

    virtual ~BitReader() {
        if (!std::uncaught_exception())
            _assert(count_ == 0);
    }

    bool Get() {
        //OPT_TIMER("ios::BitReader::Get()")

        if (count_ == 0) {
            buffer_ = ios::Get(*reader_);
            count_ = 8;
        }

        bool value((buffer_ & 0x1) != 0);
        buffer_ >>= 1;
        --count_;

        return value;
    }

    uint32_t Get(unsigned bits) {
        //OPT_TIMER("ios::BitReader::Get(unsigned)")

        // XXX: this might should not be an _assert
        _assert(bits <= 32);

        if (bits <= count_) {
            uint32_t value(buffer_ & GetMask_(bits));

            buffer_ >>= bits;
            count_ -= bits;

            return value;
        } else {
            uint32_t value(buffer_ & GetMask_(count_));
            unsigned count(count_);

            // XXX: this could be done in a single go with a ReadFully over a casted uint32_t...
            while (bits - count > 8) {
                uint32_t byte(ios::Get(*reader_));
                value |= byte << count;
                count += 8;
            }

            unsigned need(bits - count);

            if (need == 0)
                count_ = 0;
            else {
                buffer_ = ios::Get(*reader_);
                value |= (buffer_ & GetMask_(need)) << count;
                buffer_ >>= need;
                count_ = 8 - need;
            }

            return value;
        }
    }

    void Align() {
        count_ = 0;
    }
};

class BitWriter :
    public ext::ReferenceCounted
{
  private:
    _R<ios::Writer> writer_;

    unsigned count_;
    byte_t buffer_;

  public:
    BitWriter(const _R<ios::Writer> &writer) :
        writer_(writer),
        count_(0),
        buffer_(0)
    {
    }

    virtual ~BitWriter() {
        if (!std::uncaught_exception())
            _assert(count_ == 0);
    }

    void Put(bool value) {
        buffer_ |= (value ? 1 : 0) << count_;

        if (++count_ == 8) {
            ios::Put(*writer_, buffer_);
            count_ = 0;
            buffer_ = 0;
        }
    }

    void Put(uint32_t value, unsigned bits) {
        _assert(bits <= 32);

        if (bits + count_ < 8) {
            buffer_ |= value << count_;
            count_ += bits;
        } else {
            buffer_ |= value << count_;
            value >>= count_;
            bits -= count_;

            ios::Put(*writer_, buffer_);

            while (bits >= 8) {
                ios::Put(*writer_, value);
                value >>= 8;
                bits -= 8;
            }

            buffer_ = value;
            count_ = bits;
        }
    }
};

}

#endif//MENES_IOS_BITS_HPP
