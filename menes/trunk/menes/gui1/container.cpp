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

#include "gui1/container.hpp"

namespace gui1 {

Container::Container(const ext::Uuid &type) :
    ImplHolder<Widget, IContainerImpl>(type)
{
}

Container::~Container() {
    for (iterator child(children_.Begin()); child != children_.End(); ++child)
        (*child)->Destroy();
}

void Container::Destroy() {
    for (iterator child(children_.Begin()); child != children_.End(); ++child)
        (*child)->Destroy();
    Widget::Destroy();
}

Container::const_iterator Container::begin() const {
    return children_.Begin();
}

Container::iterator Container::begin() {
    return children_.Begin();
}

Container::const_iterator Container::end() const {
    return children_.End();
}

Container::iterator Container::end() {
    return children_.End();
}

const Surface &Container::GetClientSurface() const {
    return impl_->GetClientSurface();
}

Surface &Container::GetClientSurface() {
    return impl_->GetClientSurface();
}

float Container::GetClientExtent(axis_t axis) {
    return impl_->GetClientExtent(axis);
}

}
