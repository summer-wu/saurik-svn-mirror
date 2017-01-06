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

#include "clr/ecma.hpp"
#include "clr/element.hpp"
#include "clr/metadata.hpp"
#include "clr/signature.hpp"
#include "clr/utils.hpp"

#include "clr/codeds/methoddeforref.hpp"
#include "clr/codeds/typedeforref.hpp"

#include "clr/tokens/customattribute.hpp"
#include "clr/tokens/typedef.hpp"

#include "cse/utf16.hpp"
#include "ios/getput.hpp"

namespace clr {

void CustomAttribute::Parse(Metadata &metadata, uint32_t token) {
    uint32_t row[3];
    metadata.GetRow(token, row);

    metadata.GetToken<HasCustomAttribute>(row[0])->AddAttribute(this);
    type_ = metadata.GetToken<MethodDefOrRef>(row[1]);

    ext::SharedByteBlock blob(metadata.GetBlob(row[2]));
    // XXX: this isn't bounds checked correctly
    const char *value(blob);

    uint16_t prolog(ios::GetLittle<uint16_t>(value));
    EcmaAssert("Partition II, 22.3", prolog == 0x0001);

    const ElementList &elements(type_->GetSignature().GetArguments(type_->GetType()));
    for (ElementList::ConstIterator element(elements.Begin() + 1); element != elements.End(); ++element) {
        switch (element->GetIntrinsic()) {
            case Element::Boolean:
                fixed_.InsertLast(ios::GetLittle<uint32_t>(value) != 0);
            break;

            case Element::Char:
                fixed_.InsertLast(int64_t(ios::GetLittle<cse::Utf16Encoding::CodeUnit>(value)));
            break;

            case Element::I1:
            case Element::U1:
                fixed_.InsertLast(int64_t(ios::GetLittle<int8_t>(value)));
            break;

            case Element::I2:
            case Element::U2:
                fixed_.InsertLast(int64_t(ios::GetLittle<int16_t>(value)));
            break;

            case Element::I4:
            case Element::U4:
                fixed_.InsertLast(int64_t(ios::GetLittle<int32_t>(value)));
            break;

            case Element::I8:
            case Element::U8:
                fixed_.InsertLast(ios::GetLittle<int64_t>(value));
            break;

            case Element::R4:
                fixed_.InsertLast(ios::GetLittle<float>(value));
            break;

            case Element::R8:
                fixed_.InsertLast(ios::GetLittle<double>(value));
            break;

            case Element::String: {
                uint32_t length(Uncompress<uint32_t>(value));
                cse::String string(value, length);
                value += length;
                fixed_.InsertLast(string);
            } break;

            case Element::ValueType: {
                //_assert(false);
                /*switch (element->GetType()->ResolveTypeDef()->GetSize()) {
                    case 1: fixed_.InsertLast(int64_t(ios::GetLittle<int8_t>(value))); break;
                    case 2: fixed_.InsertLast(int64_t(ios::GetLittle<int16_t>(value))); break;
                    case 4: fixed_.InsertLast(int64_t(ios::GetLittle<int32_t>(value))); break;
                    case 8: fixed_.InsertLast(int64_t(ios::GetLittle<int64_t>(value))); break;

                    default:
                        _assert(false);
                }*/
            } break;

            case Element::Object:
                throw ext::NotImplementedException();

            // XXX: this needs to handle Enums

            default:
                _assert(false);
        }
    }

    uint16_t named(ios::GetLittle<uint16_t>(value));
    for (uint16_t i(0); i != named; ++i) {
    }
}

CustomAttribute::CustomAttribute() {
}

const MethodDefOrRef *CustomAttribute::GetType() const {
    return type_;
}

MethodDefOrRef *CustomAttribute::GetType() {
    return type_;
}

const CustomAttribute::Fixed &CustomAttribute::GetFixed() const {
    return fixed_;
}

}
