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

#include "api/win32/error.hpp"
#include "api/win32/module.hpp"

#include "cse/utf16.hpp"

#include <windows.h>
#include <dbghelp.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

Module::Module(const cse::String &name, Type type) {
    //if (load) {
        handle_ = ::LoadLibraryExW(cse::Utf16String(name).NullTerminate(), NULL, type == Data ? LOAD_LIBRARY_AS_DATAFILE : 0);
        if (handle_ == NULL)
            throw Error();
        /*if (0 == ::SymLoadModule(::GetCurrentProcess(), NULL, const_cast<PSTR>(path.c_str()), NULL, 0, 0))
            if (ApiTraits::GetLastError() != 0)
                throw Error();*/
    /*} else {
        _assert(false);
        // XXX: this didn't work on Windows 2000 and we needed to go to GDC
        //CheckError(::GetModuleHandleExW(0, menes::cse::Utf16String(name).NullTerminate(), &handle_));
    }*/
}

Module::~Module() {
    CheckError(::FreeLibrary(handle_));
}

void *Module::operator [](const cse::String &name) {
    void *addr(::GetProcAddress(handle_, name.NullTerminate()));
    if (addr == NULL) {
        ApiTraits::ErrorCode code(ApiTraits::GetLastError());
        if (code != ERROR_PROC_NOT_FOUND && code != ERROR_MOD_NOT_FOUND)
            throw Error();
    }

    return addr;
}

void *Module::operator [](int ordinal) {

#if _MSC_VER >= 1300
#pragma warning(push)
// C4312: '...' : conversion from '...' to '...' of greater size
#pragma warning(disable: 4312)
#endif

    void *addr(::GetProcAddress(handle_, reinterpret_cast<LPCSTR>(ordinal)));

#if _MSC_VER >= 1300
#pragma warning(pop)
#endif

    if (addr == NULL && ApiTraits::GetLastError() != ERROR_MOD_NOT_FOUND)
        throw Error();
    return addr;
}

} }
