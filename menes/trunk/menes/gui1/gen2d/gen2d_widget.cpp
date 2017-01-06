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

#include "gui1/container.hpp"
#include "gui1/desktop.hpp"

#include "gui1/gen2d/gen2d_toolkit.hpp"
#include "gui1/gen2d/gen2d_widget.hpp"

namespace gui1 {
namespace Gen2d {

WidgetImpl::WidgetImpl(Object &front) :
    front_(front),
    surface_(NULL)
{
}

WidgetImpl::~WidgetImpl() {
    Destroy();
}

float WidgetImpl::GetClientExtent(axis_t axis) {
    alg::Rectangle2f position;
    GetPosition(position);

    switch (axis) {
        case X:  return position.Width();
        case Y:  return position.Height();
        default: return 0; // XXX: _assume(false) ?
    }
}

Toolkit &WidgetImpl::GetToolkit() const {
    return Toolkit_;
}

void WidgetImpl::Create() {
    if (canvas_ != NULL)
        return;

    Container *parent = static_cast<Widget &>(front_).GetParent();
    _assert(parent != NULL);

    Canvas2d *canvas = static_cast<Canvas2d *>(parent->GetClientSurface().GetCanvas(Canvas2d::TypeId));
    _assert(canvas != NULL);

    float pixelsX(parent->GetDesktop()->GetExtent(X, Pixels));
    float pixelsY(parent->GetDesktop()->GetExtent(Y, Pixels));

    /* // XXX: important! these need to be rounded
    float left(static_cast<Widget &>(front_).Left() * pixelsX);
    float top(static_cast<Widget &>(front_).Top() * pixelsY);
    float width(static_cast<Widget &>(front_).Width() * pixelsX);
    float height(static_cast<Widget &>(front_).Height() * pixelsY);*/
}

void WidgetImpl::Destroy() {
}

bool WidgetImpl::GetVisible() const {
    return visible_;
}

void WidgetImpl::SetVisible(bool visible) {
    visible_ = visible;
}

void WidgetImpl::GetPosition(alg::Rectangle2f &position) const {
    _assert(false);
}

void WidgetImpl::SetPosition(const alg::Rectangle2f &position) {
    _assert(false);
}

} }
