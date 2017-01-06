/* Diapexis - Portable COM Reimplementation
 * Copyright (C) 2003  Jay Freeman (saurik)
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

#ifndef MENES_COM_METADATA_HPP
#define MENES_COM_METADATA_HPP

#include "cxx/platform.hpp"
#include "com/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "com/object.hpp"
#include "com/types.hpp"

#include "ext/uuid.hpp"

namespace com {

template <typename Type_>
struct MetadataImpl {
    inline static com::clsid_t GetComId() {
        return Type_::GetComId();
    }

    inline static const char *GetComName() {
        return Type_::GetComName();
    }
};

template <typename Type_>
struct Metadata :
    public MetadataImpl<Type_>
{
};

}

#define COM_METADATA_BEGIN(type) \
namespace com { \
template <> \
struct Metadata<type> : \
    public MetadataImpl<type> \
{

#define COM_CLASSUUID(a1, a2, a3, a4, a5, a6, a7, a8, b1, b2, b3, b4, b5, b6, b7, b8) public: \
    inline static com::clsid_t GetComId() { \
        static const com::clsid_t guid(0x ## a1 ## a2 ## a3 ## a4, 0x ## a5 ## a6, 0x ## a7 ## a8, 0x ## b1, 0x ## b2, 0x ## b3, 0x ## b4, 0x ## b5, 0x ## b6, 0x ## b7, 0x ## b8); \
        return guid; \
    }

#define COM_CLASSNAME(name) public: \
    inline static const char *GetComName() { \
        return name; \
    }

#define COM_METADATA_END() \
}; }

#define _idof(type) com::Metadata<type>::GetComId()
#define _nameof(type) com::Metadata<type>::GetComName()

#define COM_CLASSMETADATA() public: \
    virtual com::clsid_t GetClassId() const { return GetComId(); } \
    virtual const char *ClassName() const { return GetComName(); }

COM_METADATA_BEGIN(com::Object)
COM_CLASSNAME("com::Object")
COM_CLASSUUID(00,00,00,00,00,00,00,00,C0,00,00,00,00,00,00,46)
COM_METADATA_END()

#endif//MENES_COM_METADATA_HPP
