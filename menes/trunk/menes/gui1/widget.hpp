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

#ifndef MENES_GUI1_WIDGET_HPP
#define MENES_GUI1_WIDGET_HPP

#include "alg/rectangle.hpp"

#include "gui1/keyboard.hpp"
#include "gui1/metrics.hpp"
#include "gui1/mouse.hpp"
#include "gui1/object.hpp"

namespace gui1 {

class Container;
class Desktop;
class Surface;

class GUI1_DECLARE IWidgetImpl :
    virtual public IObjectImpl
{
  public:
    virtual bool GetVisible() const = 0;
    virtual void SetVisible(bool visible) = 0;

    virtual void GetPosition(alg::Rectangle2f &position) const = 0;
    virtual void SetPosition(const alg::Rectangle2f &position) = 0;
};

class GUI1_DECLARE Widget :
    public ImplHolder<Object, IWidgetImpl>
{
  private:
    Container *parent_;

  protected:
    explicit Widget(const ext::Uuid &type);

    virtual ~Widget();

  public:
    hop::Delegate<void ()> OnWidgetCreate;
    hop::Delegate<void ()> OnWidgetDestroy;

    hop::Delegate<void (gui1::Key)> OnWidgetKeyDown;
    hop::Delegate<void (gui1::Key)> OnWidgetKeyUp;

    hop::Delegate<void (gui1::MouseButton, float, float)> OnWidgetMouseDoubleClick;
    hop::Delegate<void (gui1::MouseButton, float, float)> OnWidgetMouseDown;
    hop::Delegate<void (gui1::MouseButton, float, float)> OnWidgetMouseMove;
    hop::Delegate<void (gui1::MouseButton, float, float)> OnWidgetMouseUp;

    hop::Delegate<void (gui1::MouseButton, int, float, float)> OnWidgetMouseWheel;

    virtual void This_WidgetCreate();
    virtual void This_WidgetDestroy();

    virtual void This_WidgetKeyDown(gui1::Key key);
    virtual void This_WidgetKeyUp(gui1::Key key);

    virtual void This_WidgetMouseDoubleClick(gui1::MouseButton button, float x, float y);
    virtual void This_WidgetMouseDown(gui1::MouseButton button, float x, float y);
    virtual void This_WidgetMouseMove(gui1::MouseButton buttons, float x, float y);
    virtual void This_WidgetMouseUp(gui1::MouseButton button, float x, float y);

    virtual void This_WidgetMouseWheel(gui1::MouseButton buttons, int delta, float x, float y);

    virtual Toolkit *GetPreferredToolkit() const;

    const Container *GetParent() const;
    Container *GetParent();

    void SetParent(Container *parent);

    virtual const Desktop *GetDesktop() const;
    virtual Desktop *GetDesktop();

    bool GetVisible() const;
    void SetVisible(bool visible);

    alg::Rectangle2f GetPosition() const;
    void GetPosition(alg::Rectangle2f &position) const;
    void SetPosition(const alg::Rectangle2f &position);
};

}

#endif//MENES_GUI1_WIDGET_HPP
