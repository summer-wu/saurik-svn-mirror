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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/directx9/error.hpp"
#include "api/directx9/inputobject.hpp"

#include "cse/utf16.hpp"

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "api/win32/undefine.hpp"

namespace api {
namespace DirectX9 {

// XXX: ext::Map
InputObject::InputObject() :
    device_(*_null<InputDevice>())
{
}

InputObject::InputObject(InputDevice &device, const DIDEVICEOBJECTINSTANCEW *instance) :
    device_(device),
    type_(instance->dwType),
    name_(cse::Utf16String(instance->tszName)),
    range_(IsAxis() ? GetRangeProperty(DIPROP_RANGE) : InputRange(0, 128))
{
    if (!IsAxis() && !IsButton()) {
        InputRange logical(GetRangeProperty(DIPROP_LOGICALRANGE));
        InputRange physical(GetRangeProperty(DIPROP_PHYSICALRANGE));
        DWORD granularity(GetDWordProperty(DIPROP_GRANULARITY));
        int a(0);
    }
}

DWORD InputObject::GetDeadZone() const {
    return GetDWordProperty(DIPROP_DEADZONE);
}

const InputDevice &InputObject::GetDevice() const {
    return device_;
}

const cse::String &InputObject::GetName() const {
    return name_;
}

cse::String InputObject::GetKeyName() const {
    return GetStringProperty(DIPROP_KEYNAME);
}

InputRange InputObject::GetRange() const {
    return range_;
}

DWORD InputObject::GetSaturation() const {
    return GetDWordProperty(DIPROP_SATURATION);
}

w32::DWord InputObject::GetType() const {
    return type_;
}

bool InputObject::IsAxis() const {
    return (type_ & DIDFT_AXIS) != 0;
}

bool InputObject::IsButton() const {
    return (type_ & DIDFT_BUTTON) != 0;
}

DWORD InputObject::GetDWordProperty(REFGUID id) const {
    DIPROPDWORD prop;
    prop.diph.dwSize = sizeof(DIPROPDWORD);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = type_;
    prop.diph.dwHow = DIPH_BYID;

    device_.GetProperty(id, prop.diph);
    return prop.dwData;
}

InputRange InputObject::GetRangeProperty(REFGUID id) const {
    DIPROPRANGE prop;
    prop.diph.dwSize = sizeof(DIPROPRANGE);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = type_;
    prop.diph.dwHow = DIPH_BYID;

    device_.GetProperty(id, prop.diph);
    return InputRange(prop.lMin, prop.lMax);
}

cse::String InputObject::GetStringProperty(REFGUID id) const {
    DIPROPSTRING prop;
    prop.diph.dwSize = sizeof(DIPROPSTRING);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = type_;
    prop.diph.dwHow = DIPH_BYID;

    device_.GetProperty(id, prop.diph);
    return cse::Utf16String(prop.wsz);
}

} }
