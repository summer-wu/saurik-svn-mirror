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

#ifndef MENES_IOS_BUFFER_HPP
#define MENES_IOS_BUFFER_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"

#include "ios/printreader.hpp"
#include "ios/streams.hpp"
#include "ios/printwriter.hpp"

namespace ios {

class Buffer :
    public ext::Buffer,
    public ios::PrintReader,
    public ios::PrintWriter
{
  private:
    typedef Buffer This_;

    virtual void WriteFully_(const byte_t *data, size_t length) {
        InsertAllLast(data, length);
    }

  public:
    Buffer() {
    }

    template <typename IteratorF_>
    Buffer(IteratorF_ begin, size_t size) :
        ext::Buffer(begin, size)
    {
    }

    Buffer(const ext::Buffer &rhs) :
        ext::Buffer(rhs)
    {
    }

    Buffer(size_t size) :
        ext::Buffer(size)
    {
    }

    This_ &operator =(const cse::String &rhs) {
        ext::Buffer::operator =(rhs.GetData());
        return *this;
    }

    This_ &operator =(const ext::Buffer &rhs) {
        ext::Buffer::operator =(rhs);
        return *this;
    }

    virtual size_t Read(byte_t *data, size_t length) {
        if (length > GetSize())
            length = GetSize();
        ext::CopyAssign(data, Begin(), length);
        // XXX: this is lame
        _forall (size_t, i, 0, length)
            RemoveFirst();
        return length;
    }

    void WriteTo(ios::Writer &stream) const {
        stream.WriteFully(Begin(), GetSize());
    }

    /*template <typename Type_>
    Buffer &operator >>(const Type_ &value) {
        *static_cast<ios::PrintReader *>(this) << value;
        return *this;
    }*/

    template <typename Type_>
    Buffer &operator <<(const Type_ &value) {
        *static_cast<ios::PrintWriter *>(this) << value;
        return *this;
    }
};

}

#endif//MENES_IOS_BUFFER_HPP
