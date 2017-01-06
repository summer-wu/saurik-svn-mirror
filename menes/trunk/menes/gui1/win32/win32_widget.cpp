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

#include "gui1/win32/win32_widget.hpp"

#include "api/win32/error.hpp"

#include "api/win32/window.hpp"
#include "hop/bind.hpp"

#include "gui1/container.hpp"
#include "gui1/desktop.hpp"

#include "gui1/win32/win32_keyboard.hpp"
#include "gui1/win32/win32_surface.hpp"
#include "gui1/win32/win32_toolkit.hpp"

static const UINT OCM__BASE(WM_USER + 0x1c00);

static const UINT OCM_COMMAND(WM_COMMAND + OCM__BASE);
static const UINT OCM_NOTIFY (WM_NOTIFY + OCM__BASE);
static const UINT OCM_PARENTNOTIFY (WM_PARENTNOTIFY + OCM__BASE);

#include <commctrl.h>

namespace gui1 {
namespace Win32 {

namespace {
gui1::Key MapWin32Key(WPARAM wParam, LPARAM lParam) {
    gui1::Key key(Keys_[wParam]);

    if ((HIWORD(lParam) & KF_EXTENDED) != 0)
        switch (key) {
            case gui1::Key_LAlt:     key = gui1::Key_RAlt;        break;
            case gui1::Key_LControl: key = gui1::Key_RControl;    break;
            //case gui1::Key_LShift:   key = gui1::Key_RShift;      break;

            case gui1::Key_Enter:    key = gui1::Key_Seperator;   break;

            case gui1::Key_Insert:   key = gui1::Key_NumPad0;     break;
            case gui1::Key_End:      key = gui1::Key_NumPad1;     break;
            case gui1::Key_Down:     key = gui1::Key_NumPad2;     break;
            case gui1::Key_PageDown: key = gui1::Key_NumPad3;     break;
            case gui1::Key_Left:     key = gui1::Key_NumPad4;     break;
            case gui1::Key_Right:    key = gui1::Key_NumPad6;     break;
            case gui1::Key_Up:       key = gui1::Key_NumPad8;     break;
            case gui1::Key_Home:     key = gui1::Key_NumPad7;     break;
            case gui1::Key_PageUp:   key = gui1::Key_NumPad9;     break;
        }

    if (key == gui1::Key_LShift && (lParam & 0x00ff0000) >> 16 == 0x2a)
        key = gui1::Key_RShift;

    return key;
} }

HMENU WidgetImpl::GetMenuOrID() {
    // this is utter garbage... I'm ashamed I even wrote this
    // oh, that's right -- I stole it from Microsoft :) :)
    if (childid_ == 0 && (style_ & WS_CHILD) != 0)
        childid_ = reinterpret_cast<uint16_t>(this);
    return reinterpret_cast<HMENU>(childid_);
}

bool WidgetImpl::OnWM_CLOSE(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    return false;
}

bool WidgetImpl::OnWM_COMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    SendMessageW(reinterpret_cast<HWND>(lParam), WM_COMMAND + OCM__BASE, wParam, lParam);
    return false;
}

bool WidgetImpl::OnWM_CREATE(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    GetFront().OnWidgetCreate();
    return false;
}

bool WidgetImpl::OnWM_DESTROY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    GetFront().OnWidgetDestroy();
	SetHandle_(NULL);
    return false;
}

bool WidgetImpl::OnWM_KEYDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    GetFront().OnWidgetKeyDown(MapWin32Key(wParam, lParam));
    return false;
}

bool WidgetImpl::OnWM_KEYUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    GetFront().OnWidgetKeyUp(MapWin32Key(wParam, lParam));
    return false;
}

