/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_SZF_HELPERS_HPP
#define MENES_SZF_HELPERS_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/files.hpp"
#include "ext/types.hpp"

#include "szf/binary.hpp"
#include "szf/linkage.hpp"
#include "szf/saveload.hpp"
#include "szf/serialize.hpp"

namespace szf {

template <typename Type_>
void SaveToBuffer(const Type_ &value, ios::Writer &buffer, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    OPT_TIMER("szf::SaveToBuffer()")
    _R<Method> method(com::CreateInstance<Method>(methodid));
    dynamic_cast<IBufferMethod *>(method.GetValue())->Bind(NULL, &buffer);
    szf::Save(method, value, "value");
}

template <typename Type_>
void SaveToFile(const _R<Serializable> &object, const cse::String &path, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    OPT_TIMER("szf::SaveToFile()")
    api::FileWriter file;
    file.Open(path);
    return SaveToBuffer(object, file, methodid);
}

template <typename Type_>
void LoadFromBuffer(Type_ &value, ios::Reader &buffer, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    OPT_TIMER("szf::LoadFromBuffer()")
    _R<Method> method(com::CreateInstance<Method>(methodid));
    dynamic_cast<IBufferMethod *>(method.GetValue())->Bind(&buffer, NULL);
    szf::Load(method, value, "value");
}

template <typename Type_>
void LoadFromFile(Type_ &value, const cse::String &path, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    OPT_TIMER("szf::LoadFromFile()")
    _S<api::FileReader> file(path);
    LoadFromBuffer(value, file, methodid);
}

template <typename Type_>
Type_ LoadFromBuffer(ios::Reader &buffer, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    Type_ object;
    LoadFromBuffer(object, buffer, methodid);
    return object;
}

template <typename Type_>
Type_ LoadFromFile(const cse::String &path, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    Type_ object;
    LoadFromFile(object, path, methodid);
    return object;
}

inline _R<Serializable> LoadFromBuffer(ios::Reader &buffer, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    return szf::LoadFromBuffer< _R<Serializable> >(buffer, methodid);
}

inline _R<Serializable> LoadFromFile(const cse::String &path, const ext::Uuid &methodid = _idof(BinaryMethod)) {
    return szf::LoadFromFile< _R<Serializable> >(path, methodid);
}

}

#endif//MENES_SZF_HELPERS_HPP
