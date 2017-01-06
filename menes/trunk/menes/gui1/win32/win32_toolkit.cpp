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

#include "gui1/win32/win32_toolkit.hpp"

#include "gui1/win32/win32_button.hpp"
#include "gui1/win32/win32_checkbox.hpp"
#include "gui1/win32/win32_combobox.hpp"
#include "gui1/win32/win32_desktop.hpp"
#include "gui1/win32/win32_editbox.hpp"
#include "gui1/win32/win32_frame.hpp"
#include "gui1/win32/win32_group.hpp"
#include "gui1/win32/win32_listbox.hpp"
#include "gui1/win32/win32_menu.hpp"
#include "gui1/win32/win32_progress.hpp"
#include "gui1/win32/win32_static.hpp"
#include "gui1/win32/win32_tabs.hpp"
#include "gui1/win32/win32_treebox.hpp"
#include "gui1/win32/win32_window.hpp"

#include "api/win32/locks.hpp"
#include "log/logging.hpp"
#include "opt/profile.hpp"

namespace gui1 {
namespace Win32 {

static const UINT MSG_BORROW_THREAD(WM_USER + 432);
static const UINT MSG_IDLE_KICK(WM_USER + 433);

ToolkitImpl Toolkit_;

ToolkitImpl::ToolkitImpl() :
    thread_(0)
{
	INITCOMMONCONTROLSEX init;
	init.dwSize = sizeof(init);
	init.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;

	if (FALSE == ::InitCommonControlsEx(&init))
        throw api::Win32::Error();
}

cse::String ToolkitImpl::GetName() const {
    return L"NativeWin32";
}

IObjectImpl *ToolkitImpl::Instance(Object &front) {
    const ext::Uuid &type(front.GetType());

    if (type == ext::Uuid::Null)
        return NULL;

    #define ImplementType(Type) \
    else if (type == Type::TypeId) \
        return new Type ## Impl(front);

    ImplementType(Button)
    ImplementType(CheckBox)
    ImplementType(ComboBox)
    ImplementType(Desktop)
    ImplementType(EditBox)
    ImplementType(Frame)
    ImplementType(Group)
    ImplementType(ListBox)
    ImplementType(Menu)
    ImplementType(Progress)
    ImplementType(Static)
    ImplementType(Tabs)
    ImplementType(TreeBox)
    ImplementType(Window)
    #undef ImplementType

    return NULL;
}

int ToolkitImpl::Run() {
    DWORD thread(::GetCurrentThreadId());
    _assert(thread_ == 0 || thread_ == thread);

    for (MSG msg;;) {
        if (FALSE == ::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            if (OnToolkitIdle.GetSize() == 0) {
                OPT_TIMER("gui1::Win32::Toolkit::Run()[GetMessage]")
                if (-1 == ::GetMessageW(&msg, NULL, 0, 0))
                    throw api::Win32::Error();
            } else {
                OnToolkitIdle();
                continue;
            }

        if (msg.hwnd != NULL) {
            OPT_TIMER("gui1::Win32::Toolkit::Run()[Trans/Dispat]")
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        } else switch (msg.message) {
            case MSG_BORROW_THREAD: {
                OPT_TIMER("gui1::Win32::Toolkit::Run()[BorrowThread]")
                (*reinterpret_cast<const hop::Function<void ()> *>(msg.wParam))();
                reinterpret_cast<api::Win32::Event *>(msg.lParam)->Set();
            } break;

            case MSG_IDLE_KICK:
            break;

            case WM_QUIT:
                return static_cast<int>(msg.wParam);
            break;

            default: {
                OPT_TIMER("gui1::Win32::Toolkit::Run()[DispatchMsgW]")
                ::DispatchMessageW(&msg);
            } break;
        }
    }
}

void ToolkitImpl::Quit(int result) {
    ::PostQuitMessage(result);
}

void ToolkitImpl::Borrow(const hop::Function<void ()> &call) {
    if (thread_ == 0 || ::GetCurrentThreadId() == thread_)
        call();
    else {
        api::Win32::Event event;
        if (FALSE == ::PostThreadMessage(thread_, MSG_BORROW_THREAD, reinterpret_cast<WPARAM>(&call), reinterpret_cast<LPARAM>(&event)))
            throw api::Win32::Error();
        event.Wait();
    }
}

void ToolkitImpl::IdleKick() {
    if (thread_ == 0 || ::GetCurrentThreadId() == thread_)
        return;
    if (FALSE == ::PostThreadMessage(thread_, MSG_IDLE_KICK, 0, 0))
        throw api::Win32::Error();
}

// XXX: this function is damned; please avoid it
void ToolkitImpl::SetCursor(bool visible) {
    if (visible)
        ::ShowCursor(TRUE);
    else while (::ShowCursor(FALSE) >= 0);
}

} }
