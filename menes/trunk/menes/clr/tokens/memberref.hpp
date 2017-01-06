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

#ifndef NINETJER_MEMBERREF_HPP
#define NINETJER_MEMBERREF_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/codeds/fielddeforref.hpp"
#include "clr/codeds/hascustomattribute.hpp"
#include "clr/codeds/member.hpp"
#include "clr/codeds/methoddeforref.hpp"

#include "clr/signature.hpp"
#include "clr/token.hpp"

#include "cse/string.hpp"

namespace clr {

class MemberRefParent;

/*0x0a*/ class MemberRef :
    public virtual Token,
    public Member,
    public FieldDefOrRef,
    public HasCustomAttribute,
    public MethodDefOrRef
{
  private:
    MemberRefParent *class_;
    cse::String name_;
    _S<Signature> signature_;

    mutable Token *cache_;

    Field *ResolveField_() const;
    Method *ResolveMethod_() const;

    TypeDefOrRef *GetType_() const;

  protected:
    virtual void Parse(Metadata &metadata, uint32_t token);

  public:
    MemberRef();

    // MememberRef Object

    const MemberRefParent *GetClass() const;
    MemberRefParent *GetClass();

    virtual const cse::String &GetName() const;

    virtual const Signature &GetSignature() const;
    virtual Signature &GetSignature();

    // MethodDefOrRef Object

    virtual const Method *ResolveMethod() const;
    virtual Method *ResolveMethod();

    const TypeDefOrRef *GetType() const;
    TypeDefOrRef *GetType();
};

}

#endif//NINETJER_MEMBERREF_HPP
