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

#include "gui1/object.hpp"
#include "gui1/surface.hpp"
#include "gui1/toolkit.hpp"

namespace gui1 {

IObjectImpl::~IObjectImpl() {
}

Object::Object(const Object &rhs) {
    // nuh uh UNNNHH   !!BAD USER!!
    _assert(false);
}

Object::Object(const ext::Uuid &type) :
    impl_(NULL),
    toolkit_(NULL),
    type_(type)
{
}

Object::~Object() {
    Destroy();
}

void Object::SetImpl_(IObjectImpl *impl) {
    impl_ = impl;
}

const ext::Uuid &Object::GetType() const {
    return type_;
}

Toolkit *Object::GetPreferredToolkit() const {
    return toolkit_;
}

void Object::SetPreferredToolkit(Toolkit *toolkit) {
    toolkit_ = toolkit;
}

Toolkit *Object::GetActiveToolkit() const {
    return impl_ == NULL ? NULL : &impl_->GetToolkit();
}

const IObjectImpl *Object::GetImpl() const {
    return impl_;
}

IObjectImpl *Object::GetImpl() {
    return impl_;
}

void Object::Create() {
    _assert(impl_ == NULL);

    Toolkit *toolkit(GetPreferredToolkit());
    _assert(toolkit != NULL);

    SetImpl_(toolkit->Instance(*this));
    _assert(impl_ != NULL);

    impl_->Create();
}

void Object::Destroy() {
    if (impl_ == NULL)
        return;

    impl_->Destroy();
    delete impl_;
    SetImpl_(NULL);
}

}
