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

#include "clr/metadata.hpp"
#include "clr/tokens/exportedtype.hpp"

namespace clr {

void ExportedType::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[5];
    metadata.GetRow(token, row);

    flags_ = static_cast<Flags::TypeAttributes::Values>(row[0]);
    defid_ = row[1];
    name_ = metadata.GetString(row[2]);
    namespace_ = metadata.GetString(row[3]);
    impl_ = metadata.GetToken<Implementation>(row[4]);
}

ExportedType::ExportedType() {
}

Flags::TypeAttributes::Values ExportedType::GetFlags() const {
    return flags_;
}

uint32_t ExportedType::GetTypeDefId() const {
    return defid_;
}

const cse::String &ExportedType::GetTypeName() const {
    return name_;
}

const cse::String &ExportedType::GetTypeNamespace() const {
    return namespace_;
}

const Implementation *ExportedType::GetImplementation() const {
    return impl_;
}

Implementation *ExportedType::GetImplementation() {
    return impl_;
}

}
