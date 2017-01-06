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

#include "gui1/win32/win32_editbox.hpp"

namespace gui1 {
namespace Win32 {

EditBoxImpl::EditBoxImpl(Object &front) :
    WidgetImpl(front, L"EDIT", true, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL)
{
}

const cse::String EditBoxImpl::GetText() const {
    return GetWindowText();
}

void EditBoxImpl::SetText(const cse::String &text) {
    SetWindowText(text);
}

bool EditBoxImpl::GetPassword() const {
    return (::GetWindowLongW(handle_, GWL_STYLE) & ES_PASSWORD) != 0;
}

void EditBoxImpl::SetPassword(bool password) {
    if (password) {
        ::SetWindowLongW(handle_, GWL_STYLE, ::GetWindowLongW(handle_, GWL_STYLE) | ES_PASSWORD);
        ::SendMessageW(handle_, EM_SETPASSWORDCHAR, '*', 0);
    } else
        ::SetWindowLongW(handle_, GWL_STYLE, ::GetWindowLongW(handle_, GWL_STYLE) & ~ES_PASSWORD);
}

} }
