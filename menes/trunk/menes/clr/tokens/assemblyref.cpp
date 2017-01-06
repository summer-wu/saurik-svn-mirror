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

#include "clr/appdomain.hpp"
#include "clr/metadata.hpp"

#include "clr/tokens/assembly.hpp"
#include "clr/tokens/assemblyref.hpp"

namespace clr {

Assembly *AssemblyRef::ResolveAssembly_() const {
    /*Assembly *assembly(module_->GetAssembly());
    _assert(assembly != NULL);

    AppDomain *domain(assembly->GetDomain());
    _assert(domain != NULL);

    cache_ = domain->Load(name_);
    _assert(cache_ != NULL);

    return cache_;*/

    _assert(false);
    return NULL;
}

HasTypeDef *AssemblyRef::ResolveHasTypeDef_() const {
    return ResolveAssembly_();
}

void AssemblyRef::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[9];
    metadata.GetRow(token, row);

    major_ = row[0];
    minor_ = row[1];
    build_ = row[2];
    revision_ = row[3];
    flags_ = static_cast<Flags::AssemblyFlags::Values>(row[4]);
    publickey_ = metadata.GetBlob(row[5]);
    name_ = metadata.GetString(row[6]);
    culture_ = metadata.GetString(row[7]);
    hash_ = metadata.GetBlob(row[8]);
}

AssemblyRef::AssemblyRef() :
    cache_(NULL)
{
}

uint16_t AssemblyRef::GetMajorVersion() const {
    return major_;
}

uint16_t AssemblyRef::GetMinorVersion() const {
    return minor_;
}

uint16_t AssemblyRef::GetBuildNumber() const {
    return build_;
}

uint16_t AssemblyRef::GetRevisionNumber() const {
    return revision_;
}

Flags::AssemblyFlags::Values AssemblyRef::GetFlags() const {
    return flags_;
}

ext::SharedByteBlock AssemblyRef::GetPublicKeyOrToken() const {
    return publickey_;
}

const cse::String &AssemblyRef::GetName() const {
    return name_;
}

const cse::String &AssemblyRef::GetCulture() const {
    return culture_;
}

ext::SharedByteBlock AssemblyRef::GetHash() const {
    return hash_;
}

}
