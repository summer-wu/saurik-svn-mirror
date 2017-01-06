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

#include "clr/metadata.hpp"
#include "clr/utils.hpp"

#include "clr/tokens/classlayout.hpp"
#include "clr/tokens/field.hpp"
#include "clr/tokens/method.hpp"
#include "clr/tokens/typedef.hpp"

#include "etl/alignment.hpp"

namespace clr {

template <typename Token_>
_finline void TypeDef::SetType_(Metadata &metadata, uint32_t row, uint32_t next, bool more, uint32_t type, ext::Vector<Token_ *> &list) {
    uint32_t last(more ? next : MakeToken(type, metadata.GetRows(type) + 1));
    list.Reserve(last - row);

    for (uint32_t token(row); token != last; ++token) {
        Token_ *value(metadata.template GetToken<Token_>(token));
        value->SetType(this);
        list.InsertLast(value);
    }
}

Field *TypeDef::FindField_(const cse::String &name, const Signature &signature) const {
    OPT_TIMER("TypeDef::FindField_()")
    for (FieldList::ConstIterator field(fields_.Begin()); field != fields_.End(); ++field)
        if ((*field)->GetName() == name && (*field)->GetSignature() == signature)
            return *field;
    return NULL;
}

Method *TypeDef::FindMethod_(const cse::String &name, const Signature &signature) const {
    OPT_TIMER("TypeDef::FindMethod_()")
    for (MethodList::ConstIterator method(methods_.Begin()); method != methods_.End(); ++method)
        if ((*method)->GetName() == name && (*method)->GetSignature() == signature)
            return *method;
    return NULL;
}

TypeDef *TypeDef::FindTypeDef_(const cse::String &space, const cse::String &name) const {
    // XXX: implement
    _assert(false);
    return NULL;
}

TypeDef *TypeDef::ResolveTypeDef_() const {
    return const_cast<TypeDef *>(this);
}

void TypeDef::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[6];
    metadata.GetRow(token, row);

    uint32_t next[6];
    bool more(metadata.GetRow(token + 1, next));

    Flags::TypeAttributes::Values flags(static_cast<Flags::TypeAttributes::Values>(row[0]));
    name_ = metadata.GetString(row[1]);
    namespace_ = metadata.GetString(row[2]);
    extends_ = metadata.GetToken<TypeDefOrRef>(row[3]);

    SetType_(metadata, row[4], next[4], more, Tables::Field, fields_);
    SetType_(metadata, row[5], next[5], more, Tables::Method, methods_);

    switch (flags & 0x00000007) {
        case 0x00000000: flags_[_B("net:visibility")] = _B("not-public"); break;
        case 0x00000001: flags_[_B("net:visibility")] = _B("public"); break;
        case 0x00000002: flags_[_B("net:visibility")] = _B("nested-public"); break;
        case 0x00000003: flags_[_B("net:visibility")] = _B("nested-private"); break;
        case 0x00000004: flags_[_B("net:visibility")] = _B("nested-family"); break;
        case 0x00000005: flags_[_B("net:visibility")] = _B("nested-assembly"); break;
        case 0x00000006: flags_[_B("net:visibility")] = _B("nested-family-and-assembly"); break;
        case 0x00000007: flags_[_B("net:visibility")] = _B("nested-family-or-assembly"); break;
    }

    switch (flags & 0x00000018) {
        case 0x00000000: flags_[_B("net:layout")] = _B("auto"); break;
        case 0x00000008: flags_[_B("net:layout")] = _B("sequential"); break;
        case 0x00000010: flags_[_B("net:layout")] = _B("explicit"); break;
    }

    switch (flags & 0x00000020) {
        case 0x00000000: flags_[_B("net:semantics")] = _B("class"); break;
        case 0x00000020: flags_[_B("net:semantics")] = _B("interface"); break;
    }

    flags_[_B("net:abstract")] = (flags & 0x00000080) != 0;
    flags_[_B("net:sealed")] = (flags & 0x00000100) != 0;
    flags_[_B("net:special-name")] = (flags & 0x00000400) != 0;

    flags_[_B("net:import")] = (flags & 0x00001000) != 0;
    flags_[_B("net:serializable")] = (flags & 0x00002000) != 0;

    switch (flags & 0x00030000) {
        case 0x00000000: flags_[_B("net:string-format")] = _B("ansi"); break;
        case 0x00010000: flags_[_B("net:string-format")] = _B("unicode"); break;
        case 0x00020000: flags_[_B("net:string-format")] = _B("auto"); break;
    }

