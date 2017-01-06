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

#ifndef MENES_IOS_READER_HPP
#define MENES_IOS_READER_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/block.hpp"
#include "ext/exception.hpp"
#include "ext/refcount.hpp"

#include "ios/types.hpp"

namespace ios {

template <typename Element_>
class IOS_DECLARE TypeReader :
    public virtual ext::ReferenceCounted
{
  public:
    class InputIterator;

  public:
    bool Get(Element_ &value) {
        return Read(&value, 1) != 0;
    }

    size_t ReadMost(Element_ *data, size_t length) {
        size_t rest(length);

        while (rest != 0) {
            size_t read(Read(data, rest));
            if (read == 0)
                return length - rest;
            data += read;
            rest -= read;
        }

        return length;
    }

    void ReadFully(Element_ *data, size_t length) {
        if (ReadMost(data, length) != length)
            throw ext::EosException();
    }

    void SeekFully(offset_t distance) {
        while (distance != 0) {
            offset_t seek(Seek(distance));
            if (seek == 0)
                throw ext::EosException();
            distance -= seek;
        }
    }

    virtual size_t Read(Element_ *data, size_t length) = 0;

    virtual offset_t Seek(offset_t distance) {
        if (distance < 0)
            throw ext::NotImplementedException();

        // XXX: this can definitely be optimized
        ext::Block<Element_> buffer(distance);
        return Read(buffer.Begin(), distance);
    }

    InputIterator GetIterator() {
        return InputIterator(*this);
    }
};

template <typename Element_>
_finline Element_ Get(TypeReader<Element_> &reader) {
    Element_ value;
    if (!reader.Get(value))
        throw ext::EosException();
    return value;
}

template <typename Element_>
_finline size_t Read(TypeReader<Element_> &reader, Element_ *data, size_t length) {
    return reader.Read(data, length);
}

template <typename Element_>
_finline void ReadFully(TypeReader<Element_> &reader, Element_ *data, size_t length) {
    reader.ReadFully(data, length);
}

// XXX: this is not technically an iterator
template <typename Element_>
class TypeReader<Element_>::InputIterator {
  public:
    typedef Element_ Indirect;

  private:
    TypeReader<Element_> &stream_;

    mutable bool read_;
    mutable Element_ data_;

  public:
    InputIterator(TypeReader<Element_> &stream) :
        stream_(stream),
        read_(false)
    {
    }

    InputIterator(const InputIterator &rhs) :
        stream_(rhs.stream_),
        read_(false)
    {
        _assert(!rhs.read_);
    }

    ~InputIterator() {
        if (!std::uncaught_exception())
            _assert(!read_);
    }

    Element_ operator *() const {
        if (!read_) {
            read_ = true;
            data_ = ios::Get(stream_);
        }

        return data_;
    }

    InputIterator &operator ++() {
        read_ = false;
        return *this;
    }
};


class ReaderReader :
    public ios::Reader
{
  protected:
    _R<Reader> reader_;

  public:
    ReaderReader(const _R<Reader> &reader) :
        reader_(reader)
    {
    }

    const _R<Reader> &GetReader() const {
        return reader_;
    }

    virtual size_t Read(byte_t *data, size_t length) {
        return reader_->Read(data, length);
    }
};

}

#endif//MENES_IOS_READER_HPP
