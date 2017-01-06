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

#include "api/threads.hpp"
#include "api/uuid.hpp"

#include "api/directx9/error.hpp"
#include "api/directx9/input.hpp"
#include "api/directx9/inputdevice.hpp"
#include "api/directx9/inputobject.hpp"

#include "app/win32/module.hpp"

#include <algorithm>

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "api/win32/undefine.hpp"

namespace api {
namespace DirectX9 {

namespace {
    struct EnumDeviceBaton_ {
        typedef ext::Vector<ext::Uuid> DeviceIds_;
        DeviceIds_ deviceids_;

        IDirectInput8W *input_;
        be::DeviceMap_ &devices_;
        gui1::Window &window_;

        EnumDeviceBaton_(IDirectInput8W *input, be::DeviceMap_ &devices, gui1::Window &window) :
            input_(input),
            devices_(devices),
            window_(window)
        {
        }
    };

    BOOL CALLBACK EnumDevice_(const DIDEVICEINSTANCEW *instance, void *arg) {
        EnumDeviceBaton_ *this_(reinterpret_cast<EnumDeviceBaton_ *>(arg));

        ext::Uuid id;
        api::Uuid::Convert(id, instance->guidInstance);

        this_->deviceids_.InsertLast(id);

        _R<InputDevice> &device(this_->devices_[id]);
        if (device == NULL)
            device = new InputDevice(this_->input_, instance, this_->window_);

        return DIENUM_CONTINUE;
    }
}

namespace be {
    struct InputEventLess_ {
    bool operator ()(const InputEvent_ &lhs, const InputEvent_ &rhs) const {
        return int32_t(lhs.sequence - rhs.sequence) < 0;
    } };
}

Input::Input(gui1::Window &window) :
    window_(window)
{
    CheckError(::DirectInput8Create(app::Win32::GetModule(), DIRECTINPUT_VERSION, IID_IDirectInput8W, reinterpret_cast<void **>(&input_.Mend()), NULL));
}

Input::~Input() {
}

void Input::UpdateDevices() {
    EnumDeviceBaton_ baton(input_, devices_, window_);
    CheckError(input_->EnumDevices(DI8DEVCLASS_ALL, &EnumDevice_, reinterpret_cast<LPVOID>(&baton), DIEDFL_ALLDEVICES | DIEDFL_INCLUDEHIDDEN | DIEDFL_INCLUDEPHANTOMS));
}

void Input::PollDevices() {
    be::InputEventList_ events;
    for (be::DeviceMap_::Iterator device(devices_.Begin()); device != devices_.End(); ++device)
        device->Second()->Poll_(events);

    std::sort(events.Begin(), events.End(), be::InputEventLess_());
    for (be::InputEventList_::Iterator event(events.Begin()); event != events.End(); ++event) {
        api::DirectX9::InputRange range(event->object->GetRange());
        uint32_t width(range.second - range.first);
        double delta(event->delta / double(width));

        //api::Cout << "[" << event->object->GetDevice().GetInstanceName() << ":" << event->object->GetName() << "] = " << event->delta << " " << delta << ios::NewLine;
    }
}

} }
