/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this Tabs of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this Tabs of conditions
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

#include "gui1/win32/win32_tabs.hpp"

#include "api/win32/error.hpp"

#include <commctrl.h>

// XXX: Fucking A-Hole!
#define SendMessage SendMessageW

namespace gui1 {
namespace Win32 {

bool TabsImpl::OnOCM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    LPNMHDR header(reinterpret_cast<LPNMHDR>(lParam));
    switch (header->code) {
        case TCN_SELCHANGE: {
            
        } break;
    }

    return ContainerImpl::OnOCM_NOTIFY(wParam, lParam, lResult);
}

TabsImpl::TabsImpl(Object &front) :
    ContainerImpl(front, WC_TABCONTROLW, true, WS_VISIBLE | WS_CHILD | TCS_HOTTRACK)
{
}

float TabsImpl::GetClientExtent(axis_t axis) {
    return ContainerImpl::GetClientExtent(axis);
}

void TabsImpl::InsertTab() {
    TCITEMW item;
    item.mask = TCIF_TEXT;
    item.pszText = L"Tab1";

    if (-1 == TabCtrl_InsertItem(handle_, 0, &item))
        throw api::Win32::Error();
}

} }
