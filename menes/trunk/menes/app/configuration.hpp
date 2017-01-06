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

#ifndef MENES_APP_CONFIGURATION_HPP
#define MENES_APP_CONFIGURATION_HPP

#include "cxx/platform.hpp"
#include "app/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/lexical.hpp"
#include "cse/string.hpp"

#include "ext/vector.hpp"

namespace app {

struct ConfigFlag {
    uint64_t value;
    const char *name;
};

#define APP_CONFIGFLAG_ENUM(flag) {flag, #flag}
#define APP_CONFIGFLAG_VALUE(value, name) {value, #name}
#define APP_CONFIGFLAG_END() {0, 0}

template <typename Type_>
Type_ GetConfig(const cse::String &xpath, const Type_ &defval = Type_()) {
    // XXX: default must be explicitely stated due to bug in 2005 Express
    cse::String value(GetConfig<cse::String>(xpath, cse::String()));
    return value.IsEmpty() ? defval : lexical_cast<Type_>(value);
}

template <>
APP_DECLARE cse::String GetConfig<cse::String>(const cse::String &xpath, const cse::String &defval);

template <typename Type_>
Type_ GetConfigFlag(const ConfigFlag *flags, const cse::String &xpath, const Type_ &defval = Type_()) {
    return static_cast<Type_>(GetConfigFlag<uint64_t>(flags, xpath, static_cast<uint64_t>(defval)));
}

template <>
APP_DECLARE uint64_t GetConfigFlag<uint64_t>(const ConfigFlag *flags, const cse::String &xpath, const uint64_t &defval);

template <typename Type_>
ext::Vector<Type_> GetConfigList(const cse::String &xpath, const cse::String &defval = cse::String()) {
    ext::Vector<cse::String> values(GetConfigList<cse::String>(xpath, defval));

    ext::Vector<Type_> list;
    for (ext::Vector<cse::String>::ConstIterator value(values.Begin()); value != values.End(); ++value)
        list.InsertLast(lexical_cast<Type_>(*value));
    return list;
}

template <>
APP_DECLARE ext::Vector<cse::String> GetConfigList<cse::String>(const cse::String &xpath, const cse::String &defval);

template <typename Type_>
class Config {
  private:
    Type_ value_;

  public:
    Config(const cse::String &xpath, const Type_ &defval = Type_()) :
        value_(GetConfig(xpath, defval))
    {
    }

    const Type_ &Get() const {
        return value_;
    }

    operator const Type_ &() const {
        return value_;
    }

    operator Type_ &() {
        return value_;
    }
};

/*template <typename Type_>
_finline Type_ GetStaticConfig(const cse::String &xpath, const Type_ &defval) {
    static Type_ value(GetConfig<Type_>(xpath, defval));
    return value;
}

template <typename Type_>
_finline Type_ GetStaticConfig(const char *xpath, size_t size, const Type_ &defval) {
    static Type_ value(GetConfig<Type_>(cse::String(xpath, size, ext::ReadOnlyExternal), defval));
    return value;
}*/

}

#define _C(xpath, defval) app::GetConfig(_B(xpath), defval)

#endif//MENES_APP_CONFIGURATION_HPP
