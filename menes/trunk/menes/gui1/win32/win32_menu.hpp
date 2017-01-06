/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_GUI1_WIN32_MENU
#define MENES_GUI1_WIN32_MENU

#include "ext/algorithm.hpp"
#include "ext/vector.hpp"

#include "gui1/menu.hpp"
#include "gui1/win32/win32_object.hpp"

namespace gui1 {
namespace Win32 {

class MenuItemImpl;

class MenuImpl :
    public ObjectImpl,
    virtual public IMenuImpl
{
  private:
    HMENU handle_;

    ext::Vector<MenuItemImpl *> items_;

  public:
    explicit MenuImpl(Object &front);

    ~MenuImpl();

    Menu &GetFront() const;

    HMENU GetHandle() const;

    static MenuImpl *GetMenu(HMENU menu);

    virtual void Create();
    virtual void Destroy();

    virtual MenuItem *Append(const cse::String &caption, Menu *submenu = NULL);
};

class MenuItemImpl :
    public MenuItem
{
  private:
    Menu &menu_;
    UINT id_;

    static UINT Id_;

  public:
    explicit MenuItemImpl(Menu &menu);

    virtual ~MenuItemImpl();

    UINT GetId() const;
    HMENU GetHandle() const;

    virtual const cse::String GetCaption() const;
    virtual void SetCaption(const cse::String &caption);

    virtual Menu *GetSubMenu() const;
    virtual void SetSubMenu(Menu *menu);
};

} }

#endif//MENES_GUI1_WIN32_MENU
