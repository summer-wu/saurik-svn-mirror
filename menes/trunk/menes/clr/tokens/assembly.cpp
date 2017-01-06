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

#include "api/console.hpp"

#include "clr/tokens/assembly.hpp"
#include "clr/tokens/file.hpp"
#include "clr/tokens/module.hpp"

#include "clr/metadata.hpp"
#include "clr/tables.hpp"
#include "clr/utils.hpp"

namespace clr {

TypeDef *Assembly::FindTypeDef_(const cse::String &space, const cse::String &name) const {
    for (ModuleMap::Iterator module(modules_.Begin()); module != modules_.End(); ++module) {
        TypeDef *type(module->Second()->FindTypeDef(space, name));
        if (type != NULL)
            return type;
    }

    return NULL;
}

Method *Assembly::GetEntryPoint_() const {
    for (ModuleMap::Iterator module(modules_.Begin()); module != modules_.End(); ++module) {
        Method *entry(module->Second()->GetEntryPoint());
        if (entry != NULL)
            return entry;
    }

    return NULL;
}

void Assembly::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[9];
    metadata.GetRow(token, row);

    hashalg_ = row[0];
    major_ = row[1];
    minor_ = row[2];
    build_ = row[3];
    revision_ = row[4];
    flags_ = static_cast<Flags::AssemblyFlags::Values>(row[5]);
    publickey_ = metadata.GetBlob(row[6]);
    name_ = metadata.GetString(row[7]);
    culture_ = metadata.GetString(row[8]);

    Module *module(metadata.GetModule());
    modules_.Insert(module->GetName(), module);
}

void Assembly::LoadModules(Metadata &metadata) {
    _forall (uint32_t, token, NilToken(Tables::File), MakeToken(Tables::File, metadata.GetRows(Tables::File))) {
        File *file(metadata.GetToken<File>(token));
        if ((file->GetFlags() & Flags::FileAttributes::ContainsNoMetadata) != 0)
            continue;

        cse::String name(file->GetName());
        api::Cout << "Loading: " << name << ios::NewLine;

        // XXX: have to leak this to keep the memory mapped
        Metadata *metadata(new Metadata(name));

        Module *module(metadata->GetModule());
        modules_.Insert(module->GetName(), module);
    }
}

uint32_t Assembly::GetHashAlgId() const {
    return hashalg_;
}

uint16_t Assembly::GetMajorVersion() const {
    return major_;
}

uint16_t Assembly::GetMinorVersion() const {
    return minor_;
}

uint16_t Assembly::GetBuildNumber() const {
    return build_;
}

uint16_t Assembly::GetRevisionNumber() const {
    return revision_;
}

Flags::AssemblyFlags::Values Assembly::GetFlags() const {
    return flags_;
}

ext::SharedByteBlock Assembly::GetPublicKey() const {
    return publickey_;
}

const cse::String &Assembly::GetName() const {
    return name_;
}

const cse::String &Assembly::GetCulture() const {
    return culture_;
}

const Method *Assembly::GetEntryPoint() const {
    return GetEntryPoint_();
}

Method *Assembly::GetEntryPoint() {
    return GetEntryPoint_();
}

const Assembly::ModuleMap &Assembly::GetModules() const {
    return modules_;
}

void Assembly::PrepareAssembly() {
    OPT_TIMER("clr::Assembly::PrepareAssembly()")
    for (ModuleMap::Iterator module(modules_.Begin()); module != modules_.End(); ++module)
        module->Second()->PrepareModule();
}

}
