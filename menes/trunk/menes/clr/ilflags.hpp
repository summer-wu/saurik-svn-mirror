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

#ifndef NINETJER_ILFLAGS_HPP
#define NINETJER_ILFLAGS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/byteorder.hpp"

namespace clr {

namespace ILHeader {

enum Flags {
    FatFormat  = 0x03,
    TinyFormat = 0x02,
    MoreSects  = 0x08,
    InitLocals = 0x10
};

struct Tiny {
    uint8_t Flags : 2;
    uint8_t Size : 6;
};

struct Fat {
    uint32_t Flags : 12;
    uint32_t Size : 4;
    uint32_t MaxStack : 16;
    luint32_t CodeSize;
    luint32_t LocalVarSigTok;
};

}

namespace ILSection {

enum ILSectionFlags {
    EHTable    = 0x01,
    OptILTable = 0x02,
    FatFormat  = 0x40,
    MoreSects  = 0x80
};

struct SmallEHHeader {
    uint8_t Kind;
    uint8_t DataSize;
    luint16_t Reserved;
};

struct FatEHHeader {
    unsigned Kind : 8;
    unsigned DataSize : 24;
};

enum EHClauseFlags {
    Exception = 0x0000,
    Filter    = 0x0001,
    Finally   = 0x0002,
    Fault     = 0x0004
};

struct SmallEHClause {
    ext::Ordered<EHClauseFlags, ext::LittleEndianOrder, uint16_t> Flags;
    luint16_t TryOffset;
    luint8_t TryLength;
    luint16_t HandlerOffset;
    luint8_t HandlerLength;
    luint32_t ClassToken;
    luint32_t FilterOffset;
};

struct FatEHClause {
    ext::Ordered<EHClauseFlags, ext::LittleEndianOrder, uint32_t> Flags;
    luint32_t TryOffset;
    luint32_t TryLength;
    luint32_t HandlerOffset;
    luint32_t HandlerLength;
    luint32_t ClassToken;
    luint32_t FilterOffset;
};

}

}

#endif//NINETJER_ILFLAGS_HPP