    flags_[_B("net:before-field-init")] = (flags & Flags::TypeAttributes::BeforeFieldInit) != 0;

    flags_[_B("net:rt-special-name")] = (flags & Flags::TypeAttributes::RTSpecialName) != 0;
    flags_[_B("net:has-security")] = (flags & Flags::TypeAttributes::RTSpecialName) != 0;
}

TypeDef::TypeDef() :
    extends_(NULL),
    encloser_(NULL),
    layout_(NULL),
    prepared_(false)
{
}

const FlagMap &TypeDef::GetFlagMap() const {
    return flags_;
}

const cse::String &TypeDef::GetName() const {
    return name_;
}

void TypeDef::SetName(const cse::String &name) {
    name_ = name;
}

const cse::String &TypeDef::GetNamespace() const {
    return namespace_;
}

void TypeDef::SetNamespace(const cse::String &space) {
    namespace_ = space;
}

const TypeDefOrRef *TypeDef::GetExtends() const {
    return extends_;
}

TypeDefOrRef *TypeDef::GetExtends() {
    return extends_;
}

void TypeDef::SetExtends(TypeDefOrRef *extends) {
    extends_ = extends;
}

const TypeDef::FieldList &TypeDef::GetFieldList() const {
    return fields_;
}

const TypeDef::MethodList &TypeDef::GetMethodList() const {
    return methods_;
}

const ClassLayout *TypeDef::GetLayout() const {
    return layout_;
}

ClassLayout *TypeDef::GetLayout() {
    return layout_;
}

void TypeDef::SetLayout(ClassLayout *layout) {
    layout_ = layout;
}

const TypeDef *TypeDef::GetEncloser() const {
    return encloser_;
}

TypeDef *TypeDef::GetEncloser() {
    return encloser_;
}

void TypeDef::SetEncloser(TypeDef *encloser) {
    if (encloser_ != NULL)
        encloser_->nesteds_.Remove(name_);
    encloser_ = encloser;
    nestedmap_ &nesteds(encloser_->nesteds_);

    _assert(namespace_.IsEmpty());
    _assert(!nesteds.Contains(name_));

    nesteds.Insert(name_, this);
}

const Field *TypeDef::FindField(const cse::String &name, const Signature &signature) const {
    return FindField_(name, signature);
}

Field *TypeDef::FindField(const cse::String &name, const Signature &signature) {
    return FindField_(name, signature);
}

const Method *TypeDef::FindMethod(const cse::String &name, const Signature &signature) const {
    return FindMethod_(name, signature);
}

Method *TypeDef::FindMethod(const cse::String &name, const Signature &signature) {
    return FindMethod_(name, signature);
}

void TypeDef::Implement(TypeDefOrRef *interface) {
    ifaces_.InsertLast(interface);
}

void TypeDef::PrepareType() {
    OPT_TIMER("clr::TypeDef::PrepareType()")

    if (prepared_)
        return;

    if (extends_ == NULL)
        // XXX: vtable
        poffset_ = 0;
    else {
        TypeDef *extends(extends_->ResolveTypeDef());
        _assert(extends != NULL);
        extends->PrepareType();
        poffset_ = extends->GetPreparedOffset() + extends->GetPreparedSize();
    }

    if (true) {
        psize_ = 0;
        for (FieldList::ConstIterator field(fields_.Begin()); field != fields_.End(); ++field) {
            Flags::FieldAttributes::Values flags = (*field)->GetFlags();
            if ((flags & Flags::FieldAttributes::Static) != 0)
                continue; // XXX: deal with these

            (*field)->PrepareField(psize_);

            size_t size((*field)->GetSize());
            //psize_ = etl::AlignValue(psize_ + size, 4);
        }
    } else {
        psize_ = layout_->GetClassSize();
    }

    //std::cout << namespace_ << "." << name_ << " = " << psize_ << std::endl;

    prepared_ = true;
}

uint32_t TypeDef::GetPreparedOffset() const {
    _assert(prepared_);
    return poffset_;
}

uint32_t TypeDef::GetPreparedSize() const {
    _assert(prepared_);
    return psize_;
}

bool TypeDef::IsNested() const {
    return flags_[_B("net:visibility")].operator cse::String().StartsWithAll("nested-");
}

}
