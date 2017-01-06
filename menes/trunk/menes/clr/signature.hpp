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

#ifndef NINETJER_SIGNATURE_HPP
#define NINETJER_SIGNATURE_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/flags.hpp"

#include "ext/refcount.hpp"
#include "ext/shared.hpp"
#include "ext/vector.hpp"

#include "ios/streams.hpp"

namespace clr {

class Element;
class Metadata;
class SignatureList;
class TypeDefOrRef;

typedef _L< _S<Element> > ElementList;
typedef _L<struct ValueSlot> ValueList;

class Signature :
    public ext::ReferenceCounted
{
  private:
    _R<Element> result_;

    Flags::CallingConvention::Values calling_;
    ElementList arguments_;

    enum HasThis_ {
        None,
        Implicit,
        Explicit
    } hasthis_;

  public:
    Signature();

    /*Signature(Flags::CallingConvention::Values calling, const Element &result);
    Signature(Flags::CallingConvention::Values calling, const Element &result, const SignatureList &list);*/

    bool operator ==(const Signature &signature) const;
    bool operator !=(const Signature &signature) const;

    void Parse(Metadata &metadata, ios::Reader &blob);
    void Parse(Metadata &metadata, const ext::SharedByteBlock &blob);

    Flags::CallingConvention::Values GetCalling() const;

    ElementList GetArguments(TypeDefOrRef *type = NULL) const;

    const Element &GetResult() const;
    Element &GetResult();
};

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Signature &signature);

}

#endif//NINETJER_SIGNATURE_HPP
