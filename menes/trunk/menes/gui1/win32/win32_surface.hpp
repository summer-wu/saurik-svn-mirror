/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_GUI1_WIN32_CANVAS
#define MENES_GUI1_WIN32_CANVAS

#include "gui1/surface.hpp"

#include <wtypes.h>
#include "api/win32/undefine.hpp"

namespace gui1 {
namespace Win32 {

class CanvasWin32 :
    public Canvas2d
{
  public:
    static ext::Uuid TypeId;

  protected:
    HWND handle_;

  private:
    HDC context_;
    unsigned usage_;

  protected:
    virtual void SetHandle_(HWND handle);

  public:
    CanvasWin32();
    CanvasWin32(HWND handle);

    ~CanvasWin32();

    virtual void BeginDraw();
    virtual void EndDraw();

    virtual void SetColor(const Color &color);

    virtual void MoveTo(const alg::Vector2f &pos);
    virtual void LineTo(const alg::Vector2f &pos);

    virtual void SetPoint(const alg::Vector2f &pos, const Color &color);

    HWND GetHandle();
};

} }

#endif//MENES_GUI1_WIN32_CANVAS
