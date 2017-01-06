/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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
#include "clr/storage.hpp"

#include "clr/net/implementation.hpp"
#include "clr/net/parse.hpp"

#include "clr/tokens/standalonesig.hpp"

namespace clr {
namespace Net {

Implementation::Implementation(Metadata &metadata, const char *base) :
    root_(NULL)
{
    const ILHeader::Tiny *tinyhdr(reinterpret_cast<const ILHeader::Tiny *>(base));
    if (tinyhdr->Flags == ILHeader::TinyFormat) {
        stack_ = 8;
        code_.Assign(base + sizeof(ILHeader::Tiny), tinyhdr->Size, ext::ReadOnlyExternal);
    } else {
        const ILHeader::Fat *fathdr(reinterpret_cast<const ILHeader::Fat *>(base));

        stack_ = fathdr->MaxStack;
        code_.Assign(base += fathdr->Size * 4, fathdr->CodeSize, ext::ReadOnlyExternal);
        if (fathdr->LocalVarSigTok != 0)
            locals_ = metadata.GetToken<StandAloneSig>(fathdr->LocalVarSigTok)->GetSignature();

        // XXX: parse extra sections
    }

    Parse(metadata);
}

Implementation::~Implementation() {
}

ext::SharedByteBlock Implementation::GetCode() const {
    return code_;
}

const Signature &Implementation::GetLocals() const {
    return locals_;
}

Signature &Implementation::GetLocals() {
    return locals_;
}

ValueList Implementation::GetLocalsValueList() const {
    ValueList list;
    const ElementList &elems(locals_.GetArguments());
    list.InsertAllLast(elems.Begin(), elems.GetSize());
    return list;
}

Instruction *Implementation::GetRoot() {
    return root_;
}

void Implementation::Parse(Metadata &metadata) {
    root_ = Net::ParseInstruction(metadata, code_);
}

} }
