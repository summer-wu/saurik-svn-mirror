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

#ifndef MENES_API_DIRECTX9_INPUTMANAGER_HPP
#define MENES_API_DIRECTX9_INPUTMANAGER_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/com/handle.hpp"
#include "api/directx9/linkage.hpp"


#include "hop/function.hpp"

#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/uuid.hpp"

#include "gui1/window.hpp"

struct IDirectInput8W;

namespace api {
namespace DirectX9 {

class InputDevice;
class InputObject;

struct InputObjectId {
    ext::Uuid device;
    DWORD object;
};

typedef hop::Function<void (ext::Uuid, const api::DirectX9::InputObject &)> InputEventHandler_;

namespace be {
    typedef ext::RedBlackMap< ext::Uuid, _R<InputDevice> > DeviceMap_;
}

class Input {
  private:
    gui1::Window &window_;
    api::Com::Handle<IDirectInput8W>::Result input_;

    be::DeviceMap_ devices_;

  public:
    API_DIRECTX9_DECLARE Input(gui1::Window &window);
    API_DIRECTX9_DECLARE ~Input();

    API_DIRECTX9_DECLARE void UpdateDevices();
    API_DIRECTX9_DECLARE void PollDevices();

    API_DIRECTX9_DECLARE void RegisterEvent(const InputObjectId &object, const hop::Function<void (uint32_t)> &callback);
};

} }

#endif//MENES_API_DIRECTX9_INPUTMANAGER_HPP
