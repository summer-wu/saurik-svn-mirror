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
#include "clr/signature.hpp"
#include "clr/tables.hpp"
#include "clr/utils.hpp"

#include "clr/codeds/typedeforref.hpp"

#include "clr/tokens/typedef.hpp"
#include "clr/tokens/typespec.hpp"

#include "ext/byteorder.hpp"
#include "ios/getput.hpp"

namespace clr {

Element::Element() :
    intrinsic_(Element::Void),
    type_(NULL),
    nested_(NULL),
    index_(~uint32_t())
{
}

Element::Element(Intrinsic intrinsic) :
    intrinsic_(intrinsic),
    type_(NULL),
    index_(~uint32_t())
{
    _assert(false);
}

Element::Element(Intrinsic intrinsic, const Element &elem) :
    intrinsic_(intrinsic),
    type_(NULL),
    index_(~uint32_t())
{
    extras_.InsertLast(elem);
    _assert(false);
}

Element::Element(Intrinsic intrinsic, TypeDefOrRef *type) :
    intrinsic_(intrinsic),
    type_(type),
    index_(~uint32_t())
{
}

/*Element::Element(const Signature &fnptr) :
{
}*/

bool Element::operator ==(const Element &rhs) const {
    if (intrinsic_ != rhs.intrinsic_)
        return false;

    if (type_ == NULL) {
        if (rhs.type_ != NULL)
            return false;
    } else if (rhs.type_ == NULL || type_ != rhs.type_ && type_->ResolveTypeDef() != rhs.type_->ResolveTypeDef())
        return false;

    if (nested_ == NULL) {
        if (rhs.nested_ != NULL)
            return false;
    } else if (rhs.nested_ == NULL || *nested_ != *rhs.nested_)
        return false;

    if (extras_ != rhs.extras_)
        return false;

    // XXX: custom modifiers
    return true;
}

bool Element::operator !=(const Element &rhs) const {
    return !operator ==(rhs);
}

void Element::Parse(Metadata &metadata, ios::Reader &blob) {
    intrinsic_ = static_cast<Intrinsic>(ios::GetLittle<uint8_t>(blob));

    while ((intrinsic_ & Element::Modifier) == Element::Modifier)
        intrinsic_ = static_cast<Intrinsic>(ios::GetLittle<uint8_t>(blob));

    switch (intrinsic_) {
        case Element::Void:
        case Element::Boolean:
        case Element::Char:
        case Element::I1:
        case Element::U1:
        case Element::I2:
        case Element::U2:
        case Element::I4:
        case Element::U4:
        case Element::I8:
        case Element::U8:
        case Element::R4:
        case Element::R8:
        case Element::String:
            break;

        case Element::Ptr:
        case Element::ByRef:
            extras_.InsertLast();
            extras_.Last().Parse(metadata, blob);
        break;

        case Element::ValueType:
        case Element::Class:
            type_ = metadata.GetToken<TypeDefOrRef>(Tables::coder.Decode(0x00, Uncompress<uint32_t>(blob)));
            _assert(dynamic_cast<TypeSpec *>(type_) == NULL);
        break;

        case Element::Parameter:
            index_ = Uncompress<uint32_t>(blob);
        break;

        case Element::Array:
            extras_.InsertLast();
            extras_.Last().Parse(metadata, blob);

            dims_.SetSize(Uncompress<uint32_t>(blob));
            _assert(dims_.GetSize() != 0);

            _forall (uint32_t, i, 0, Uncompress<uint32_t>(blob))
                dims_[i].size = Uncompress<uint32_t>(blob);

            _forall (uint32_t, i, 0, Uncompress<uint32_t>(blob))
                dims_[i].lower = Uncompress<uint32_t>(blob);
        break;

        case Element::Specialize:
            extras_.InsertLast();
            extras_.Last().Parse(metadata, blob);

            _forall (uint32_t, i, 0, Uncompress<uint32_t>(blob)) {
                extras_.InsertLast();
                extras_.Last().Parse(metadata, blob);
            }
        break;

        case Element::TypedByRef:
        case Element::I:
        case Element::U:
            break;

        case Element::FnPtr:
            nested_ = new _H<Signature>();
            nested_->Parse(metadata, blob);
        break;

        case Element::Object:
            break;

        case Element::SZArray:
            extras_.InsertLast();
            extras_.Last().Parse(metadata, blob);
        break;

        case Element::FuckIfIKnow:
            index_ = Uncompress<uint32_t>(blob);
        break;

        case Element::CModReqd:
        case Element::CModOpt:
            custom_.InsertLast(metadata.GetToken<TypeDefOrRef>(Tables::coder.Decode(0x00, Uncompress<uint32_t>(blob))));
            Parse(metadata, blob);
        break;

        default:
            api::Cerr << "Unknown Element Type: " << ios::SetBase(16) << intrinsic_ << ios::SetBase(10) << ios::NewLine;
            _assert(false);
    }
}

bool Element::IsVoid() const {
    return intrinsic_ == Element::Void;
}

Element::Intrinsic Element::GetIntrinsic() const {
    return intrinsic_;
}

const TypeDefOrRef *Element::GetType() const {
    return type_;
}

TypeDefOrRef *Element::GetType() {
    return type_;
}

const Signature *Element::GetNested() const {
    return nested_;
}

Signature *Element::GetNested() {
    return nested_;
}

const Element *Element::GetExtra() const {
    return &extras_.First();
}

const ElementList &Element::GetExtras() const {
    return extras_;
}

uint32_t Element::GetIndex() const {
    return index_;
}

uint32_t Element::GetSize() const {
    switch (intrinsic_) {
        case Void:      return 0;
        case Boolean:   return 1;
        case Char:      return 1;
        case I1:        return 1;
        case U1:        return 1;
        case I2:        return 1;
        case U2:        return 1;
        case I4:        return 1;
        case U4:        return 1;
        case I8:        return 1;
        case U8:        return 1;
        case R4:        return 1;
        case R8:        return 1;
        case String:    return 1;

        case Ptr:       return 1;
        case ByRef:     return 1;

        case ValueType: {
            TypeDef *type(type_->ResolveTypeDef());
            _assert(type != NULL);
            type->PrepareType();
            return type->GetPreparedSize();
        }

        case Class:     return 1;

        case Array:
        case TypedByRef:
            _assert(false);
        break;

        case I:         return 1;
        case U:         return 1;

        case FnPtr:     return 1;

        case Object:    return 1;

        case SZArray:   return 1;

        default:
            _assume(false);
    }

    _assert(false);
    return 0;
}

/*uint32_t Element::GetSize() const {
    switch (intrinsic_) {
        case Void:      return 0;
        case Boolean:   return 1;
        case Char:      return 2;
        case I1:        return 1;
        case U1:        return 1;
        case I2:        return 2;
        case U2:        return 2;
        case I4:        return 4;
        case U4:        return 4;
        case I8:        return 8;
        case U8:        return 8;
        case R4:        return 4;
        case R8:        return 8;
        case String:    return 4;

        case Ptr:       return 4;
        case ByRef:     return 4;

        case ValueType: {
            TypeDef *type = type_->ResolveTypeDef();
            _assert(type != NULL);
            type->PrepareType();
            return type->GetPreparedSize();
        }

        case Class:     return 4;

        case Array:
        case TypedByRef:
            _assert(false);
        break;

        case I:         return sizeof(int);
        case U:         return sizeof(unsigned);

        case FnPtr:     return 4;

        case Object:    return 4;

        case SZArray:   return 4;

        case CModReqd:
        case CModOpt:
        default:                            _assume(false);
    }

    return 0;
}*/

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Element &element) {
    switch (element.GetIntrinsic()) {
        case Element::Void:
            out << "void";
        break;

        case Element::Boolean:
            out << "bool";
        break;

        case Element::Char:
            out << "char";
        break;

        case Element::I1:
            out << "int8";
        break;

        case Element::U1:
            out << "unsigned int8";
        break;

        case Element::I2:
            out << "int16";
        break;

        case Element::U2:
            out << "unsigned int16";
        break;

        case Element::I4:
            out << "int32";
        break;

        case Element::U4:
            out << "unsigned int32";
        break;

        case Element::I8:
            out << "int64";
        break;

        case Element::U8:
            out << "unsigned int64";
        break;

        case Element::R4:
            out << "float32";
        break;

        case Element::R8:
            out << "float64";
        break;

        case Element::String:
            out << "string";
        break;

        case Element::Ptr:
            out << *element.GetExtra() << " *";
        break;

        case Element::ByRef:
            out << *element.GetExtra() << " &";
        break;

        case Element::ValueType:
            out << "valuetype " << element.GetType()->GetName();
        break;

        case Element::Class:
            out << "class " << element.GetType()->GetName();
        break;

        case Element::Array:
            out << *element.GetExtra() << "[...]";
        break;

        case Element::TypedByRef:
            _assert(false);
        break;

        case Element::I:
            out << "native int";
        break;

        case Element::U:
            out << "native unsigned int";
        break;

        case Element::FnPtr:
            out << "method " << element.GetNested();
        break;

        case Element::Object:
            out << "object";
        break;

        case Element::SZArray:
            out << *element.GetExtra() << "[]";
        break;

        case Element::CModReqd:
        case Element::CModOpt:
            out << "FUCK";
        break;

        default:
            _assume(false);
    }

    return out;
}

}
