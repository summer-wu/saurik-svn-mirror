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

#ifndef MENES_WAF_ESCAPE_HPP
#define MENES_WAF_ESCAPE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/streams.hpp"

namespace waf {

class Escape :
    public ios::WriterWriter
{
  private:
    enum State_ {
        Normal = ' ',
        Ampersand = '&',
        Bracket = '<'
    } state_;

  public:
    Escape(const _R<ios::Writer> &writer) :
        ios::WriterWriter(writer),
        state_(Normal)
    {
    }

    ~Escape() {
        if (state_ == Ampersand)
            writer_->WriteFully("&amp;", 5);
        else if (state_ == Bracket)
            writer_->WriteFully("&lt;", 4);
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        OPT_TIMER("waf::Escape::Write()")

        const char *begin(data);

        for (size_t i(0); i != length; ++i) {
            byte_t value(data[i]);

            switch (state_) {
                case Normal:
                Output:
                    if (value == '&')
                        state_ = Ampersand;
                    else if (value == '<')
                        state_ = Bracket;
                    else
                        ios::Put(*writer_, value);
                break;

                case Ampersand:
                    if (value == '&')
                        writer_->WriteFully("&amp;", 5);
                    else {
                        if (cse::IsWhiteSpace::Execute(value))
                            writer_->WriteFully("&amp;", 5);
                        else
                            ios::Put(*writer_, '&');

                        state_ = Normal;
                        goto Output;
                    }
                break;

                case Bracket:
                    if (value == '<')
                        writer_->WriteFully("&lt;", 4);
                    else {
                        if (
                            (value < 'A' || value > 'Z') &&
                            (value < 'a' || value > 'z') &&
                            value != '/' && value != '?' && value != '!'
                        )
                            writer_->WriteFully("&lt;", 4);
                        else
                            ios::Put(*writer_, '<');

                        state_ = Normal;
                        goto Output;
                    }
                break;
            }
        }

        return length;
    }
};

}

#endif//MENES_WAF_ESCAPE_HPP
