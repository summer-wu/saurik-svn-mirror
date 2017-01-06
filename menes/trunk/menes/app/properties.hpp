/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_APP_PROPERTIES_HPP
#define MENES_APP_PROPERTIES_HPP

#include "cxx/platform.hpp"
#include "app/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "cse/string.hpp"

#include "ios/streams.hpp"

namespace app {

class Properties {
  private:
    ext::RedBlackMap<cse::String, cse::String> values_;

    class Property {
      private:
        Properties &properties_;
        cse::String &value_;

      public:
        Property(Properties &properties, cse::String &value) :
            properties_(properties),
            value_(value)
        {
        }

        Property &operator =(const cse::String &value) {
            value_ = value;
            return *this;
        }

        operator cse::String() const {
            return properties_.Replace(value_);
        }
    };

  public:
    void Read(ios::Reader &reader);
    void Read(const cse::String &file);

    cse::String Replace(const cse::String &value) const {
        // XXX: implement
        return value;
    }

    Property operator [](const cse::String &name) {
        return Property(*this, values_[name]);
    }

    cse::String operator [](const cse::String &name) const {
        return Replace(values_[name]);
    }
};

}

#endif//MENES_APP_PROPERTIES_HPP
