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

#ifndef NINETJER_ELEMENT_HPP
#define NINETJER_ELEMENT_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/flags.hpp"

#include "ext/refcount.hpp"
#include "ext/vector.hpp"

#include "ios/streams.hpp"

namespace clr {

class Element;
class Metadata;
class Signature;
class TypeDefOrRef;

typedef _L< _S<Element> > ElementList;

class Element :
    public ext::ReferenceCounted
{
  public:
    struct Dimension {
        uint32_t size;
        uint32_t lower;

        Dimension() :
            size(0),
            lower(0)
        {
        }
    };

    typedef _L<TypeDefOrRef *> CustomList;
    typedef _L<Dimension> DimList;

    enum Intrinsic {
        End         = 0x00,

        Void        = 0x01,
        Boolean     = 0x02,
        Char        = 0x03,
        I1          = 0x04,
        U1          = 0x05,
        I2          = 0x06,
        U2          = 0x07,
        I4          = 0x08,
        U4          = 0x09,
        I8          = 0x0a,
        U8          = 0x0b,
        R4          = 0x0c,
        R8          = 0x0d,
        String      = 0x0e,

        Ptr         = 0x0f,
        ByRef       = 0x10,

        ValueType   = 0x11,
        Class       = 0x12,

        Parameter   = 0x13,
        Array       = 0x14,
        Specialize  = 0x15,
        TypedByRef  = 0x16,

        I           = 0x18,
        U           = 0x19,

        FnPtr       = 0x1b,
        Object      = 0x1c,
        SZArray     = 0x1d,

        FuckIfIKnow = 0x1e,

        CModReqd    = 0x1f,
        CModOpt     = 0x20,
        Internal    = 0x21,
        Max         = 0x22,

        Modifier    = 0x40,
        Sentinal    = 0x01 | Modifier,
        Pinned      = 0x05 | Modifier
    };

  private:
    Intrinsic intrinsic_;

    TypeDefOrRef *type_;
    _R<Signature> nested_;

    _L< _S<Element> > extras_;

    uint32_t index_;

    DimList dims_;

    CustomList custom_;

  public:
    Element();

    explicit Element(Intrinsic intrinsic);
    explicit Element(Intrinsic intrinsic, const Element &elem);
    explicit Element(Intrinsic intrinsic, TypeDefOrRef *type);

    Element(const Signature &fnptr);

    bool operator ==(const Element &rhs) const;
    bool operator !=(const Element &rhs) const;

    void Parse(Metadata &metadata, ios::Reader &blob);

    bool IsVoid() const;

    Intrinsic GetIntrinsic() const;

    const TypeDefOrRef *GetType() const;
    TypeDefOrRef *GetType();

    const Signature *GetNested() const;
    Signature *GetNested();

    const Element *GetExtra() const;
    const ElementList &GetExtras() const;

    uint32_t GetIndex() const;

    uint32_t GetSize() const;
};

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Element &element);

}

#endif//NINETJER_ELEMENT_HPP
