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

#ifndef MENES_GUI1_WIN32_WINDOW
#define MENES_GUI1_WIN32_WINDOW

#include "gui1/window.hpp"
#include "gui1/win32/win32_container.hpp"

namespace gui1 {
namespace Win32 {

class WindowImpl :
    public ContainerImpl,
    virtual public IWindowImpl
{
  protected:
    virtual bool OnWM_CLOSE(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_DESTROY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);
    virtual bool OnWM_MENUCOMMAND(WPARAM wParam, LPARAM lParam, LRESULT &lResult);

  public:
    explicit WindowImpl(Object &front);
    WindowImpl(Object &front, WindowClass &wndclass);

    virtual const cse::String GetTitle() const;
    virtual void SetTitle(const cse::String &title);

    virtual void Close();

    virtual Menu *GetMenu() const;
    virtual void SetMenu(Menu *menu);
};

} }

#endif//MENES_GUI1_WIN32_WINDOW
