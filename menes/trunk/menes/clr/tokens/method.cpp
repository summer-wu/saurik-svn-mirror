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

#include "clr/element.hpp"
#include "clr/metadata.hpp"
#include "clr/utils.hpp"

#include "clr/net/implementation.hpp"

#include "clr/tokens/method.hpp"
#include "clr/tokens/param.hpp"
#include "clr/tokens/typedef.hpp"

namespace clr {

Method *Method::ResolveMethod_() const {
    return const_cast<Method *>(this);
}

TypeDef *Method::ResolveTypeDef_() const {
    _assert(false);
    return NULL;
}

void Method::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[6];
    metadata.GetRow(token, row);

    uint32_t next[6];
    bool more(metadata.GetRow(token + 1, next));

    rva_ = row[0];
    implflags_ = static_cast<Flags::MethodImplAttributes::Values>(row[1]);
    uint32_t flags(row[2]);
    name_ = metadata.GetString(row[3]);
    signature_.Parse(metadata, metadata.GetBlob(row[4]));

    uint32_t last(more ? next[5] : MakeToken(Tables::Param, metadata.GetRows(Tables::Param) + 1));
    parameters_.Reserve(last - row[5]);
    for (uint32_t token(row[5]); token != last; ++token)
        parameters_.InsertLast(metadata.GetToken<Param>(token));

    if (rva_ != 0 && (implflags_ & Flags::MethodImplAttributes::CodeTypeMask) == Flags::MethodImplAttributes::IL)
        implementation_ = new Net::Implementation(metadata, metadata.GetFile().FindRva(rva_));

    switch (flags & 0x0007) {
        case 0x0000: accessibility_ = Accessibility::CompilerControlled; break;
        case 0x0001: accessibility_ = Accessibility::Private; break;
        case 0x0002: accessibility_ = Accessibility::FamilyAndAssembly; break;
        case 0x0003: accessibility_ = Accessibility::Assembly; break;
        case 0x0004: accessibility_ = Accessibility::Family; break;
        case 0x0005: accessibility_ = Accessibility::FamilyOrAssembly; break;
        case 0x0006: accessibility_ = Accessibility::Public; break;
        default: _assert(false);
    }

    static_ = (flags & 0x0010) != 0;
    final_ = (flags & 0x0020) != 0;
    virtual_ = (flags & 0x0040) != 0;
    hidebysignature_ = (flags & 0x0080) != 0;

    abstract_ = (flags & 0x0400) != 0;
    specialname_ = (flags & 0x0800) != 0;

    pinvokeimpl_ = (flags & 0x2000) != 0;
    unmanagedexport_ = (flags & 0x0008) != 0;

    switch (flags & 0x0100) {
        case 0x0000: vtablelayout_ = VtableLayout::ReuseSlot; break;
        case 0x0100: vtablelayout_ = VtableLayout::NewSlot; break;
        default: _assert(false);
    }

    if (pinvokeimpl_) {
        api::Cout << name_ << ios::NewLine;
    }

    //RTSpecialName      = 0x1000,
    //HasSecurity        = 0x4000,
    //RequireSecObject   = 0x8000
}

void Method::SetType(TypeDef *type) {
    type_ = type;
}

Method::Method() :
    type_(NULL),
    implementation_(NULL)
{
}

Method::~Method() {
    delete implementation_;
}

uint32_t Method::GetRva() const {
    return rva_;
}

Flags::MethodImplAttributes::Values Method::GetImplFlags() const {
    return implflags_;
}

const cse::String &Method::GetName() const {
    return name_;
}

const Signature &Method::GetSignature() const {
    return signature_;
}

Signature &Method::GetSignature() {
    return signature_;
}

const Method::Parameters &Method::GetParameters() const {
    return parameters_;
}

const Implementation *Method::GetImplementation() const {
    return implementation_;
}

Implementation *Method::GetImplementation() {
    return implementation_;
}

const TypeDefOrRef *Method::GetType() const {
    return type_;
}

TypeDefOrRef *Method::GetType() {
    return type_;
}

}
