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

#ifndef NINETJER_ASSEMBLYREF_HPP
#define NINETJER_ASSEMBLYREF_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/string.hpp"

#include "ext/shared.hpp"

#include "clr/flags.hpp"
#include "clr/codeds/hascustomattribute.hpp"
#include "clr/codeds/implementation.hpp"
#include "clr/codeds/resolutionscope.hpp"
#include "clr/token.hpp"

namespace clr {

class Assembly;

/*0x23*/ class AssemblyRef :
    public virtual Token,
    public HasCustomAttribute,
    public Implementation,
    public ResolutionScope
{
  private:
    uint16_t major_;
    uint16_t minor_;
    uint16_t build_;
    uint16_t revision_;
    Flags::AssemblyFlags::Values flags_;
    ext::SharedByteBlock publickey_;
    cse::String name_;
    cse::String culture_;
    ext::SharedByteBlock hash_;

    mutable Assembly *cache_;

    Assembly *ResolveAssembly_() const;
    virtual HasTypeDef *ResolveHasTypeDef_() const;

  protected:
    virtual void Parse(Metadata &metadata, uint32_t token);

  public:
    explicit AssemblyRef();

    uint16_t GetMajorVersion() const;
    uint16_t GetMinorVersion() const;
    uint16_t GetBuildNumber() const;
    uint16_t GetRevisionNumber() const;
    Flags::AssemblyFlags::Values GetFlags() const;
    ext::SharedByteBlock GetPublicKeyOrToken() const;
    const cse::String &GetName() const;
    const cse::String &GetCulture() const;
    ext::SharedByteBlock GetHash() const;
};

}

#endif//NINETJER_ASSEMBLYREF_HPP
