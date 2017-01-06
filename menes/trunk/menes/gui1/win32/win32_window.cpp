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

#include "gui1/win32/win32_window.hpp"
#include "gui1/win32/win32_menu.hpp"

namespace gui1 {
namespace Win32 {

namespace {

WindowClass WndClass_(L"ThurisWindow", WidgetImpl::Handler);

}

bool WindowImpl::OnWM_CLOSE(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    static_cast<Window &>(front_).OnWindowClose();
    return WidgetImpl::OnWM_CLOSE(wParam, lParam, lResult);
}

bool WindowImpl::OnWM_DESTROY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    SetMenu(NULL);
    return WidgetImpl::OnWM_DESTROY(wParam, lParam, lResult);
}

bool WindowImpl::OnWM_MENUCOMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    HMENU menu(reinterpret_cast<HMENU>(lParam));

    MENUITEMINFOW info;
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_DATA;

    if (FALSE == ::GetMenuItemInfoW(menu, static_cast<UINT>(wParam), TRUE, &info))
        throw api::Win32::Error();

    reinterpret_cast<MenuItemImpl *>(info.dwItemData)->OnMenuItemSelect();

    return ContainerImpl::OnWM_MENUCOMMAND(wParam, lParam, lResult);
}

WindowImpl::WindowImpl(Object &front) :
    ContainerImpl(front, WndClass_, false, CS_DBLCLKS | WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_SYSMENU /*& ~WS_CLIPCHILDREN*/)
{
}

WindowImpl::WindowImpl(Object &front, WindowClass &wndclass) :
    ContainerImpl(front, wndclass, true, CS_DBLCLKS | WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_SYSMENU /*& ~WS_CLIPCHILDREN*/)
{
}

const cse::String WindowImpl::GetTitle() const {
    return GetWindowText();
}

void WindowImpl::SetTitle(const cse::String &title) {
    SetWindowText(title);
}

void WindowImpl::Close() {
    ::PostMessageW(handle_, WM_CLOSE, 0, 0);
}

Menu *WindowImpl::GetMenu() const {
    _assert(false);
    return NULL;
}

void WindowImpl::SetMenu(Menu *menu) {
    HMENU handle(menu == NULL ? NULL : dynamic_cast<MenuImpl *>(menu->GetImpl())->GetHandle());
    if (FALSE == ::SetMenu(GetHandle(), handle))
        throw api::Win32::Error();
}

} }
