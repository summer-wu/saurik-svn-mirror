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

#include "api/uuid.hpp"

#include "api/directx9/error.hpp"
#include "api/directx9/inputdevice.hpp"
#include "api/directx9/inputobject.hpp"

#include "cse/utf16.hpp"
#include "gui1/win32/win32_window.hpp"

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "api/win32/undefine.hpp"

namespace api {
namespace DirectX9 {

namespace be {
struct EnumObject_ {
    unsigned maxoffset_;
    InputDevice &device_;

    typedef ext::Vector<DIOBJECTDATAFORMAT> Formats_;
    Formats_ formats_;

    InputDevice::ObjectMap_ &objects_;

    EnumObject_(InputDevice &device, InputDevice::ObjectMap_ &objects) :
        maxoffset_(0),
        device_(device),
        objects_(objects)
    {
    }

    static BOOL CALLBACK Execute(LPCDIDEVICEOBJECTINSTANCEW instance, LPVOID arg) {
        EnumObject_ *this_(reinterpret_cast<EnumObject_ *>(arg));

        if ((instance->dwType & DIDFT_NODATA) == 0) {
            InputObject object(this_->device_, instance);
            this_->objects_.Insert(instance->dwOfs, object);

            DIOBJECTDATAFORMAT format;
            format.pguid = NULL;
            format.dwOfs = instance->dwOfs;
            format.dwType = instance->dwType;
            format.dwFlags = 0;

            if (this_->maxoffset_ < instance->dwOfs)
                this_->maxoffset_ = instance->dwOfs;

            this_->formats_.InsertLast(format);
        }

        return DIENUM_CONTINUE;
    }
}; }

void InputDevice::Poll_(be::InputEventList_ &events) {
    HRESULT code(device_->Acquire());
    if (FAILED(code)) {
        if (code == DIERR_OTHERAPPHASPRIO)
            return;
        throw Error(code);
    }

    CheckError(device_->Poll());

    DWORD size(api::Win32::Infinite);
    CheckError(device_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &size, DIGDD_PEEK));

    typedef ext::Vector<DIDEVICEOBJECTDATA> Data_;
    Data_ data(size);

    CheckError(device_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), data.Begin(), &size, 0));
    data.SetSize(size);

    _foreach (Data_, item, data) {
        //api::Cout << "Data: " << item->dwData << ", Offset: " << item->dwOfs << ", Sequence: " << item->dwSequence << ", TimeStamp: " << item->dwTimeStamp << ios::NewLine;

        ObjectMap_::ConstIterator object(objects_.Find(item->dwOfs));
        if (object == objects_.End())
            throw;

        be::InputEvent_ value(object->Second());
        value.delta = (object->Second().IsButton() && item->dwData == 0) ? -128 : item->dwData;
        value.sequence = item->dwSequence;
        value.timestamp = item->dwTimeStamp;

        events.InsertLast(value);
    }
}

InputDevice::InputDevice(IDirectInput8W *input, const DIDEVICEINSTANCEW *instance, gui1::Window &window) :
    instance_(api::Uuid::Convert(instance->guidInstance), cse::Utf16String(instance->tszInstanceName)),
    product_(api::Uuid::Convert(instance->guidProduct), cse::Utf16String(instance->tszProductName))
{
    CheckError(input->CreateDevice(api::Uuid::Convert(instance_.id_), &device_.Mend(), NULL));

    gui1::Win32::WindowImpl *impl(dynamic_cast<gui1::Win32::WindowImpl *>(window.GetImpl()));
    _assert(impl != NULL);

    CheckError(device_->SetCooperativeLevel(impl->GetHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY));

    be::EnumObject_ baton(*this, objects_);
    CheckError(device_->EnumObjects(&be::EnumObject_::Execute, reinterpret_cast<LPVOID>(&baton), DIDFT_ALL));

    DIDATAFORMAT format;
    format.dwSize = sizeof(DIDATAFORMAT);
    format.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
    format.dwFlags = 0;
    format.dwDataSize = baton.maxoffset_ + 4 - (baton.maxoffset_ % 4);
    format.dwNumObjs = static_cast<DWORD>(baton.formats_.GetSize());
    format.rgodf = baton.formats_.Begin();

    CheckError(device_->SetDataFormat(&format));

    SetDWordProperty(DIPROP_AXISMODE, DIPROPAXISMODE_REL);
    SetDWordProperty(DIPROP_BUFFERSIZE, format.dwDataSize * 128);
}

InputDevice::~InputDevice() {
    CheckError(device_->Unacquire());
}

const ext::Uuid &InputDevice::GetId() const {
    return id_;
}

cse::String InputDevice::GetInstanceName() const {
    return instance_.name_;
}

cse::String InputDevice::GetPortName() const {
    return GetStringProperty(DIPROP_GETPORTDISPLAYNAME);
}

cse::String InputDevice::GetProductName() const {
    return product_.name_;
}

cse::String InputDevice::GetTypeName() const {
    return GetStringProperty(DIPROP_TYPENAME);
}

void InputDevice::GetProperty(REFGUID id, DIPROPHEADER &header) const {
    CheckError(device_->GetProperty(id, &header));
}

DWORD InputDevice::GetDWordProperty(REFGUID id) const {
    DIPROPDWORD prop;
    prop.diph.dwSize = sizeof(DIPROPDWORD);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 0;
    prop.diph.dwHow = DIPH_DEVICE;

    GetProperty(id, prop.diph);
    return prop.dwData;
}

cse::String InputDevice::GetStringProperty(REFGUID id) const {
    DIPROPSTRING prop;
    prop.diph.dwSize = sizeof(DIPROPSTRING);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 0;
    prop.diph.dwHow = DIPH_DEVICE;

    GetProperty(id, prop.diph);
    return cse::Utf16String(prop.wsz);
}

void InputDevice::SetDWordProperty(REFGUID id, DWORD value) {
    DIPROPDWORD prop;
    prop.diph.dwSize = sizeof(DIPROPDWORD);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 0;
    prop.diph.dwHow = DIPH_DEVICE;
    prop.dwData = value;

    CheckError(device_->SetProperty(id, &prop.diph));
}

} }
