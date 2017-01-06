/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "api/apr/module.hpp"

namespace api {
namespace Apr {

const char *DsoApiTraits::GetName() {
    return "APR-DSO";
}

DsoApiTraits::ErrorCode DsoApiTraits::GetLastError() {
    throw ext::NotImplementedException();
}

cse::String DsoApiTraits::GetMessage(const ErrorCode &code) {
    char message[1024];
    return ::apr_dso_error(code, message, sizeof(message));
}

int DsoApiTraits::ToInteger(const ErrorCode &code) {
    return static_cast<int>(reinterpret_cast<intptr_t>(code));
}

Module::Module(const cse::String &path, Type type) {
    if (::apr_dso_load(&handle_, path.NullTerminate(), pool_) != APR_SUCCESS)
        throw DsoError(handle_);
}

Module::~Module() {
    if (::apr_dso_unload(handle_) != APR_SUCCESS)
        throw DsoError(handle_);
}

void *Module::operator [](const cse::String &name) {
    ::apr_dso_handle_sym_t symbol;
    if (::apr_dso_sym(&symbol, handle_, name.NullTerminate()) != APR_SUCCESS)
        throw DsoError(handle_);
    return symbol;
}

} }
