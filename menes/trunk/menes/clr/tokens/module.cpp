/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "clr/implementation.hpp"
#include "clr/metadata.hpp"
#include "clr/utils.hpp"

#include "clr/tokens/assembly.hpp"
#include "clr/tokens/customattribute.hpp"
#include "clr/tokens/field.hpp"
#include "clr/tokens/method.hpp"
#include "clr/tokens/module.hpp"
#include "clr/tokens/typedef.hpp"

#include "opt/profile.hpp"

namespace clr {

TypeDef *Module::FindTypeDef_(const cse::String &space, const cse::String &name) const {
    OPT_TIMER("clr::Module::FindTypeDef_()")
    return typedefs_.GetOr(NamePair(space, name), NULL);
}

HasTypeDef *Module::ResolveHasTypeDef_() const {
    return const_cast<Module *>(this);
}

void Module::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[5];
    metadata.GetRow(0x00000001, row);

    generation_ = row[0];
    name_ = metadata.GetString(row[1]);
    mvid_ = metadata.GetUuid(row[2]);
    encid_ = metadata.GetUuid(row[3]);
    encbaseid_ = metadata.GetUuid(row[4]);

    {
        OPT_TIMER("clr::Module::Parse()[TypeDef]")
        for (uint32_t token(MakeToken(Tables::TypeDef, metadata.GetRows(Tables::TypeDef))); token > NilToken(Tables::TypeDef); --token) {
            TypeDef *type(metadata.GetToken<TypeDef>(token));

            OPT_TIMER("clr::Module::Parse()[TypeDef:Map]")
            if (!type->IsNested())
                typedefs_.Insert(NamePair(type->GetNamespace(), type->GetName()), type);
        }
    }

    #define ForceTable(table) { \
        OPT_TIMER("clr::Module::Parse()[" #table "]") \
        _forall (uint32_t, token, NilToken(Tables::table), MakeToken(Tables::table, metadata.GetRows(Tables::table))) \
            metadata.GetToken<Token>(token); \
    }

    ForceTable(ClassLayout)
    ForceTable(FieldLayout)
    ForceTable(FieldRva)

    {
        OPT_TIMER("clr::Module::Parse()[InterfaceImpl]")
        for (uint32_t token(MakeToken(Tables::InterfaceImpl, metadata.GetRows(Tables::InterfaceImpl))); token > NilToken(Tables::InterfaceImpl); --token) {
            uint32_t row[2];
            metadata.GetRow(token, row);
            metadata.GetToken<TypeDef>(row[0])->Implement(metadata.GetToken<TypeDefOrRef>(row[1]));
        }
    }

    ForceTable(NestedClass)
    ForceTable(PropertyMap)
    ForceTable(CustomAttribute)

    {
        OPT_TIMER("clr::Module::Parse()[ParseCode]")
        for (uint32_t token(MakeToken(Tables::Method, metadata.GetRows(Tables::Method))); token > NilToken(Tables::Method); --token)
            if (Implementation *implementation = metadata.GetToken<Method>(token)->GetImplementation())
                implementation->Parse(metadata);
    }

    #undef ForceTable
}

uint16_t Module::GetGeneration() const {
    return generation_;
}

void Module::SetGeneration(uint16_t generation) {
    generation_ = generation;
}

const cse::String &Module::GetName() const {
    return name_;
}

void Module::SetName(const cse::String &name) {
    name_ = name;
}

ext::Uuid Module::GetMvid() const {
    return mvid_;
}

void Module::SetMvid(const ext::Uuid &mvid) {
    mvid_ = mvid;
}

ext::Uuid Module::GetEncId() const {
    return encid_;
}

void Module::SetEncId(const ext::Uuid &encid) {
    encid_ = encid;
}

ext::Uuid Module::GetEncBaseId() const {
    return encbaseid_;
}

void Module::SetEncBaseId(const ext::Uuid &encbaseid) {
    encbaseid_ = encbaseid;
}

const Assembly *Module::GetAssembly() const {
    return assembly_;
}

Assembly *Module::GetAssembly() {
    return assembly_;
}

const Method *Module::GetEntryPoint() const {
    return entry_;
}

Method *Module::GetEntryPoint() {
    return entry_;
}

void Module::SetEntryPoint(Method *method) {
    entry_ = method;
}

void Module::PrepareModule() {
    OPT_TIMER("clr::Module::PrepareModule()")
    _foreach (const TypeMap, type, GetTypes())
        type->Second()->PrepareType();
}

const Module::TypeMap &Module::GetTypes() const {
    return typedefs_;
}

}
