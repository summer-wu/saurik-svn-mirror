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

#ifndef NINETJER_TYPEDEF_HPP
#define NINETJER_TYPEDEF_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "cse/string.hpp"
#include "ext/vector.hpp"

#include "clr/flags.hpp"
#include "clr/codeds/hascustomattribute.hpp"
#include "clr/codeds/hasdeclsecurity.hpp"
#include "clr/hastypedef.hpp"
#include "clr/token.hpp"

#include "clr/codeds/member.hpp"
#include "clr/codeds/typedeforref.hpp"

namespace clr {

class ClassLayout;
class Field;
class Layout;
class Method;
class Metadata;
class Signature;

/*0x02*/ class TypeDef :
    public virtual Token,
    public Member,
    public TypeDefOrRef,
    public HasCustomAttribute,
    public HasDeclSecurity,
    public HasTypeDef
{
  public:
    typedef ext::Vector<Field *> FieldList;
    typedef ext::Vector<Method *> MethodList;
    typedef ext::Vector<TypeDefOrRef *> InterfaceList;

  private:
    FlagMap flags_;

    cse::String name_;
    cse::String namespace_;
    TypeDefOrRef *extends_;
    FieldList fields_;
    MethodList methods_;

    InterfaceList ifaces_;

    typedef ext::RedBlackMap<cse::String, TypeDef *> nestedmap_;
    nestedmap_ nesteds_;

    TypeDef *encloser_;
    ClassLayout *layout_;

    bool prepared_;
    uint32_t poffset_;
    uint32_t psize_;

    virtual Field *FindField_(const cse::String &name, const Signature &signature) const;
    virtual Method *FindMethod_(const cse::String &name, const Signature &signature) const;
    virtual TypeDef *FindTypeDef_(const cse::String &space, const cse::String &name) const;
    virtual TypeDef *ResolveTypeDef_() const;

  protected:
    template <typename Token_>
    void SetType_(Metadata &metadata, uint32_t row, uint32_t next, bool more, uint32_t type, ext::Vector<Token_ *> &list);

    virtual void Parse(Metadata &metadata, uint32_t token);

  public:
    TypeDef();

    // TypeDef Table

    const FlagMap &GetFlagMap() const;

    virtual const cse::String &GetName() const;
    void SetName(const cse::String &name);

    virtual const cse::String &GetNamespace() const;
    void SetNamespace(const cse::String &space);

    const TypeDefOrRef *GetExtends() const;
    TypeDefOrRef *GetExtends();
    void SetExtends(TypeDefOrRef *extends);

    const FieldList &GetFieldList() const;
    const MethodList &GetMethodList() const;
    const InterfaceList &GetInterfaceList() const;

    const ClassLayout *GetLayout() const;
    ClassLayout *GetLayout();
    void SetLayout(ClassLayout *layout);

    // NestedClass Table

    const TypeDef *GetEncloser() const;
    TypeDef *GetEncloser();
    void SetEncloser(TypeDef *encloser);

    // TypeDef Object

    const Field *FindField(const cse::String &name, const Signature &signature) const;
    Field *FindField(const cse::String &name, const Signature &signature);

    const Method *FindMethod(const cse::String &name, const Signature &signature) const;
    Method *FindMethod(const cse::String &name, const Signature &signature);

    void Implement(TypeDefOrRef *interface);

    void PrepareType();

    uint32_t GetPreparedOffset() const;
    uint32_t GetPreparedSize() const;

    // TypeDefOrRef Interface

    virtual bool IsNested() const;
};

}

#endif//NINETJER_TYPEDEF_HPP
