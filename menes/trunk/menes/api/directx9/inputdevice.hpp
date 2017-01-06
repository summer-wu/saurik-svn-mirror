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

#ifndef MENES_API_DIRECTX9_INPUTDEVICE_HPP
#define MENES_API_DIRECTX9_INPUTDEVICE_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/com/handle.hpp"
#include "api/directx9/linkage.hpp"

#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/refcount.hpp"

#include "gui1/window.hpp"

struct DIDEVICEINSTANCEW;
struct DIPROPHEADER;

struct IDirectInput8W;
struct IDirectInputDevice8W;

namespace api {
namespace DirectX9 {

class InputObject;

namespace be {
    struct InputEvent_ {
        const InputObject *object;

        int32_t delta;
        DWORD sequence;
        DWORD timestamp;

        InputEvent_() {
        }

        InputEvent_(const InputObject &object) :
            object(&object)
        {
        }
    };

    typedef ext::Vector<InputEvent_> InputEventList_;

    struct EnumObject_;
}

class InputDevice :
    public ext::ReferenceCounted
{
    friend struct be::EnumObject_;
    friend class Input;

  private:
    ext::Uuid id_;
    api::Com::Handle<IDirectInputDevice8W>::Result device_;

    typedef ext::RedBlackMap<DWORD, InputObject> ObjectMap_;
    ObjectMap_ objects_;

    struct NamedGuid {
        ext::Uuid id_;
        cse::String name_;

        NamedGuid(const ext::Uuid &id, const cse::String &name) :
            id_(id),
            name_(name)
        {
        }
    };

    NamedGuid instance_;
    NamedGuid product_;

    void Poll_(be::InputEventList_ &events);

  public:
    InputDevice(IDirectInput8W *input, const DIDEVICEINSTANCEW *instance, gui1::Window &window);
    ~InputDevice();

    const ext::Uuid &GetId() const;
    cse::String GetInstanceName() const;
    cse::String GetPortName() const;
    cse::String GetProductName() const;
    cse::String GetTypeName() const;

    void GetProperty(REFGUID id, DIPROPHEADER &header) const;

    DWORD GetDWordProperty(REFGUID id) const;
    cse::String GetStringProperty(REFGUID id) const;
    void SetDWordProperty(REFGUID id, DWORD value);
};

} }

#endif//MENES_API_DIRECTX9_INPUTDEVICE_HPP
