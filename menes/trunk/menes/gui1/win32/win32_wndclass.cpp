/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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

#include "gui1/linkage.hpp"
#include "gui1/win32/win32_wndclass.hpp"

namespace gui1 {
namespace Win32 {

GUI1_WIN32_DECLARE HINSTANCE Instance_;

namespace {
    inline ATOM ToAtom(const wchar_t *name) {
        return reinterpret_cast<ATOM>(name);
    }

    inline const wchar_t *ToWStr(ATOM atom) {
        return reinterpret_cast<const wchar_t *>(atom);
    }
}

WindowClass::WindowClass(ATOM atom) :
    atom_(ToWStr(atom)),
    count_(false)
{
}

WindowClass::WindowClass(const wchar_t *name) :
    atom_(name),
    count_(false)
{
}

WindowClass::WindowClass(const WindowClass &wndcls) :
    atom_(wndcls.atom_),
    count_(wndcls.count_)
{
}

WindowClass::WindowClass(const cse::String &name, WNDPROC handler) {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = 0;
    wcex.lpfnWndProc = handler;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hIcon = NULL;
    wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = name.c_str();
    wcex.hIconSm = NULL;

    atom_ = ToWStr(::RegisterClassExW(&wcex));
}

WindowClass::~WindowClass() {
    if (count_.Alone())
        if (FALSE == ::UnregisterClassW(atom_, Instance_))
            if (api::Win32::ApiTraits::GetLastError() != ERROR_CLASS_DOES_NOT_EXIST)
                // XXX: WHAT THE FUCK?!?
                throw api::Win32::Error();
}

WindowClass::operator const wchar_t *() const {
    return atom_;
}

} }
