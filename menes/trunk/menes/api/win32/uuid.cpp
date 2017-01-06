/* Menes - C++ High-Level Utility Library
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/win32/uuid.hpp"

#include "ext/byteorder.hpp"
#include "ext/uuid.hpp"

#include <rpc.h>
#include <rpcdce.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

ext::Uuid Uuid::Create() {
    return CreateSequential();
}

ext::Uuid Uuid::CreateRandom() {
    Type id;
    ::UuidCreate(&id);
    return Convert(id);
}

ext::Uuid Uuid::CreateSequential() {
    Type id;
    ::UuidCreateSequential(&id);
    return Convert(id);
}

void Uuid::Convert(ext::Uuid &to, const Type &from) {
    *reinterpret_cast<uint32_t *>(&to.data_[0]) = ext::SwapBytes(from.Data1);
    *reinterpret_cast<uint16_t *>(&to.data_[4]) = ext::SwapBytes(from.Data2);
    *reinterpret_cast<uint16_t *>(&to.data_[6]) = ext::SwapBytes(from.Data3);
    ::memcpy(&to.data_[8], &from.Data4, 8);
}

void Uuid::Convert(Type &to, const ext::Uuid &from) {
    to.Data1 = ext::SwapBytes(*reinterpret_cast<const uint32_t *>(&from.data_[0]));
    to.Data2 = ext::SwapBytes(*reinterpret_cast<const uint16_t *>(&from.data_[4]));
    to.Data3 = ext::SwapBytes(*reinterpret_cast<const uint16_t *>(&from.data_[6]));
    ::memcpy(&to.Data4, &from.data_[8], 8);
}

ext::Uuid Uuid::Convert(const Type &from) {
    ext::Uuid to;
    Convert(to, from);
    return to;
}

Uuid::Type Uuid::Convert(const ext::Uuid &from) {
    Type to;
    Convert(to, from);
    return to;
}

bool Uuid::Equal(const Type &lhs, const Type &rhs) {
    return FALSE != ::IsEqualGUID(lhs, rhs);
}

} }
