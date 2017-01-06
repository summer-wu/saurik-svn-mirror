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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/environment.hpp"
#include "api/posix/error.hpp"
#include "ext/exception.hpp"

#include <cstdlib> //OK

namespace api {
namespace Posix {

API_DECLARE Environment TheEnvironment;

cse::String Environment::Get(const cse::String &name) const {
    char *value(::getenv(name.NullTerminate()));
    if (value == NULL)
        throw ext::NotFoundException();
    return value;
}

void Environment::Set(const cse::String &name, const cse::String &value) {
    cse::String string(_S<ios::String>() << name << _B("=") << value);
    ext::AutoPointer<char> data(string.GetData().GetSize() + 1);
    ext::CopyAssign(data.GetValue(), string.GetData().Begin(), string.GetData().End());
    CheckError(::putenv(data.GetValue()));
    data.Release();
}

/*bool Environment::Insert(const cse::String &name, const cse::String &value) {
    throw ext::NotImplementedException();
}*/

void Environment::Remove(const cse::String &name) {
    ::putenv(const_cast<char *>(name.NullTerminate()));
}

} }
