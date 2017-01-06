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

#ifndef NINETJER_MODULE_HPP
#define NINETJER_MODULE_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/hastypedef.hpp"
#include "clr/token.hpp"

#include "clr/codeds/hascustomattribute.hpp"
#include "clr/codeds/resolutionscope.hpp"

#include "ext/uuid.hpp"

namespace clr {

class Method;

/*0x00*/ class Module :
    public virtual Token,

    public HasCustomAttribute,
    public ResolutionScope,

    public HasTypeDef
{
    friend class Assembly;

  public:
    typedef etl::Pair<cse::String, cse::String> NamePair;
    typedef ext::RedBlackMap<NamePair, TypeDef *> TypeMap;

  private:
    TypeMap typedefs_;

    bool preload_;

    uint16_t generation_;
    cse::String name_;
    ext::Uuid mvid_;
    ext::Uuid encid_;
    ext::Uuid encbaseid_;

    Assembly *assembly_;
    Method *entry_;

    virtual TypeDef *FindTypeDef_(const cse::String &space, const cse::String &name) const;
    virtual HasTypeDef *ResolveHasTypeDef_() const;

    virtual void Parse(Metadata &metadata, uint32_t token);
    void Parse_();

  public:
    // Module Table

    uint16_t GetGeneration() const;
    void SetGeneration(uint16_t generation);

    const cse::String &GetName() const;
    void SetName(const cse::String &name);

    ext::Uuid GetMvid() const;
    void SetMvid(const ext::Uuid &mvid);

    ext::Uuid GetEncId() const;
    void SetEncId(const ext::Uuid &encid);

    ext::Uuid GetEncBaseId() const;
    void SetEncBaseId(const ext::Uuid &encbaseid);

    // Module Object

    const Assembly *GetAssembly() const;
    Assembly *GetAssembly();

    const Method *GetEntryPoint() const;
    Method *GetEntryPoint();

    void SetEntryPoint(Method *method);

    void PrepareModule();

    const TypeMap &GetTypes() const;
};

}

#endif//NINETJER_MODULE_HPP
