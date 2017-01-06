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
#include "clr/tables.hpp"
#include "clr/utils.hpp"

#include "clr/codeds/memberrefparent.hpp"
#include "clr/codeds/typedeforref.hpp"

#include "clr/tokens/field.hpp"
#include "clr/tokens/memberref.hpp"
#include "clr/tokens/method.hpp"
#include "clr/tokens/typedef.hpp"

namespace clr {

Field *MemberRef::ResolveField_() const {
    // XXX: better as cached_ boolean
    if (cache_ != NULL)
        return dynamic_cast<Field *>(cache_);

    TypeDef *type(class_->ResolveTypeDef());
    _assert(type != NULL);

    Field *field(type->FindField(name_, signature_));
    _assert(field != NULL);
    cache_ = field;

    return field;
}

Method *MemberRef::ResolveMethod_() const {
    // XXX: better as cached_ boolean
    if (cache_ != NULL)
        return dynamic_cast<Method *>(cache_);

    TypeDef *type(class_->ResolveTypeDef());
    _assert(type != NULL);

    Method *method(type->FindMethod(name_, signature_));
    _assert(method != NULL);
    cache_ = method;

    return method;
}

TypeDefOrRef *MemberRef::GetType_() const {
    // XXX: implement
    TypeDefOrRef *parent = dynamic_cast<TypeDefOrRef *>(class_);
    if (parent != NULL)
        return parent;
    _assert(false);
    return NULL;
}

void MemberRef::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[3];
    metadata.GetRow(token, row);

    class_ = metadata.GetToken<MemberRefParent>(row[0]);
    name_ = metadata.GetString(row[1]);
    signature_.Parse(metadata, metadata.GetBlob(row[2]));
}

MemberRef::MemberRef() :
    cache_(NULL)
{
}

const MemberRefParent *MemberRef::GetClass() const {
    return class_;
}

MemberRefParent *MemberRef::GetClass() {
    return class_;
}

const cse::String &MemberRef::GetName() const {
    return name_;
}

const Signature &MemberRef::GetSignature() const {
    return signature_;
}

Signature &MemberRef::GetSignature() {
    return signature_;
}

const Method *MemberRef::ResolveMethod() const {
    return ResolveMethod_();
}

Method *MemberRef::ResolveMethod() {
    return ResolveMethod_();
}

const TypeDefOrRef *MemberRef::GetType() const {
    return GetType_();
}

TypeDefOrRef *MemberRef::GetType() {
    return GetType_();
}

}
