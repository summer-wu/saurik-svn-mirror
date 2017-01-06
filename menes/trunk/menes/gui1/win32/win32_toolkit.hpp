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

#ifndef MENES_GUI1_WIN32_TOOLKIT
#define MENES_GUI1_WIN32_TOOLKIT

#include "hop/delegate.hpp"

#include "gui1/toolkit.hpp"
#include "gui1/win32/linkage.hpp"

#include <wtypes.h>
#include "api/win32/undefine.hpp"

namespace gui1 {
namespace Win32 {

class GUI1_WIN32_DECLARE ToolkitImpl :
    public Toolkit
{
  private:
    DWORD thread_;

  public:
    hop::Delegate<void ()> OnToolkitIdle;

  public:
    ToolkitImpl();

    virtual cse::String GetName() const;
    virtual IObjectImpl *Instance(Object &front);
    virtual int Run();
    virtual void Quit(int result = 0);

    void Borrow(const hop::Function<void ()> &call);
    void IdleKick();

    // XXX: this function is damned; please avoid it
    void SetCursor(bool visible);
};

GUI1_WIN32_DECLARE extern ToolkitImpl Toolkit_;

} }

#endif//MENES_GUI1_WIN32_TOOLKIT
