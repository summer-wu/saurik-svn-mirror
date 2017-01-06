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

#include "api/win32/handle.hpp"

#include <wtypes.h>
#include <winbase.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

void HandleManager::Clear(Value &value) {
    value = INVALID_HANDLE_VALUE;
}

bool HandleManager::IsEmpty(const Value &value) {
    return value == INVALID_HANDLE_VALUE || value == NULL;
}

bool HandleManager::IsEqual(const Value &lhs, const Value &rhs) {
    return lhs == rhs || IsEmpty(lhs) && IsEmpty(rhs);
}

void HandleManager::Take(Value &lhs, const Value &rhs) {
    w32::Handle process(::GetCurrentProcess());
    if (0 == ::DuplicateHandle(process, rhs, process, &lhs, 0, FALSE, DUPLICATE_SAME_ACCESS))
        throw Error();
#if !defined(NDEBUG) && defined(MENES_DEBUG_WIN32_HANDLE)
    dbg << "Copy: " << rhs << " -> " << lhs << ios::NewLine;
#endif
}

bool HandleManager::Drop(const Value &value) {
#if !defined(NDEBUG) && defined(MENES_DEBUG_WIN32_HANDLE)
    dbg << "Close: " << handle_ << ios::NewLine;
#endif
    // XXX: dies w/GetCurrentThread()
    if (value != reinterpret_cast<w32::Handle>(-2) && FALSE == ::CloseHandle(value))
        throw Error();
    // XXX: incorrect return semantic
    return true;
}

bool Handle::Wait(unsigned msecs) {
#if !defined(NDEBUG) && defined(MENES_DEBUG_WIN32_HANDLE)
    dbg << "Wait: " << handle_ << ios::NewLine;
#endif
    DWORD result(::WaitForSingleObject(GetValue(), msecs));
    if (result == WAIT_FAILED)
        throw Error();
    return result != WAIT_TIMEOUT;
}

} }
