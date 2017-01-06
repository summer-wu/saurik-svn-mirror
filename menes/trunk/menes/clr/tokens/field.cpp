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

#include "clr/tokens/field.hpp"

#include "clr/element.hpp"
#include "clr/metadata.hpp"

namespace clr {

Field *Field::ResolveField_() const {
    return const_cast<Field *>(this);
}

void Field::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[3];
    metadata.GetRow(token, row);

    flags_ = static_cast<Flags::FieldAttributes::Values>(row[0]);
    name_ = metadata.GetString(row[1]);
    signature_.Parse(metadata, metadata.GetBlob(row[2]));
}

void Field::SetType(TypeDef *type) {
    type_ = type;
}

Field::Field() :
    layout_(NULL),
    rva_(NULL),
    type_(NULL),
    prepared_(false),
    poffset_(0)
{
}

Flags::FieldAttributes::Values Field::GetFlags() const {
    return flags_;
}

const cse::String &Field::GetName() const {
    return name_;
}

void Field::SetName(const cse::String &name) {
    name_ = name;
}

const Signature &Field::GetSignature() const {
    return signature_;
}

Signature &Field::GetSignature() {
    return signature_;
}

bool Field::IsStatic() const {
    return (GetFlags() & Flags::FieldAttributes::Static) != 0;
}

const FieldLayout *Field::GetLayout() const {
    return layout_;
}

void Field::SetLayout(FieldLayout *layout) {
    layout_ = layout;
}

const FieldRva *Field::GetRva() const {
    return rva_;
}

void Field::SetRva(FieldRva *rva) {
    rva_ = rva;
}

const TypeDef *Field::GetType() const {
    return type_;
}

TypeDef *Field::GetType() {
    return type_;
}

void Field::PrepareField(uint32_t offset) {
    if (prepared_) {
        _assert(offset == poffset_);
        return;
    }

    poffset_ = offset;
    prepared_ = true;
}

uint32_t Field::GetPreparedOffset() const {
    _assert(prepared_);
    return poffset_;
}

uint32_t Field::GetSize() const {
    return signature_.GetResult().GetSize();
}

}
