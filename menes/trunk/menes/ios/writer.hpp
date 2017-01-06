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

#ifndef MENES_IOS_WRITER_HPP
#define MENES_IOS_WRITER_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/refcount.hpp"
#include "ios/types.hpp"

namespace ios {

template <typename Element_>
class TypeWriter :
    virtual public ext::ReferenceCounted
{
  public:
    void WriteFully(const Element_ *data, size_t length) {
        while (length != 0) {
            size_t written(Write(data, length));
            data += written;
            length -= written;
        }
    }

    virtual size_t Write(const Element_ *data, size_t length) = 0;
    virtual void Flush() = 0;
};

template <typename Element_, typename Value_>
_finline void Put(TypeWriter<Element_> &writer, const Value_ &value) {
    const Element_ &element(value);
    writer.WriteFully(&element, sizeof(element));
}

template <typename Element_>
_finline void WriteFully(TypeWriter<Element_> &writer, const Element_ *data, size_t length) {
    writer.WriteFully(data, length);
}

class WriterWriter :
    public ios::Writer
{
  protected:
    _R<Writer> writer_;

  public:
    WriterWriter(const _R<Writer> &writer) :
        writer_(writer)
    {
    }

    virtual void Flush() {
        writer_->Flush();
    }

    const _R<Writer> &GetWriter() const {
        return writer_;
    }
};

}

#endif//MENES_IOS_WRITER_HPP
