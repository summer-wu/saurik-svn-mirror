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
#include "clr/signature.hpp"
#include "clr/utils.hpp"

#include "ext/byteorder.hpp"
#include "ext/foreach.hpp"

#include "ios/getput.hpp"
#include "ios/iterated.hpp"

namespace clr {

Signature::Signature()
{
}

/*Signature::Signature(Flags::CallingConvention::Values calling, const Element &result) :
    calling_(calling)
{
    arguments_.InsertLast(result);
}

Signature::Signature(Flags::CallingConvention::Values calling, const Element &result, const SignatureList &list) :
    calling_(calling)
{
    arguments_.InsertLast(result);
    arguments_.InsertAllLast(list.GetElements());
}*/

bool Signature::operator ==(const Signature &signature) const {
    return calling_ == signature.calling_ &&
           arguments_ == signature.arguments_;
}

bool Signature::operator !=(const Signature &signature) const {
    return !operator ==(signature);
}

void Signature::Parse(Metadata &metadata, ios::Reader &blob) {
    calling_ = static_cast<Flags::CallingConvention::Values>(ios::GetLittle<uint8_t>(blob));

    size_t arguments(0);
    if ((calling_ & Flags::CallingConvention::Mask) != Flags::CallingConvention::Field)
        arguments = Uncompress<uint32_t>(blob);
    arguments_.Reserve(arguments);

    if ((calling_ & Flags::CallingConvention::Mask) != Flags::CallingConvention::LocalSig) {
        result_ = new _H<Element>();
        result_->Parse(metadata, blob);
    }

    if ((calling_ & Flags::CallingConvention::HasThis) == 0)
        hasthis_ = None;
    else if ((calling_ & Flags::CallingConvention::ExplicitThis) == 0)
        hasthis_ = Implicit;
    else
        hasthis_ = Explicit;

    for (uint32_t i(0); i != arguments; ++i) {
        arguments_.InsertLast();
        arguments_.Last().Parse(metadata, blob);
        // XXX: this needs to check for the optional sentinal data for VarArg/C conventions
        _assert(arguments_.Last().GetIntrinsic() != Element::Sentinal);
    }
}

void Signature::Parse(Metadata &metadata, const ext::SharedByteBlock &blob) {
    // XXX: this isn't bounds checked correctly
    _S<ios::MemoryReader> stream(blob);
    Parse(metadata, stream);
}

Flags::CallingConvention::Values Signature::GetCalling() const {
    return calling_;
}

ElementList Signature::GetArguments(TypeDefOrRef *type) const {
    if (hasthis_ != Implicit)
        return arguments_;

    _assert(type != NULL);

    ElementList arguments;
    arguments.Reserve(arguments_.GetSize() + 1);
    arguments.InsertLast(_S<Element>(Element::Class, type));
    arguments.InsertAllLast(arguments_);

    return arguments;
}

const Element &Signature::GetResult() const {
    _assert(!result_.IsEmpty());
    return *result_;
}

Element &Signature::GetResult() {
    _assert(!result_.IsEmpty());
    return *result_;
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Signature &signature) {
    out << signature.GetResult() << " * (";

    const ElementList &elements(signature.GetArguments());
    _foreach (const ElementList, element, elements) {
        if (element != elements.Begin())
            out << ", ";
        out << *element;
    }

    out << ")";
    return out;
}

}
