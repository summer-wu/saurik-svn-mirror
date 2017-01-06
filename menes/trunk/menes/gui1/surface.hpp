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

#ifndef MENES_GUI1_SURFACE_HPP
#define MENES_GUI1_SURFACE_HPP

#include "alg/tensor.hpp"

#include "ext/uuid.hpp"

#include "gui1/color.hpp"
#include "gui1/metrics.hpp"

namespace gui1 {

class GUI1_DECLARE Canvas {
};

class GUI1_DECLARE Surface {
  public:
    virtual Canvas *GetCanvas(const ext::Uuid &type);
    virtual float GetClientExtent(axis_t axis) = 0;
};

class GUI1_DECLARE Canvas2dSubset :
    public Canvas
{
  public:
    static ext::Uuid TypeId;

  public:
    
};

class GUI1_DECLARE Canvas2d :
    public Canvas
{
  public:
    static ext::Uuid TypeId;

  public:
    virtual void BeginDraw() = 0;
    virtual void EndDraw() = 0;

    virtual void SetColor(const Color &color) = 0;

    virtual void MoveTo(const alg::Vector2f &pos) = 0;
    virtual void LineTo(const alg::Vector2f &pos) = 0;

    virtual void SetPoint(const alg::Vector2f &pos, const Color &color) = 0;

    /*virtual void Measure(const cse::String &text, ext::Vector2f &pos) = 0;
    virtual void Write(const cse::String &text, const ext::Vector2f &pos) = 0;*/
};

/*class GUI1_DECLARE Surface3d {
};

class GUI1_DECLARE Surface2dFromSurface2d :
    public Surface2d
{
  private:
    Surface2d &surface_;
    matrix33f transform_;

  public:
    virtual void SetColor(const color_t &color);

    virtual void MoveTo(const ext::Vector2f &pos);
    virtual void LineTo(const ext::Vector2f &pos);

    virtual void SetPoint(const ext::Vector2f &pos, const color_t &color);
};*/

}

#endif//MENES_GUI1_SURFACE_HPP
