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

#include "gui1/treebox.hpp"
#include "gui1/surface.hpp"

namespace gui1 {

ext::Uuid TreeBox::TypeId(0x6836692f,0x3b87,0x4aa5,0xac,0xd1,0xdd,0x00,0x22,0x0b,0x49,0x9c);

TreeBox::TreeBox() :
    ImplHolder<Widget, ITreeBoxImpl>(TypeId)
{
}

TreeBox::~TreeBox() {
}

void TreeBox::BindModel(TreeModel *model) {
    _assert(impl_ != NULL);
    impl_->BindModel(model);
}

TreeModel *TreeBox::GetSelected() const {
    _assert(impl_ != NULL);
    return impl_->GetSelected();
}

void TreeBox::SetSelected(TreeModel *model) {
    _assert(impl_ != NULL);
    impl_->SetSelected(model);
}

}
