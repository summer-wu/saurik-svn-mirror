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

#ifndef NINETJER_METHOD_HPP
#define NINETJER_METHOD_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/properties.hpp"
#include "cse/string.hpp"
#include "ext/vector.hpp"

#include "clr/accessibility.hpp"
#include "clr/flags.hpp"
#include "clr/signature.hpp"
#include "clr/token.hpp"

#include "clr/codeds/hascustomattribute.hpp"
#include "clr/codeds/hasdeclsecurity.hpp"
#include "clr/codeds/member.hpp"
#include "clr/codeds/memberforwarded.hpp"
#include "clr/codeds/memberrefparent.hpp"
#include "clr/codeds/methoddeforref.hpp"

namespace clr {

class Implementation;
class Param;

namespace VtableLayout {
enum Type {
    ReuseSlot,
    NewSlot
}; }

template <typename Type_>
class TokenCache;

/*0x06*/ class Method :
    public virtual Token,
    public Member,
    public HasCustomAttribute,
    public HasDeclSecurity,
    public MemberRefParent,
    public MethodDefOrRef,
    public MemberForwarded
{
    friend class TypeDef;

  public:
    typedef ext::Vector<Param *> Parameters;

  private:
    uint32_t rva_;
    Flags::MethodImplAttributes::Values implflags_;
    cse::String name_;
    _S<Signature> signature_;
    Parameters parameters_;

    TypeDef *type_;
    Implementation *implementation_;

    virtual Method *ResolveMethod_() const;
    virtual TypeDef *ResolveTypeDef_() const;

  protected:
    virtual void Parse(Metadata &metadata, uint32_t token);
    void SetType(TypeDef *type);

    EXT_PROPERTY(Accessibility::Type, accessibility_, Accessibility)
    EXT_PROPERTY(VtableLayout::Type, vtablelayout_, VtableLayout)

    EXT_PROPERTY(bool, static_, Static)
    EXT_PROPERTY(bool, final_, Final)
    EXT_PROPERTY(bool, virtual_, Virtual)
    EXT_PROPERTY(bool, hidebysignature_, HideBySignature)
    EXT_PROPERTY(bool, abstract_, Abstract)
    EXT_PROPERTY(bool, specialname_, SpecialName)
    EXT_PROPERTY(bool, pinvokeimpl_, PinvokeImpl)
    EXT_PROPERTY(bool, unmanagedexport_, UnmanagedExport)

  public:
    Method();
    ~Method();

    // Method Table

    uint32_t GetRva() const;
    Flags::MethodImplAttributes::Values GetImplFlags() const;

    virtual const cse::String &GetName() const;

    virtual const Signature &GetSignature() const;
    virtual Signature &GetSignature();

    const Parameters &GetParameters() const;

    // Method Object

    const Implementation *GetImplementation() const;
    Implementation *GetImplementation();

    // MethodDefOrRef Interface

    // XXX: g++ conformance - TypeDefOrRef -> TypeDef
    const TypeDefOrRef *GetType() const;
    TypeDefOrRef *GetType();
};

}

#endif//NINETJER_METHOD_HPP
