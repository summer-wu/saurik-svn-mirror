/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_API_DIRECTX9_INPUTOBJECT_HPP
#define MENES_API_DIRECTX9_INPUTOBJECT_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/com/handle.hpp"

#include "api/directx9/input.hpp"
#include "api/directx9/inputdevice.hpp"
#include "api/directx9/linkage.hpp"

#include "ext/refcount.hpp"
#include "gui1/window.hpp"
#include "api/win32/types.hpp"

struct IDirectInputDevice8W;
struct DIDEVICEOBJECTINSTANCEW;

namespace api {
namespace DirectX9 {

typedef std::pair<LONG, LONG> InputRange;

class InputObject {
  private:
    InputDevice &device_;
    w32::DWord type_;
    cse::String name_;
    InputRange range_;
    w32::DWord value_;

  public:
    // XXX: ext::Map
    InputObject();

    InputObject(InputDevice &device, const DIDEVICEOBJECTINSTANCEW *instance);

    DWORD GetDeadZone() const;
    const InputDevice &GetDevice() const;
    cse::String GetKeyName() const;
    const cse::String &GetName() const;
    InputRange GetRange() const;
    DWORD GetSaturation() const;
    w32::DWord GetType() const;

    bool IsAxis() const;
    bool IsButton() const;

    DWORD GetDWordProperty(REFGUID id) const;
    InputRange GetRangeProperty(REFGUID id) const;
    cse::String GetStringProperty(REFGUID id) const;
};

} }

#endif//MENES_API_DIRECTX9_INPUTOBJECT_HPP
