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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "gui1/gen2d/gen2d_toolkit.hpp"

#include "gui1/widget.hpp"

#include "gui1/gen2d/gen2d_button.hpp"
/*#include "gui1/gen2d/gen2d_checkbox.hpp"
#include "gui1/gen2d/gen2d_desktop.hpp"
#include "gui1/gen2d/gen2d_editbox.hpp"
#include "gui1/gen2d/gen2d_group.hpp"
#include "gui1/gen2d/gen2d_listbox.hpp"
#include "gui1/gen2d/gen2d_static.hpp"
#include "gui1/gen2d/gen2d_tabs.hpp"
#include "gui1/gen2d/gen2d_treebox.hpp"
#include "gui1/gen2d/gen2d_window.hpp"*/

namespace gui1 {
namespace Gen2d {

ToolkitImpl Toolkit_;

cse::String ToolkitImpl::GetName() const {
    return _B("General2d");
}

IObjectImpl *ToolkitImpl::Instance(Object &front) {
    const ext::Uuid &type = front.GetType();

    if (type == ext::Uuid::Null)
        return NULL;

    #define ImplementType(Type) \
    else if (type == Type::TypeId) \
        return new Type ## Impl(front);

    ImplementType(Button)
    /*ImplementType(CheckBox)
    ImplementType(Desktop)
    ImplementType(EditBox)
    ImplementType(Group)
    ImplementType(ListBox)
    ImplementType(Static)
    ImplementType(Tabs)
    ImplementType(TreeBox)
    ImplementType(Window)*/
    #undef ImplementType

    return NULL;
}

int ToolkitImpl::Run() {
    return -1;
}

void ToolkitImpl::Quit(int result) {
}

} }
