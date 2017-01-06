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

#ifndef MENES_IOS_STDADAPTERS_HPP
#define MENES_IOS_STDADAPTERS_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/string.hpp"
#include "ext/types.hpp"

#include "ios/streams.hpp"
#include "ios/printwriter.hpp"

#include <iostream>

namespace ios {

class ToStreamBuf :
    public std::streambuf
{
  private:
    typedef std::streambuf Base_;
    typedef std::char_traits<byte_t> Traits_;

    static const size_t size_ = 1024;

    _R<Reader> reader_;
    _R<Writer> writer_;

  protected:
    virtual int underflow() {
        byte_t *back(eback());
        size_t length(reader_->Read(back, size_));
        if (length == 0)
            return Traits_::eof();
        setg(back, back, back + length);
        return *back;
    }

    virtual int_type overflow(int_type c) {
        byte_t *base(pbase());
        std::streamsize length(static_cast<std::streamsize>(pptr() - base));

        if (c != Traits_::eof())
            base[length++] = c;
        else if (length == 0)
            return c;

        // XXX: rewrite to _not_ use WriteFully()
        writer_->WriteFully(base, length);
        setp(base, base + size_);
        return c;
    }

    virtual int sync() {
        overflow(Traits_::eof());
        writer_->Flush();
        // XXX: I forgot, is this supposed to return 0?
        return 0;
    }

  public:
    ToStreamBuf(const _R<Reader> reader, const _R<Writer> writer) :
        reader_(reader),
        writer_(writer)
    {
        {
            byte_t *ptr(new byte_t[size_]);
            setg(ptr, ptr, ptr);
        }

        {
            byte_t *ptr(new byte_t[size_ + 1]);
            setp(ptr, ptr + size_);
        }
    }

    ~ToStreamBuf() {
        overflow(Traits_::eof());

        delete [] Base_::eback();
        delete [] Base_::pbase();
    }
};

class FromStreamBuf :
    public ios::Reader,
    public ios::Writer
{
  private:
    std::streambuf &buffer_;

  public:
    FromStreamBuf(std::streambuf &buffer) :
        buffer_(buffer)
    {
    }

    virtual size_t Read(byte_t *data, size_t length) {
        return buffer_.sgetn(data, static_cast<std::streamsize>(length));
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        return buffer_.sputn(data, static_cast<std::streamsize>(length));
    }

    virtual void Flush() {
        buffer_.pubsync();
    }
};

class ToIoStream :
    public std::iostream
{
  private:
    ToStreamBuf streambuf_;

  public:
    ToIoStream(Reader *reader, Writer *writer) :
        std::iostream(&streambuf_),
        streambuf_(reader, writer)
    {
    }
};

inline PrintWriter &operator <<(PrintWriter &out, std::ostream &(_w32cdecl *manip)(std::ostream &)) {
    if (manip == static_cast<std::ostream &(_w32cdecl *)(std::ostream &)>(std::endl))
        return out << NewLine;
    else if (manip == static_cast<std::ostream &(_w32cdecl *)(std::ostream &)>(std::flush))
        // XXX: douyhhh
        return out; // << Flush();
    _assert(false);
    return out;
}

inline PrintWriter &operator <<(PrintWriter &out, const std::string &value) {
    out.Write(&*value.begin(), value.size());
    return out;
}

}

template <typename Encoding_, typename Buffer_>
inline std::ostream &operator <<(std::ostream &out, const cse::UnicodeString<Buffer_> &rhs) {
    return out << Adapt(rhs);
}

#ifndef MENES_STL_NOWSTRING
/*template <typename Encoding_, typename Buffer_>
inline std::wostream &operator <<(std::wostream &out, const cse::UnicodeString &rhs) {
    return out << rhs.operator std::wstring();
}*/
#endif

template <typename Encoding_, typename Buffer_>
inline cse::UnicodeString<Encoding_, Buffer_> operator +(const std::string &lhs, const cse::UnicodeString<Encoding_, Buffer_> &rhs) {
    cse::UnicodeString<Encoding_, Buffer_> temp(lhs);
    temp.InsertAllLast(rhs);
    return temp;
}

// XXX: this is obviously in the wrong file
namespace ext {

inline std::string Adapt(const cse::String &value) {
    const cse::String::Data &data(value.GetData());
    return std::string(data.Begin(), data.GetSize());
}

inline cse::String Adapt(const std::string &value) {
    return cse::String(&*value.begin(), value.size());
}

}

#endif//MENES_IOS_STDADAPTERS_HPP
