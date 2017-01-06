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

#if 0

#include "api/win32/window.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/win32/undefine.hpp"
#include "api/win32/error.hpp"

typedef __w64 long longptr_t;

#ifndef _WIN64
#define GetWindowLongPtrW GetWindowLongW
#define SetWindowLongPtrW SetWindowLongW
#endif

extern "C" {
    WIN32_DECLARE w32::Bool WIN32_CALL DestroyWindow(w32::Window *handle);
    WIN32_DECLARE longptr_t WIN32_CALL GetWindowLongPtrW(w32::Window *handle, int index);
    WIN32_DECLARE longptr_t WIN32_CALL SendMessageW(w32::Window *handle, unsigned message, uintptr_t wparam, longptr_t lparam);
    WIN32_DECLARE longptr_t WIN32_CALL SetWindowLongPtrW(w32::Window *handle, int index, longptr_t value);
}

namespace w32 {

void Window::Destroy() {
    if (0 == ::DestroyWindow(this))
        throw Error();
}

longptr_t Window::GetLong(int index) {
    longptr_t old(::GetWindowLongPtrW(this, index));
    if (old == 0 && ApiTraits::GetLastError() != 0)
        throw Error();
    return old;
}

longptr_t Window::SetLong(int index, longptr_t value) {
    longptr_t old(::SetWindowLongPtrW(this, index, value));
    if (old == 0 && ApiTraits::GetLastError() != 0)
        throw Error();
    return old;
}

longptr_t Window::SendMessage(unsigned message, uintptr_t wparam, longptr_t lparam) {
    return ::SendMessageW(this, message, wparam, lparam);
}

}

#endif
