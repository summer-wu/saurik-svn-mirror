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
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSEBB
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/exename.hpp"
#include "app/configuration.hpp"
#include "ext/list.hpp"
#include "log/logging.hpp"

#if 0
#include <sstream>
#include <string>

#include <windows.h>
#include "api/win32/undefine.hpp"
#endif

namespace app {

template <>
APP_DECLARE cse::String GetConfig<cse::String>(const cse::String &xpath, const cse::String &defval) {
#if 0
    std::wstring wxpath(menes::utf8_cast(xpath));
    std::wstring::size_type slash(wxpath.find(L'/'));

    w32::DWord length, size(512);
    ext::Vector<wchar_t> buffer;

    std::wstring wdefval(menes::utf8_cast(defval));
    cse::String string(api::GetExecutablePath().GetAll() + ".ini");
    std::wstring filename(menes::utf8_cast(string));

    std::wstring section(wxpath.substr(0, slash));
    std::wstring key(wxpath.substr(slash + 1));

    do {
        buffer.SetSize(size *= 2);
        length = ::GetPrivateProfileStringW(section.c_str(), key.c_str(), wdefval.c_str(), buffer.Begin(), static_cast<DWORD>(buffer.GetSize()), filename.c_str());
    } while (length == size - 1);

    cse::String value(menes::utf8_cast(buffer.Begin(), length));
    //LOG_THREAD() << "GetConfig(): [" << menes::utf8_cast(section) << "]/" << menes::utf8_cast(key) << " = " << value << ios::NewLine;

    return value;
#else
    return defval;
#endif
}

namespace {
class list_actor {
  private:
    ext::Vector<cse::String> &list_;

  public:
    list_actor(ext::Vector<cse::String> &list) :
        list_(list)
    {
    }

    template <typename Iter_>
    void operator() (Iter_ const &begin, Iter_ const &end) const {
        list_.InsertLast(cse::String(begin, end));
    }
}; }

template <>
APP_DECLARE uint64_t GetConfigFlag<uint64_t>(const ConfigFlag *flags, const cse::String &xpath, const uint64_t &defval) {
    cse::String value(GetConfig<cse::String>(xpath));
    if (value.IsEmpty())
        return defval;

    _L<cse::String> list(ext::SplitAll(value, _B("|")));

    uint64_t combined(0);
    for (_L<cse::String>::ConstIterator string(list.Begin()); string != list.End(); ++string) {
        bool found(false);
        for (const ConfigFlag *flag(flags); flag->name != NULL; ++flag)
            if (string->Trim<cse::IsLinearWhiteSpace>() == flag->name) {
                combined |= flag->value;
                found = true;
                break;
            }

        if (!found)
            return defval;
    }

    return combined;
}

template <>
APP_DECLARE ext::Vector<cse::String> GetConfigList<cse::String>(const cse::String &xpath, const cse::String &defval) {
#if 0
    // XXX: I _need_ a std:::istream replacement
    std::istringstream values(ext::Adapt(GetConfig<cse::String>(xpath, defval)));
    ext::Vector<cse::String> list;
    for (std::string value; values >> value; list.InsertLast(ext::Adapt(value)));
    return list;
#else
    return ext::Vector<cse::String>();
#endif
}

}
