/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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
#include "clr/codeds/methoddeforref.hpp"

#include "clr/tokens/methodimpl.hpp"
#include "clr/tokens/typedef.hpp"

namespace clr {

void MethodImpl::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[3];
    metadata.GetRow(token, row);

    class_ = metadata.GetToken<TypeDef>(row[0]);
    body_ = metadata.GetToken<MethodDefOrRef>(row[1]);
    decl_ = metadata.GetToken<MethodDefOrRef>(row[2]);
}

MethodImpl::MethodImpl() {
}

const TypeDef *MethodImpl::GetClass() const {
    return class_;
}

TypeDef *MethodImpl::GetClass() {
    return class_;
}

const MethodDefOrRef *MethodImpl::GetMethodBody() const {
    return body_;
}

MethodDefOrRef *MethodImpl::GetMethodBody() {
    return body_;
}

const MethodDefOrRef *MethodImpl::GetMethodDeclaration() const {
    return decl_;
}

MethodDefOrRef *MethodImpl::GetMethodDeclaration() {
    return decl_;
}

}
