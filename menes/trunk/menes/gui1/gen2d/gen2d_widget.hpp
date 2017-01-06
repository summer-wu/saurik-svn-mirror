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

#ifndef MENES_GUI1_GEN2D_WIDGET
#define MENES_GUI1_GEN2D_WIDGET

#include "gui1/widget.hpp"
#include "gui1/surface.hpp"

namespace gui1 {
namespace Gen2d {

class WidgetImpl :
    virtual public IWidgetImpl
{
  protected:
    Object &front_;
    Surface *surface_;
    Canvas2d *canvas_;

    bool visible_;

  protected:
    WidgetImpl(Object &front);
    virtual ~WidgetImpl();

    virtual float GetClientExtent(axis_t axis);

    virtual void Paint() = 0;

  public:
    virtual Toolkit &GetToolkit() const;

    virtual void Create();
    virtual void Destroy();

    virtual bool GetVisible() const;
    virtual void SetVisible(bool visible);

    virtual void GetPosition(alg::Rectangle2f &position) const;
    virtual void SetPosition(const alg::Rectangle2f &position);
};

} }

#endif//MENES_GUI1_GEN2D_WIDGET
