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

#include "api/win32/error.hpp"
#include "gui1/win32/win32_menu.hpp"

#include <memory>

namespace gui1 {
namespace Win32 {

MenuImpl::MenuImpl(Object &front) :
    ObjectImpl(front),
    handle_(NULL)
{
}

MenuImpl::~MenuImpl() {
}

Menu &MenuImpl::GetFront() const {
    return static_cast<Menu &>(front_);
}

HMENU MenuImpl::GetHandle() const {
    return handle_;
}

MenuImpl *MenuImpl::GetMenu(HMENU menu) {
    MENUINFO info;
    info.cbSize = sizeof(MENUINFO);
    info.fMask = MIM_MENUDATA;

    if (FALSE == ::GetMenuInfo(menu, &info))
        throw api::Win32::Error();

    return reinterpret_cast<MenuImpl *>(info.dwMenuData);
}

void MenuImpl::Create() {
    _assert(handle_ == NULL);

    handle_ = ::CreateMenu();
    if (handle_ == NULL)
        throw api::Win32::Error();

    MENUINFO info;
    info.cbSize = sizeof(MENUINFO);
    info.fMask = MIM_STYLE;

    if (FALSE == ::GetMenuInfo(handle_, &info))
        throw api::Win32::Error();

    info.fMask |= MIM_MENUDATA;
    info.dwStyle |= MNS_NOTIFYBYPOS;

    info.dwMenuData = reinterpret_cast<UINT_PTR>(this);

    if (FALSE == ::SetMenuInfo(handle_, &info))
        throw api::Win32::Error();
}

void MenuImpl::Destroy() {
    ext::Delete(items_.Begin(), items_.End());
    items_.Clear();
    if (handle_ != NULL) {
        /* XXX: terrible, terrible resource leak!!!
        if (FALSE == ::DestroyMenu(handle_))
            throw api::Win32::Error();*/
        handle_ = NULL;
    }
}

MenuItem *MenuImpl::Append(const cse::String &caption, Menu *submenu) {
    std::auto_ptr<MenuItemImpl> item(new MenuItemImpl(GetFront()));

    MENUITEMINFOW info;

    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_DATA | MIIM_ID | MIIM_SUBMENU | MIIM_STRING;

    info.dwItemData = reinterpret_cast<UINT_PTR>(item.get());
    info.hSubMenu = (submenu == NULL ? NULL : dynamic_cast<MenuImpl *>(submenu->GetImpl())->GetHandle());
    info.wID = item->GetId();

    info.dwTypeData = const_cast<wchar_t *>(caption.c_str());
    info.cch = static_cast<UINT>(caption.size());

    if (FALSE == ::InsertMenuItemW(handle_, -1, TRUE, &info))
        throw api::Win32::Error();

    items_.InsertLast(item.get());
    return item.release();
}

UINT MenuItemImpl::Id_(0);

MenuItemImpl::MenuItemImpl(Menu &menu) :
    menu_(menu),
    id_(++Id_)
{
}

MenuItemImpl::~MenuItemImpl() {
    /* // XXX: this fix doesn't work :(
    SetSubMenu(NULL); */
}

UINT MenuItemImpl::GetId() const {
    return id_;
}

HMENU MenuItemImpl::GetHandle() const {
    return dynamic_cast<MenuImpl *>(menu_.GetImpl())->GetHandle();
}

const cse::String MenuItemImpl::GetCaption() const {
    MENUITEMINFOW info;
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_STRING;

    // XXX: evil static buffer...
    wchar_t caption[256];
    info.dwTypeData = caption;
    info.cch = 256;

    if (FALSE == ::GetMenuItemInfoW(GetHandle(), id_, FALSE, &info))
        throw api::Win32::Error();

    return cse::String(info.dwTypeData, info.cch);
}

void MenuItemImpl::SetCaption(const cse::String &caption) {
    MENUITEMINFOW info;
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_STRING;

    info.dwTypeData = const_cast<wchar_t *>(caption.c_str());
    info.cch = static_cast<UINT>(caption.size());

    if (FALSE == ::SetMenuItemInfoW(GetHandle(), id_, FALSE, &info))
        throw api::Win32::Error();
}

Menu *MenuItemImpl::GetSubMenu() const {
    MENUITEMINFOW info;
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_SUBMENU;

    if (FALSE == ::GetMenuItemInfoW(GetHandle(), id_, FALSE, &info))
        throw api::Win32::Error();

    return info.hSubMenu == NULL ? NULL : &MenuImpl::GetMenu(info.hSubMenu)->GetFront();
}

void MenuItemImpl::SetSubMenu(Menu *menu) {
    MENUITEMINFOW info;
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_SUBMENU;

    info.hSubMenu = (menu == NULL ? NULL : dynamic_cast<MenuImpl *>(menu->GetImpl())->GetHandle());

    if (FALSE == ::SetMenuItemInfoW(GetHandle(), id_, FALSE, &info))
        throw api::Win32::Error();
}

} }
