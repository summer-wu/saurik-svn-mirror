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

#ifndef MENES_XML_XMLENCODE_HPP
#define MENES_XML_XMLENCODE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/digits.hpp"
#include "ios/streams.hpp"

#include "xml/linkage.hpp"
#include "xml/settings.hpp"

namespace xml {

struct IsWhiteSpace {
_finline static bool Execute(const cse::CodePoint &value) {
    return value == 0x20 || value == 0x9 || value == 0xd || value == 0xa;
} };

class UnknownEntityException :
    public ext::StringException
{
  public:
    UnknownEntityException(const cse::String &name) :
        ext::StringException(name)
    {
    }
};

class XmlEncoder :
    public ios::WriterWriter
{
  public:
    XmlEncoder(const _R<ios::Writer> &writer) :
        ios::WriterWriter(writer)
    {
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        _forall (size_t, i, 0, length)
            switch (data[i]) {
                case '<':
                    writer_->WriteFully("&lt;", 4);
                break;

                case '>':
                    writer_->WriteFully("&gt;", 4);
                break;

                case '"':
                    writer_->WriteFully("&quot;", 6);
                break;

                case '\'':
                    writer_->WriteFully("&apos;", 6);
                break;

                case '&':
                    writer_->WriteFully("&amp;", 5);
                break;

                default:
                    ios::Put(*writer_, data[i]);
                break;
            }

        return length;
    }
};

class XmlDecoder :
    public ios::WriterWriter
{
  private:
    const Settings &settings_;

    bool reading_;
    ext::Buffer entity_;

    bool Resolve_(const cse::String &name, cse::String &value) const {
        if (name.IsEmpty())
            return false;
        else if (name == _B("amp"))
            value = _B("&");
        else if (name == _B("apos"))
            value = _B("'");
        else if (name == _B("gt"))
            value = _B(">");
        else if (name == _B("lt"))
            value = _B("<");
        else if (name == _B("quot"))
            value = _B("\"");
        else if (name[0] == '#')
            if (name[1] == 'x')
                value = cse::CodePoint(cse::FromDigits<16, uint32_t>(name.Substring(2)));
            else
                value = cse::CodePoint(cse::FromDigits<10, uint32_t>(name.Substring(1)));
        else
            return false;

        return true;
    }

    void PassEntity_() {
        ios::Put(*writer_, '&');
        writer_->WriteFully(entity_.Begin(), entity_.GetSize());
        entity_.Clear();
    }

  public:
    XmlDecoder(const _R<ios::Writer> &writer, const Settings &settings = XmlSettings) :
        ios::WriterWriter(writer),
        settings_(settings),
        reading_(false)
    {
    }

    ~XmlDecoder() {
        if (!std::uncaught_exception())
            if (reading_)
                PassEntity_();
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        const char *begin(data);

        _forall (size_t, i, 0, length)
            if (reading_) {
                if (data[i] == '&')
                    PassEntity_();
                else if (data[i] != ';')
                    entity_.InsertLast(data[i]);
                else {
                    reading_ = false;
                    begin = data + i + 1;

                    cse::String value;
                    if (!Resolve_(entity_, value)) {
                        PassEntity_();
                        ios::Put(*writer_, ';');
                    } else {
                        writer_->WriteFully(value.GetData().Begin(), value.GetData().GetSize());
                        entity_.Clear();
                    }
                }
            } else if (data[i] == '&') {
                writer_->WriteFully(begin, data + i - begin);
                reading_ = true;
            }

        if (!reading_)
            writer_->WriteFully(begin, data + length - begin);

        return length;
    }
};

}

#endif//MENES_XML_XMLENCODE_HPP
