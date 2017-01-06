/* Menes Optimization - C++ Profiling and Verification
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

#ifndef MENES_SZF_SAVELOAD_HPP
#define MENES_SZF_SAVELOAD_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/limits.hpp"
#include "etl/template.hpp"
#include "etl/traits.hpp"

#include "szf/linkage.hpp"
#include "szf/method.hpp"

#define SZF_LOAD(field) szf::Load(method, field, #field);
#define SZF_SAVE(field) szf::Save(method, field, #field);

namespace szf {

class ISaveLoad;
class Serializable;

namespace be {

template <typename Type_>
struct SaveLoadIntegral_ {
    static const bool is_signed = etl::Limits<Type_>::IsIntegral && etl::Limits<Type_>::IsSigned;
    static const bool is_unsigned = etl::IsEnum<Type_>::value || etl::Limits<Type_>::IsIntegral && !etl::Limits<Type_>::IsSigned;
};

template <typename Elem_, size_t Size_>
struct SaveLoadIntegral_<Elem_[Size_]> {
    static const bool is_signed = false;
    static const bool is_unsigned = false;
};

}

template <typename Type_>
typename etl::IfTrue<be::SaveLoadIntegral_<Type_>::is_signed, void>::Result
Save(Method *method, Type_ value, const char *name) {
    method->SaveSigned(static_cast<int64_t>(value), name);
}

template <typename Type_>
typename etl::IfTrue<be::SaveLoadIntegral_<Type_>::is_signed, void>::Result
Load(Method *method, Type_ &value, const char *name) {
    int64_t value64;
    method->LoadSigned(value64, name);
    value = static_cast<Type_>(value64);
}

template <typename Type_>
typename etl::IfTrue<be::SaveLoadIntegral_<Type_>::is_unsigned, void>::Result
Save(Method *method, Type_ value, const char *name) {
    method->SaveUnsigned(static_cast<uint64_t>(value), name);
}

template <typename Type_>
typename etl::IfTrue<be::SaveLoadIntegral_<Type_>::is_unsigned, void>::Result
Load(Method *method, Type_ &value, const char *name) {
    uint64_t value64;
    method->LoadUnsigned(value64, name);
    value = static_cast<Type_>(value64);
}

/* // XXX: this wasn't safe anyway... think pointers...
template <typename Type_>
typename etl::IfTrue<etl::IsPod<Type_>::value, void>::Result
Save(Method *method, const Type_ &value, const char *name) {
    method->SaveData(method, &value, sizeof(Type_), name);
}

template <typename Type_>
typename etl::IfTrue<etl::IsPod<Type_>::value, void>::Result
Load(Method *method, Type_ &value, const char *name) {
    method->LoadData(method, &value, sizeof(Type_), name);
}*/

SZF_DECLARE void Save(Method *method, bool value, const char *name);
SZF_DECLARE void Load(Method *method, bool &value, const char *name);

SZF_DECLARE void Save(Method *method, float value, const char *name);
SZF_DECLARE void Load(Method *method, float &value, const char *name);

SZF_DECLARE void Save(Method *method, double value, const char *name);
SZF_DECLARE void Load(Method *method, double &value, const char *name);

SZF_DECLARE void Save(Method *method, const ISaveLoad &value, const char *name);
SZF_DECLARE void Load(Method *method, ISaveLoad &value, const char *name);

SZF_DECLARE void Save(Method *method, Serializable *value, const char *name);
SZF_DECLARE void Load(Method *method, _R<Serializable> &value, const char *name);

template <typename Type_>
void Save(Method *method, const _R<Type_> &value, const char *name) {
    if (value.IsEmpty())
        Save(method, _null<Serializable>(), name);
    else {
        Serializable *object(dynamic_cast<Serializable *>(value.GetValue()));
        _assert(object != NULL);
        Save(method, object, name);
    }
}

template <typename Type_>
void Load(Method *method, _R<Type_> &value, const char *name) {
    _R<Serializable> object;
    Load(method, object, name);
    if (object.IsEmpty())
        value = NULL;
    else {
        value = dynamic_cast<Type_ *>(object.GetValue());
        _assert(value != NULL);
    }
}

template <typename Elem_, size_t Size_>
void Save(Method *method, const Elem_ (&value)[Size_], const char *name) {
    method->OpenScope(name);

    for (int i(0); i != Size_; ++i)
        Save(method, value[i], "element");

    method->CloseScope();
}

template <typename Elem_, size_t Size_>
void Load(Method *method, Elem_ (&value)[Size_], const char *name) {
    method->OpenScope(name);

    for (int i(0); i != Size_; ++i)
        Load(method, value[i], "element");

    method->CloseScope();
}

template <typename Type_>
Type_ Method::Load(const char *name) {
    Type_ value;
    szf::Load(this, value, name);
    return value;
}

}

#endif//MENES_SZF_SAVELOAD_HPP
