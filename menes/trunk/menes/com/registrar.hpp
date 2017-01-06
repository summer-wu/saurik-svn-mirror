/* Diapexis - Portable COM Reimplementation
 * Copyright (C) 2001-2004  Jay Freeman (saurik)
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

#ifndef MENES_COM_REGISTRAR_HPP
#define MENES_COM_REGISTRAR_HPP

#include "cxx/platform.hpp"
#include "com/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#ifdef _MSC_VER
// C4660: template-class specialization '...' is already instantiated
//#pragma warning(disable: 4660)
#endif

#include "com/factory.hpp"
#include "com/metadata.hpp"
#include "com/object.hpp"
#include "com/types.hpp"

#include "cse/types.hpp"

#include "ext/exception.hpp"
#include "ext/stackref.hpp"
#include "ext/types.hpp"

namespace com {

class ClassNotFoundException :
    public ext::StringException
{
  public:
    ClassNotFoundException(const ext::Uuid &id);
    ClassNotFoundException(const cse::String &name);
};

COM_DECLARE void Terminate();

COM_DECLARE void Load();
COM_DECLARE void Load(const cse::String &path);

COM_DECLARE clsid_t Register(const cse::String &name, clsid_t guid, const _R<ClassFactory> &factory);

// XXX: because _nameof() returns a const char * for irritating reasons
COM_DECLARE clsid_t Register(const char *name, clsid_t guid, const _R<ClassFactory> &factory);

template <typename Type_>
clsid_t Register() {
    static _S< SimpleFactory<Type_> > factory;
    return Register(_nameof(Type_), _idof(Type_), &factory);
}

COM_DECLARE clsid_t Resolve(const cse::String &name);

COM_DECLARE _R<ClassFactory> GetClassObject(clsid_t guid);

COM_DECLARE _R<Object> CreateInstance(const cse::String &name);

template <typename Type_>
_R<Type_> CreateInstance(const cse::String &name) {
    return dynamic_cast<Type_ *>(CreateInstance(name).GetValue());
}

COM_DECLARE _R<Object> CreateInstance(clsid_t guid);

template <typename Type_>
_R<Type_> CreateInstance(clsid_t guid) {
    return dynamic_cast<Type_ *>(CreateInstance(guid).GetValue());
}

template <typename Type_>
struct AutoRegister {
    static AutoRegister<Type_> auto_;

    AutoRegister() {
        com::Register<Type_>();
    }
};

template <typename Type_>
AutoRegister<Type_> AutoRegister<Type_>::auto_;

namespace be {
    template <typename Type_>
    void AutoRegister_(const Type_ *) {
        AutoRegister<Type_>::auto_;
    }
}

}

#define COM_REGISTER(type) \
    namespace com { \
    void AutoRegister_(type *) { \
        com::AutoRegister<type>::auto_; \
    } }

#define COM_DYNAMICINSTANCE() \
    void AutoRegister_() const { com::be::AutoRegister_(this); }

#endif//MENES_COM_REGISTRAR_HPP
