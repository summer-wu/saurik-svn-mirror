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

#include "gui1/editbox.hpp"

namespace gui1 {

ext::Uuid EditBox::TypeId(0x573aaa57,0xde15,0x42d3,0x83,0x74,0xe5,0xd8,0x43,0x1a,0x44,0xfd);

EditBox::EditBox() :
    ImplHolder<Widget, IEditBoxImpl>(TypeId)
{
}

const cse::String EditBox::GetText() const {
    _assert(impl_ != NULL);
    return impl_->GetText();
}

void EditBox::SetText(const cse::String &text) {
    _assert(impl_ != NULL);
    impl_->SetText(text);
}

bool EditBox::GetPassword() const {
    return impl_->GetPassword();
}

void EditBox::SetPassword(bool password) {
    impl_->SetPassword(password);
}

}
