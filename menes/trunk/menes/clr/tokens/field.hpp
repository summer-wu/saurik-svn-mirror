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

#ifndef NINETJER_FIELD_HPP
#define NINETJER_FIELD_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/flags.hpp"
#include "clr/signature.hpp"
#include "clr/token.hpp"

#include "clr/codeds/fielddeforref.hpp"
#include "clr/codeds/hasconstant.hpp"
#include "clr/codeds/hascustomattribute.hpp"
#include "clr/codeds/hasfieldmarshal.hpp"
#include "clr/codeds/member.hpp"
#include "clr/codeds/memberforwarded.hpp"

#include "clr/tokens/typedef.hpp"

#include "cse/string.hpp"

namespace clr {

class FieldLayout;
class FieldRva;
class TypeDef;

/*0x04*/ class Field :
    public virtual Token,
    public Member,
    public FieldDefOrRef,
    public HasConstant,
    public HasCustomAttribute,
    public HasFieldMarshal,
    public MemberForwarded
{
    friend class TypeDef;

  private:
    Flags::FieldAttributes::Values flags_;
    cse::String name_;
    _S<Signature> signature_;

    FieldLayout *layout_;
    FieldRva *rva_;

    TypeDef *type_;

    bool prepared_;
    uint32_t poffset_;

    virtual Field *ResolveField_() const;

  protected:
    virtual void Parse(Metadata &metadata, uint32_t token);
    void SetType(TypeDef *type);

  public:
    Field();

    // FieldTable

    Flags::FieldAttributes::Values GetFlags() const;

    const cse::String &GetName() const;
    void SetName(const cse::String &name);

    const Signature &GetSignature() const;
    Signature &GetSignature();

    bool IsStatic() const;

    const FieldLayout *GetLayout() const;
    void SetLayout(FieldLayout *layout);

    const FieldRva *GetRva() const;
    void SetRva(FieldRva *rva);

    // Field Object

    const TypeDef *GetType() const;
    TypeDef *GetType();

    void PrepareField(uint32_t offset);

    uint32_t GetPreparedOffset() const;
    uint32_t GetSize() const;
};

}

#endif//NINETJER_FIELD_HPP
