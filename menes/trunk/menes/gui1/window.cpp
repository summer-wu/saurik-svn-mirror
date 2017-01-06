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

#include "hop/bind.hpp"
#include "gui1/window.hpp"

namespace gui1 {

ext::Uuid Window::TypeId(0x73bf2948,0x790e,0x4a41,0xac,0xfb,0x63,0x6b,0xf9,0x8a,0x60,0x4b);

void Window::This_WindowClose() {
}

Window::Window() :
    ImplHolder<Container, IWindowImpl>(TypeId)
{
    OnWindowClose += hop::BindAll(&Window::This_WindowClose, this);
}

const cse::String Window::GetTitle() const {
    _assert(impl_ != NULL);
    return impl_->GetTitle();
}

void Window::SetTitle(const cse::String &title) {
    _assert(impl_ != NULL);
    return impl_->SetTitle(title);
}

void Window::Close() {
    _assert(impl_ != NULL);
    return impl_->Close();
}

Menu *Window::GetMenu() const {
    _assert(impl_ != NULL);
    return impl_->GetMenu();
}

void Window::SetMenu(Menu *menu) {
    _assert(impl_ != NULL);
    return impl_->SetMenu(menu);
}

}
