/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2004  Jay Freeman (saurik)
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

#ifndef MENES_API_LIBPCRE_REGEX_HPP
#define MENES_API_LIBPCRE_REGEX_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/string.hpp"
#include "ext/vector.hpp"

#include <pcre.h>

namespace api {
namespace Pcre {

class RegEx {
  private:
    typedef ext::Vector<int> Substrings_;

  public:
    class Match {
        friend class RegEx;

      private:
        cse::String::Data data_;
        Substrings_ substrings_;

        Match(const cse::String::Data &data, Substrings_ &substrings);

      public:
        Match();

        operator bool() const;
        cse::String operator [](size_t substring) const;
    };

  private:
    ::pcre *code_;
    ::pcre_extra *study_;
    int capture_;

  public:
    RegEx(const cse::String &pattern);
    ~RegEx();

    Match operator ()(const cse::String &data) const;
    Match operator ()(const char *data, size_t length) const;
};

} }

#endif//MENES_API_LIBPCRE_REGEX_HPP
