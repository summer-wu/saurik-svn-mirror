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

#include "gui1/widget.hpp"

#include "gui1/container.hpp"
#include "gui1/toolkit.hpp"

#include "hop/bind.hpp"

namespace gui1 {

Widget::Widget(const ext::Uuid &type) :
    ImplHolder<Object, IWidgetImpl>(type),
    parent_(NULL)
{
    OnWidgetCreate += hop::Bind<0>(&Widget::This_WidgetCreate, this);
    OnWidgetDestroy += hop::Bind<0>(&Widget::This_WidgetDestroy, this);

    OnWidgetKeyDown += hop::Bind<0>(&Widget::This_WidgetKeyDown, this);
    OnWidgetKeyUp += hop::Bind<0>(&Widget::This_WidgetKeyUp, this);

    OnWidgetMouseDoubleClick += hop::Bind<0>(&Widget::This_WidgetMouseDoubleClick, this);
    OnWidgetMouseDown += hop::Bind<0>(&Widget::This_WidgetMouseDown, this);
    OnWidgetMouseMove += hop::Bind<0>(&Widget::This_WidgetMouseMove, this);
    OnWidgetMouseUp += hop::Bind<0>(&Widget::This_WidgetMouseUp, this);
    OnWidgetMouseWheel += hop::Bind<0>(&Widget::This_WidgetMouseWheel, this);
}

Widget::~Widget() {
    Destroy();
    SetParent(NULL);
}

const Container *Widget::GetParent() const {
    return parent_;
}

void Widget::This_WidgetCreate() {
}

void Widget::This_WidgetDestroy() {
}

void Widget::This_WidgetKeyDown(gui1::Key key) {
}

void Widget::This_WidgetKeyUp(gui1::Key key) {
}

void Widget::This_WidgetMouseDoubleClick(gui1::MouseButton button, float x, float y) {
}

void Widget::This_WidgetMouseDown(gui1::MouseButton button, float x, float y) {
}

void Widget::This_WidgetMouseMove(gui1::MouseButton buttons, float x, float y) {
}

void Widget::This_WidgetMouseUp(gui1::MouseButton button, float x, float y) {
}

void Widget::This_WidgetMouseWheel(gui1::MouseButton buttons, int delta, float x, float y) {
}

Toolkit *Widget::GetPreferredToolkit() const {
    if (Toolkit *toolkit = Object::GetPreferredToolkit())
        return toolkit;
    return parent_ == NULL ? NULL : parent_->GetPreferredToolkit();
}

Container *Widget::GetParent() {
    return parent_;
}

void Widget::SetParent(Container *parent) {
    parent_ = parent;
}

const Desktop *Widget::GetDesktop() const {
    _assert(parent_ != NULL);
    return parent_->GetDesktop();
}

Desktop *Widget::GetDesktop() {
    _assert(parent_ != NULL);
    return parent_->GetDesktop();
}

bool Widget::GetVisible() const {
    _assert(impl_ != NULL);
    return impl_->GetVisible();
}

void Widget::SetVisible(bool visible) {
    _assert(impl_ != NULL);
    return impl_->SetVisible(visible);
}

alg::Rectangle2f Widget::GetPosition() const {
    alg::Rectangle2f position;
    GetPosition(position);
    return position;
}

void Widget::GetPosition(alg::Rectangle2f &position) const {
    _assert(impl_ != NULL);
    return impl_->GetPosition(position);
}

void Widget::SetPosition(const alg::Rectangle2f &position) {
    _assert(impl_ != NULL);
    return impl_->SetPosition(position);
}

}
