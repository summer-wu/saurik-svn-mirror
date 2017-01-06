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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/environment.hpp"
#include "api/win32/error.hpp"

#include "ext/exception.hpp"
#include "cse/utf16.hpp"

#include <wtypes.h>
#include <winbase.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

API_DECLARE Environment TheEnvironment;

cse::String Environment::Get(const cse::String &name) const {
    cse::Utf16String::Data buffer;
    DWORD length(256);

    do {
        buffer.SetSize(length);
        // XXX: does not throw ext::NotFoundException on ERROR_ENVVAR_NOT_FOUND
        length = CheckError(::GetEnvironmentVariableW(cse::Utf16String(name).NullTerminate(), buffer.Begin(), length));
    } while (length > buffer.GetSize());

    buffer.SetSize(length);
    return cse::Utf16String(buffer);
}

void Environment::Set(const cse::String &name, const cse::String &value) {
    CheckError(::SetEnvironmentVariableW(cse::Utf16String(name).NullTerminate(), cse::Utf16String(value).NullTerminate()));
}

/*bool Environment::Insert(const cse::String &name, const cse::String &value) {
    throw ext::NotImplementedException();
}*/

void Environment::Remove(const cse::String &name) {
    CheckError(::SetEnvironmentVariableW(cse::Utf16String(name).NullTerminate(), NULL));
}

} }