bool WidgetImpl::OnWM_LBUTTONDBLCLK(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseDoubleClick(gui1::LeftMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseDown(gui1::LeftMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_LBUTTONUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseUp(gui1::LeftMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_MBUTTONDBLCLK(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseDoubleClick(gui1::MiddleMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_MBUTTONDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseDown(gui1::MiddleMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_MBUTTONUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseUp(gui1::MiddleMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_MENUCOMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    return false;
}

namespace {
gui1::MouseButton GetMouseButtons(unsigned state) {
    unsigned buttons(0);
    if ((state & MK_LBUTTON) != 0)
        buttons |= LeftMouseButton;
    if ((state & MK_MBUTTON) != 0)
        buttons |= MiddleMouseButton;
    if ((state & MK_RBUTTON) != 0)
        buttons |= RightMouseButton;
    return static_cast<gui1::MouseButton>(buttons);
} }

bool WidgetImpl::OnWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    gui1::MouseButton buttons(GetMouseButtons(GET_KEYSTATE_WPARAM(wParam)));
    GetFront().OnWidgetMouseMove(buttons, x, y);
    return false;
}

bool WidgetImpl::OnWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    gui1::MouseButton buttons(GetMouseButtons(GET_KEYSTATE_WPARAM(wParam)));
    GetFront().OnWidgetMouseWheel(buttons, GET_WHEEL_DELTA_WPARAM(wParam), x, y);
    return false;
}

bool WidgetImpl::OnWM_MOVE(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    return false;
}

bool WidgetImpl::OnWM_NCDESTROY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    SetHandle_(NULL);
    if (!destroying_)
        GetFront().Destroy();
    return false;
}

bool WidgetImpl::OnWM_NCHITTEST(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    return false;
}

bool WidgetImpl::OnWM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    LPNMHDR header = reinterpret_cast<LPNMHDR>(lParam);
    ::SendMessageW(header->hwndFrom, WM_NOTIFY + OCM__BASE, wParam, lParam);
    return false;
}

bool WidgetImpl::OnWM_PARENTNOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    HWND child(NULL);

    switch (LOWORD(wParam)) {
        case WM_CREATE:
        case WM_DESTROY:
            child = reinterpret_cast<HWND>(lParam);
        break;
        default:
            // XXX: child = GetDlgItem(HIWORD(wParam));
        break;
    }

    if (child != NULL)
        ::SendMessageW(child, WM_PARENTNOTIFY + OCM__BASE, wParam, lParam);
    return false;
}

bool WidgetImpl::OnWM_QUIT(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    return false;
}

bool WidgetImpl::OnWM_RBUTTONDBLCLK(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseDoubleClick(gui1::RightMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_RBUTTONDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseDown(gui1::RightMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_RBUTTONUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    float x(LOWORD(lParam) / GetFront().GetDesktop()->GetExtent(X, Pixels));
    float y(HIWORD(lParam) / GetFront().GetDesktop()->GetExtent(Y, Pixels));
    GetFront().OnWidgetMouseUp(gui1::RightMouseButton, x, y);
    return false;
}

bool WidgetImpl::OnWM_SIZE(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    return false;
}

bool WidgetImpl::OnOCM_COMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    _assert(reinterpret_cast<HWND>(lParam) == handle_);
    return false;
}

bool WidgetImpl::OnOCM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    LPNMHDR header(reinterpret_cast<LPNMHDR>(lParam));
    _assert(header->hwndFrom == handle_);
    return false;
}

bool WidgetImpl::OnOCM_PARENTNOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    if (LOWORD(lParam) == WM_CREATE)
        GetFront().OnWidgetCreate();
    return false;
}

WidgetImpl::WidgetImpl(Object &front, HWND handle, bool subclass) :
    ObjectImpl(front),
    external_(false),
    wndclass_(static_cast<ATOM>(::GetClassLong(handle, GCW_ATOM))),
    style_(0),
    styleex_(0),
    childid_(0),
    subclass_(subclass),
    wndproc_(NULL),
    destroying_(false)
{
    SetHandle_(handle);
}

WidgetImpl::WidgetImpl(Object &front, const WindowClass &wndclass, bool subclass, int style, int styleex) :
    ObjectImpl(front),
    external_(false),
    wndclass_(wndclass),
    style_(style),
    styleex_(styleex),
    childid_(0),
    subclass_(subclass),
    wndproc_(NULL),
    destroying_(false)
{
}

WidgetImpl::~WidgetImpl() {
    Destroy();
}

float WidgetImpl::GetClientExtent(axis_t axis) {
    RECT rect;
    if (FALSE == ::GetClientRect(handle_, &rect))
        throw api::Win32::Error();

    float extent(GetFront().GetDesktop()->GetExtent(axis, Pixels));

    switch (axis) {
        case X:  return (rect.right - rect.left) / extent;
        case Y:  return (rect.bottom - rect.top) / extent;
        default: return 0; // XXX: _assume(false) ?
    }
}

void WidgetImpl::SetHandle_(HWND handle) {
    if (handle == handle_)
        return;

    if (handle_ != NULL && !external_) {
        if (subclass_)
            SetWindowLong(GWLP_WNDPROC, reinterpret_cast<uintptr_t>(wndproc_));
        RemoveProp(L"ThurisImpl");
    }

    CanvasWin32::SetHandle_(handle);

    if (handle != NULL && !external_) {
        style_ = static_cast<int>(GetWindowLong(GWL_STYLE));

        // XXX: *sigh*... fix this first
        // styleex_ = GetWindowLong(GWL_EXSTYLE);

        SetProp(L"ThurisImpl", reinterpret_cast<HANDLE>(this));
        if (subclass_)
            wndproc_ = reinterpret_cast<WNDPROC>(SetWindowLong(GWLP_WNDPROC, reinterpret_cast<uintptr_t>(WidgetImpl::Handler)));
    }
}

Canvas *WidgetImpl::GetCanvas(const ext::Uuid &type) {
    if (type == CanvasWin32::TypeId)
        return this;
    else if (type == Canvas2d::TypeId)
        return this;
    return NULL;
}

HWND WidgetImpl::GetHandle() const {
    return handle_;
}

WidgetImpl *WidgetImpl::GetWidget(HWND handle) {
    return reinterpret_cast<WidgetImpl *>(GetProp(handle, L"ThurisImpl", NULL));
}

Widget &WidgetImpl::GetFront() const {
    return static_cast<Widget &>(front_);
}

LRESULT WidgetImpl::Dispatch(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    #define MapMessage(msg) \
        case msg: \
            if (On ## msg(wParam, lParam, lResult)) \
                return lResult; \
        break;

    LRESULT lResult(0);

    OnWin32WidgetMessage(uMsg, wParam, lParam);

    WNDPROC wndproc(wndproc_);
    HWND handle(handle_);

    switch (uMsg) {
        MapMessage(WM_CLOSE)
        MapMessage(WM_COMMAND)
        MapMessage(WM_CREATE)
        MapMessage(WM_DESTROY)
        MapMessage(WM_KEYDOWN)
        MapMessage(WM_KEYUP)
        MapMessage(WM_LBUTTONDBLCLK)
        MapMessage(WM_LBUTTONDOWN)
        MapMessage(WM_LBUTTONUP)
        MapMessage(WM_MBUTTONDBLCLK)
        MapMessage(WM_MBUTTONDOWN)
        MapMessage(WM_MBUTTONUP)
        //MapMessage(WM_MBUTTONDBLCLK)
        MapMessage(WM_MENUCOMMAND)
        MapMessage(WM_MOUSEMOVE)
        MapMessage(WM_MOUSEWHEEL)
        MapMessage(WM_MOVE)
        MapMessage(WM_NCDESTROY)
        MapMessage(WM_NCHITTEST)
        MapMessage(WM_NOTIFY)
        MapMessage(WM_PARENTNOTIFY)
        MapMessage(WM_QUIT)
        MapMessage(WM_RBUTTONDBLCLK)
        MapMessage(WM_RBUTTONDOWN)
        MapMessage(WM_RBUTTONUP)
        MapMessage(WM_SIZE)

        MapMessage(OCM_COMMAND)
        MapMessage(OCM_NOTIFY)
        MapMessage(OCM_PARENTNOTIFY)
    };

    // WARNING: 'this' might be invalid!!

    #undef MapReflect
    #undef MapMessage
    return wndproc == NULL ?
        ::DefWindowProcW(handle, uMsg, wParam, lParam) :
        ::CallWindowProcW(wndproc, handle, uMsg, wParam, lParam);

    /*IMessageDelegate *delegate = messages_[uMsg];
    if (delegate != NULL)
        (*delegate)(uMsg, wParam, lParam);*/
}

LRESULT WidgetImpl::Handler(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WidgetImpl *widget(GetWidget(handle));

    /* menes::win32::dbg << std::hex;
    menes::win32::dbg << handle << "(" << widget << ") [0x" << uMsg << ": (0x" << wParam << ", 0x" << lParam << ")]" << ios::NewLine;
    menes::win32::dbg << std::dec; */

    if (widget == NULL && (uMsg == WM_NCCREATE || uMsg == WM_CREATE)) {
        CREATESTRUCT *create = reinterpret_cast<CREATESTRUCT *>(lParam);
        widget = reinterpret_cast<WidgetImpl *>(create->lpCreateParams);
        widget->SetHandle_(handle);
        //dbg << "  Fixup (" << window << ')' << ios::NewLine;
    }

    // XXX: wtf? why do I need to do the -1 crap
    return (widget == NULL /*|| widget == reinterpret_cast<WidgetImpl *>(-1)*/) ?
        ::DefWindowProcW(handle, uMsg, wParam, lParam) :
        widget->Dispatch(uMsg, wParam, lParam);
}

const cse::String WidgetImpl::GetWindowText() const {
    wchar_t text[1024];

    int count(::GetWindowTextW(handle_, text, 1024));
    if (count == 0 && api::Win32::ApiTraits::GetLastError() != 0)
        throw api::Win32::Error();

    return cse::String(text, count);
}

void WidgetImpl::SetWindowText(const cse::String &text) {
    if (FALSE == ::SetWindowTextW(handle_, text.c_str()))
    // XXX: WTF?!?
        if (api::Win32::ApiTraits::GetLastError() != ERROR_SUCCESS)
            throw api::Win32::Error();
}

void WidgetImpl::ShowWindow(int command) {
    if (FALSE == ::ShowWindow(handle_, command))
        throw api::Win32::Error();
}

longptr_t WidgetImpl::GetWindowLong(int index) const {
    return GetWindowLong(handle_, index);
}

longptr_t WidgetImpl::SetWindowLong(int index, longptr_t value) {
    return SetWindowLong(handle_, index, value);
}

longptr_t WidgetImpl::GetWindowLong(HWND handle, int index) {
    longptr_t old(::GetWindowLongPtrW(handle, index));
    if (old == 0 && api::Win32::ApiTraits::GetLastError() != 0)
        throw api::Win32::Error();
    return old;
}

longptr_t WidgetImpl::SetWindowLong(HWND handle, int index, longptr_t value) {

#if _MSC_VER >= 1300
// Faulty 64-bit Compatibility Check
#pragma warning(push)
// C4244: '...' : conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4244)
#endif

    longptr_t old(::SetWindowLongPtrW(handle, index, value));

#if _MSC_VER >= 1300
#pragma warning(pop)
#endif

    if (old == 0 && api::Win32::ApiTraits::GetLastError() != 0)
        throw api::Win32::Error();
    return old;
}

void *WidgetImpl::GetProp(const wchar_t *atom, void *value) {
    return GetProp(handle_, atom, value);
}

void *WidgetImpl::GetProp(const wchar_t *atom) {
    return GetProp(handle_, atom);
}

void WidgetImpl::SetProp(const wchar_t *atom, void *value) {
    return SetProp(handle_, atom, value);
}

void *WidgetImpl::RemoveProp(const wchar_t *atom) {
    return RemoveProp(handle_, atom);
}

void *WidgetImpl::GetProp(HWND handle, const wchar_t *atom) {
    HANDLE current(::GetPropW(handle, atom));
    if (current == NULL && api::Win32::ApiTraits::GetLastError() == ERROR_FILE_NOT_FOUND)
        throw api::Win32::Error();
    return current;
}

void *WidgetImpl::GetProp(HWND handle, const wchar_t *atom, void *value) {
    HANDLE current(::GetPropW(handle, atom));
    if (current == NULL && api::Win32::ApiTraits::GetLastError() == ERROR_FILE_NOT_FOUND)
        return value;
    return current;
}

void WidgetImpl::SetProp(HWND handle, const wchar_t *atom, void *value) {
    if (FALSE == ::SetPropW(handle, atom, value))
        throw api::Win32::Error();
}

void *WidgetImpl::RemoveProp(HWND handle, const wchar_t *atom) {
    HANDLE value(::RemovePropW(handle, atom));
    if (value == NULL) // XXX: this might be bogus...
        throw api::Win32::Error();
    return value;
}

void WidgetImpl::Create() {
    Toolkit_.Borrow(hop::BindAll(&WidgetImpl::DoCreate, this));
}

void WidgetImpl::Destroy() {
    Toolkit_.Borrow(hop::BindAll(&WidgetImpl::DoDestroy, this));
}

void WidgetImpl::DoCreate() {
    if (handle_ != NULL)
        return;

    Container *parent(GetFront().GetParent());
    _assert(parent != NULL);

    CanvasWin32 *surface(static_cast<CanvasWin32 *>(parent->GetClientSurface().GetCanvas(CanvasWin32::TypeId)));
    _assert(surface != NULL);

    HMENU id(GetMenuOrID());

    float pixelsX(parent->GetDesktop()->GetExtent(X, Pixels));
    float pixelsY(parent->GetDesktop()->GetExtent(Y, Pixels));

    // XXX: important! these need to be rounded
    float left(0); //GetFront().Left() * pixelsX);
    float top(0); //GetFront().Top() * pixelsY);
    float width(100); //GetFront().Width() * pixelsX);
    float height(100); //GetFront().Height() * pixelsY);

    HWND handle = ::CreateWindowExW(styleex_, wndclass_, L"", style_, left, top, width, height, surface->GetHandle(), id, NULL, reinterpret_cast<void *>(this));
    if (handle == NULL)
        throw api::Win32::Error();

    _assert(::IsWindowUnicode(handle) != FALSE);

    if (handle_ == NULL)
        SetHandle_(handle);
    else _assert(handle_ == handle);

    RECT rect;
    ::GetClientRect(handle_, &rect);
}

void WidgetImpl::DoDestroy() {
	if (handle_ != NULL && !external_ && FALSE != ::IsWindow(handle_)) {
        /* // XXX: this isn't quite right...
        HWND handle(handle_);
        SetHandle_(NULL);*/

        destroying_ = true;
        if (FALSE == ::DestroyWindow(handle_))
            throw api::Win32::Error();
    }
}

bool WidgetImpl::GetVisible() const {
    return ::IsWindowVisible(handle_) == FALSE ? false : true;
}

void WidgetImpl::SetVisible(bool visible) {
    ::ShowWindow(handle_, visible ? SW_SHOW : SW_HIDE);
}

void WidgetImpl::GetPosition(alg::Rectangle2f &position) const {
    Desktop *desktop(GetFront().GetDesktop());
    _assert(desktop != NULL);

    float pixelsX(desktop->GetExtent(X, Pixels));
    float pixelsY(desktop->GetExtent(Y, Pixels));

    RECT rect;
    if (FALSE == ::GetWindowRect(handle_, &rect))
        throw api::Win32::Error();

    HWND parent(dynamic_cast<WidgetImpl *>(GetFront().GetParent()->GetImpl())->GetHandle());

    POINT loc;

    loc.x = rect.left;
    loc.y = rect.top;
    if (FALSE == ::ScreenToClient(parent, &loc))
        throw api::Win32::Error();
    rect.left = loc.x;
    rect.top = loc.y;

    loc.x = rect.right;
    loc.y = rect.bottom;
    if (FALSE == ::ScreenToClient(parent, &loc))
        throw api::Win32::Error();
    rect.right = loc.x;
    rect.bottom = loc.y;

    position.SetL(rect.left / pixelsX);
    position.SetR(rect.right / pixelsX);

    position.SetT(rect.top / pixelsY);
    position.SetB(rect.bottom / pixelsY);
}

void WidgetImpl::SetPosition(const alg::Rectangle2f &position) {
    Desktop *desktop(GetFront().GetDesktop());
    _assert(desktop != NULL);

    float pixelsX(desktop->GetExtent(X, Pixels));
    float pixelsY(desktop->GetExtent(Y, Pixels));

    if (FALSE == ::MoveWindow(handle_, position.Left() * pixelsX, position.Top() * pixelsY, position.Width() * pixelsX, position.Height() * pixelsY, TRUE))
        throw api::Win32::Error();
}

} }
