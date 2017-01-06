/* Menes GUI - C++ Sensible Abstract GUI Toolkit
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

#include "gui1/win32/win32_desktop.hpp"

#include "api/win32/error.hpp"

#include <commdlg.h>

namespace gui1 {
namespace Win32 {

float DesktopImpl::GetExtent_(axis_t axis, unit_t unit) const {
    _assert(unit == Pixels);

    RECT rect;
    if (FALSE == ::GetWindowRect(handle_, &rect))
        throw api::Win32::Error();

    switch (axis) {
        case X:  return float(rect.right - rect.left);
        case Y:  return float(rect.bottom - rect.top);
        default: return 0; // XXX: _assume(false) ?
    }
}

DesktopImpl::DesktopImpl(Object &front) :
    ContainerImpl(front, NULL, false)
{
    external_ = true;
}

void DesktopImpl::Create() {
    SetHandle_(::GetDesktopWindow());
}

cse::String DesktopImpl::OpenFile(const cse::String &filter) {
    wchar_t path[MAX_PATH];
    path[0] = L'\0';

    OPENFILENAMEW open;
    open.lStructSize = sizeof(OPENFILENAME);
    open.hwndOwner = NULL;
    open.hInstance = NULL;
    open.lpstrFilter = filter.c_str();
    open.lpstrCustomFilter = NULL;
    open.nMaxCustFilter = 0;
    open.nFilterIndex = 0;
    open.lpstrFile = path;
    open.nMaxFile = MAX_PATH;
    open.lpstrFileTitle = NULL;
    open.nMaxFileTitle = 0;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = NULL;
    open.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    open.nFileOffset = 0;
    open.nFileExtension = 0;
    open.lpstrDefExt = NULL;
    open.lCustData = NULL;
    open.lpfnHook = NULL;
    open.lpTemplateName = NULL;
    open.pvReserved = 0;
    open.dwReserved = 0;
    open.FlagsEx = 0;

    // XXX: get better error information
    ::GetOpenFileNameW(&open);

    return path;
}

} }
