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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/win32/error.hpp"
#include "cse/utf16.hpp"

#include <wtypes.h>
#include <psapi.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

const char *ApiTraits::GetName() {
    return "Win32";
}

ApiTraits::ErrorCode ApiTraits::GetLastError() {
    return ::GetLastError();
}

cse::String ApiTraits::GetMessage(const ErrorCode &code) {
    HLOCAL buffer(NULL);
    if (0 == ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &buffer, 0, NULL)) {
        if (ApiTraits::GetLastError() != ERROR_MR_MID_NOT_FOUND)
            throw Error();

        // XXX: better iteration
        HMODULE modules[256];
        DWORD needed;
        if (FALSE == ::EnumProcessModules(::GetCurrentProcess(), modules, sizeof(modules), &needed))
            throw Error();

        for (size_t i(0); i != needed / sizeof(HMODULE); ++i)
            if (0 == ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, modules[i], code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &buffer, 0, NULL)) {
                Error error;
                // XXX: this should throw on catestrophic error
            } else break;
    }

    if (buffer == NULL)
        return "";

    cse::Utf8String::Data message(cse::Utf8String(cse::Utf16String(reinterpret_cast<wchar_t *>(buffer))));
    if (message.Last() == '\n')
        message.RemoveLast();
    if (message.Last() == '\r')
        message.RemoveLast();
    if (buffer == ::LocalFree(buffer))
        throw Error();
    return message;
}

} }
