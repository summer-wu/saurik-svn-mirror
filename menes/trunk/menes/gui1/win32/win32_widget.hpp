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

#ifndef MENES_GUI1_WIN32_WIDGET
#define MENES_GUI1_WIN32_WIDGET

#include "gui1/widget.hpp"

#include "gui1/win32/win32_object.hpp"
#include "gui1/win32/win32_surface.hpp"
#include "gui1/win32/win32_wndclass.hpp"

namespace gui1 {
namespace Win32 {

class GUI1_WIN32_DECLARE WidgetImpl :
    public ObjectImpl,
    virtual public IWidgetImpl,
    public CanvasWin32,
    public Surface
{
  protected:
    bool external_;

  private:
    WindowClass wndclass_;
    int style_, styleex_;
    uint16_t childid_;

    bool subclass_;
    WNDPROC wndproc_;

    bool destroying_;

    HMENU GetMenuOrID();

  public:
    hop::Delegate<void (UINT, WPARAM, LPARAM)> OnWin32WidgetMessage;

  protected:
    virtual bool OnWM_CLOSE(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_COMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_CREATE(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_DESTROY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_KEYDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_KEYUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_LBUTTONDBLCLK(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_LBUTTONUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MBUTTONDBLCLK(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MBUTTONDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MBUTTONUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MENUCOMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MOVE(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_NCDESTROY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_NCHITTEST(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_PARENTNOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_QUIT(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_RBUTTONDBLCLK(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_RBUTTONDOWN(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_RBUTTONUP(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_SIZE(WPARAM wParam, LPARAM lParam, LRESULT &lResult);

    virtual bool OnOCM_COMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnOCM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnOCM_PARENTNOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);

    WidgetImpl(Object &front, HWND handle, bool subclass);
    WidgetImpl(Object &front, const WindowClass &wndclass, bool subclass, int style, int styleex = 0);

    virtual ~WidgetImpl();

    virtual float GetClientExtent(axis_t axis);

    virtual void SetHandle_(HWND handle);

    virtual Canvas *GetCanvas(const ext::Uuid &type);

  public:
    HWND GetHandle() const;
    static WidgetImpl *GetWidget(HWND handle);

    Widget &GetFront() const;

    virtual LRESULT Dispatch(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK Handler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    const cse::String GetWindowText() const;
    void SetWindowText(const cse::String &text);

    void ShowWindow(int command);

    longptr_t GetWindowLong(int index) const;
    longptr_t SetWindowLong(int index, longptr_t value);

    static longptr_t GetWindowLong(HWND handle, int index);
    static longptr_t SetWindowLong(HWND handle, int index, longptr_t value);

    void *GetProp(const wchar_t *atom);
    void *GetProp(const wchar_t *atom, void *value);

    void SetProp(const wchar_t *atom, void *value);
    void *RemoveProp(const wchar_t *atom);

    static void *GetProp(HWND handle, const wchar_t *atom);
    static void *GetProp(HWND handle, const wchar_t *atom, void *value);

    static void SetProp(HWND handle, const wchar_t *atom, void *value);
    static void *RemoveProp(HWND handle, const wchar_t *atom);

    virtual void Create();
    virtual void Destroy();

    virtual void DoCreate();
    virtual void DoDestroy();

    virtual bool GetVisible() const;
    virtual void SetVisible(bool visible);

    virtual void GetPosition(alg::Rectangle2f &position) const;
    virtual void SetPosition(const alg::Rectangle2f &position);
};

} }

#endif//MENES_GUI1_WIN32_WIDGET
