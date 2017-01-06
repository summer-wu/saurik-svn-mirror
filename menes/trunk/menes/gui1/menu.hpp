/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2003  Jay Freeman (saurik)
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

#ifndef MENES_GUI1_MENU_HPP
#define MENES_GUI1_MENU_HPP

#include "gui1/object.hpp"

namespace gui1 {

class Menu;
class MenuItem;

class GUI1_DECLARE IMenuImpl :
    virtual public IObjectImpl
{
  public:
    virtual MenuItem *Append(const cse::String &caption, Menu *submenu = NULL) = 0;
};

class GUI1_DECLARE Menu :
    public ImplHolder<Object, IMenuImpl>
{
  public:
    static ext::Uuid TypeId;

  public:
    Menu();
    virtual ~Menu();

    MenuItem *Append(const cse::String &caption, Menu *submenu = NULL);
};

class GUI1_DECLARE MenuItem {
  public:
    hop::Delegate<void ()> OnMenuItemSelect;

  public:
    virtual ~MenuItem();

    virtual const cse::String GetCaption() const = 0;
    virtual void SetCaption(const cse::String &caption) = 0;

    virtual Menu *GetSubMenu() const = 0;
    virtual void SetSubMenu(Menu *menu) = 0;
};

}

#endif//MENES_GUI1_MENU_HPP
