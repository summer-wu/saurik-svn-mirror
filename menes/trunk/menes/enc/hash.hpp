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

#ifndef MENES_ENC_HASH_HPP
#define MENES_ENC_HASH_HPP

#include "cxx/platform.hpp"
#include "enc/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"
#include "ios/block.hpp"

namespace enc {

class HashWriter :
    public ios::BlockWriter<64>
{
  private:
    typedef ios::BlockWriter<64> Base_;

  protected:
    _R<ios::Writer> writer_;
    uint64_t bits_;

    virtual void Write_(const char *data) = 0;
    virtual void Close_(const char *data, size_t length);

    virtual void WriteHash_() const = 0;
    virtual uint64_t Encode_(uint64_t value) const = 0;

  public:
    HashWriter(const _R<ios::Writer> &writer);

    virtual size_t Write(const byte_t *data, size_t length);

    virtual void Flush() {
        writer_->Flush();
    }
};

template <size_t Size_, typename Order_>
class HashWriterImpl :
    public HashWriter
{
  public:
    static const size_t Size = Size_;

  protected:
    uint32_t hash_[Size / sizeof(uint32_t)];

    virtual void WriteHash_() const {
        _forall (size_t, i, 0, Size / sizeof(uint32_t)) {
            uint32_t value(Order_::Encode(hash_[i]));
            writer_->WriteFully(reinterpret_cast<byte_t *>(&value), sizeof(value));
        }
    }

    virtual uint64_t Encode_(uint64_t value) const {
        return Order_::Encode(value);
    }

  public:
    HashWriterImpl(const _R<ios::Writer> &writer) :
        HashWriter(writer)
    {
    }

    size_t GetSize() const {
        return Size;
    }
};

}

#endif//MENES_ENC_HASH_HPP
