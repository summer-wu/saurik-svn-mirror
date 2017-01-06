/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#include "gui1/menu.hpp"
#include "gui1/surface.hpp"

namespace gui1 {

ext::Uuid Menu::TypeId(0xc8c2ec1f, 0x8ec2, 0x40bc, 0x9e, 0xb8, 0x01, 0x29, 0x3c, 0xa2, 0x96, 0x41);

Menu::Menu() :
    ImplHolder<Object, IMenuImpl>(TypeId)
{
}

Menu::~Menu() {
}

MenuItem *Menu::Append(const cse::String &caption, Menu *submenu) {
    _assert(impl_ != NULL);
    return impl_->Append(caption, submenu);
}

//ext::Uuid MenuItem::TypeId(0x73f03c2b, 0x6db1, 0x4f36, 0x89, 0x2c, 0x3a, 0x1a, 0x55, 0x18, 0x09, 0x36);

MenuItem::~MenuItem() {
}

/*const cse::String MenuItem::GetCaption() const {
    _assert(impl_ != NULL);
    return impl_->GetCaption();
}

void MenuItem::SetCaption(const cse::String &caption) {
    _assert(impl_ != NULL);
    return impl_->SetCaption(caption);
}

Menu *MenuItem::GetSubMenu() const {
    _assert(impl_ != NULL);
    return impl_->GetSubMenu();
}

void MenuItem::SetSubMenu(Menu *menu) {
    _assert(impl_ != NULL);
    return impl_->SetSubMenu(menu);
}*/

}
