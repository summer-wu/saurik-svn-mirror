/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_IOS_FILTERMAP_HPP
#define MENES_IOS_FILTERMAP_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/streams.hpp"
#include "etl/limits.hpp"

namespace ios {

class FilterMap :
    public ios::WriterWriter
{
  private:
    byte_t charmap_[1 << etl::Limits<byte_t>::Bits];

  public:
    FilterMap(const _R<ios::Writer> &writer) :
        ios::WriterWriter(writer)
    {
        _forall (byte_t, i, etl::Limits<byte_t>::Min(), etl::Limits<byte_t>::Max())
            operator [](i) = i;
    }

    virtual size_t Write(const Element_ *data, size_t length) {
        _forall (size_t, i, 0, length)
            writer_->Put(operator [](data[i]));
    }

    byte_t operator [](byte_t index) const {
        size_t offset(index);
        offset -= etl::Limits<byte_t>::Min();
        return charmap_[offset];
    }

    byte_t &operator [](byte_t index) {
        size_t offset(index);
        offset -= etl::Limits<byte_t>::Min();
        return charmap_[offset];
    }
};

}

#endif//MENES_IOS_FILTERMAP_HPP
