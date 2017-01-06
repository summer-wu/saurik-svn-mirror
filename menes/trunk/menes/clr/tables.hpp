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

#ifndef NINETJER_TABLES_HPP
#define NINETJER_TABLES_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"

namespace Tables {

enum Index {
    Module               = 0x00,
    TypeRef              = 0x01,
    TypeDef              = 0x02,
    // Unused            = 0x03,
    Field                = 0x04,
    // Unused            = 0x05,
    Method               = 0x06,
    // Unused            = 0x07,
    Param                = 0x08,
    InterfaceImpl        = 0x09,
    MemberRef            = 0x0a,
    Constant             = 0x0b,
    CustomAttribute      = 0x0c,
    FieldMarshal         = 0x0d,
    DeclSecurity         = 0x0e,
    ClassLayout          = 0x0f,

    FieldLayout          = 0x10,
    StandAloneSig        = 0x11,
    EventMap             = 0x12,
    // Unused            = 0x13,
    Event                = 0x14,
    PropertyMap          = 0x15,
    // Unused            = 0x16,
    Property             = 0x17,
    MethodSemantics      = 0x18,
    MethodImpl           = 0x19,
    ModuleRef            = 0x1a,
    TypeSpec             = 0x1b,
    ImplMap              = 0x1c,
    FieldRva             = 0x1d,
    // Unused            = 0x1e,
    // Unused            = 0x1f,

    Assembly             = 0x20,
    AssemblyProcessor    = 0x21,
    AssemblyOS           = 0x22,
    AssemblyRef          = 0x23,
    AssemblyRefProcessor = 0x24,
    AssemblyRefOS        = 0x25,
    File                 = 0x26,
    ExportedType         = 0x27,
    ManifestResource     = 0x28,
    NestedClass          = 0x29
};

struct TableInfo {
    //TableDef *def;
    const char *base;
    uint32_t rows;
    uint32_t width;
    uint32_t *cols;

    TableInfo() :
        base(NULL),
        rows(0),
        width(0),
        cols(NULL)
    {
    }

    TableInfo(uint32_t rows) :
        base(NULL),
        rows(rows),
        width(0),
        cols(NULL)
    {
    }

    ~TableInfo() {
        delete [] cols;
    }
};

class IColumn {
  public:
    virtual unsigned Size(uint32_t sarg, uint32_t heaps, const ext::Vector<TableInfo> &tables) = 0;
    virtual uint32_t Decode(uint32_t sarg, uint32_t value);
    virtual uint32_t Encode(uint32_t sarg, uint32_t value);
};

struct ColumnDef {
    const char *name;
    uint32_t sarg;
    IColumn *type;
};

struct TableDef {
    uint32_t index;
    const char *name;
    uint32_t width;
    ColumnDef *cols;
};

extern IColumn &coder;

extern const unsigned maxtable;
extern TableDef * const tabledefs[];

}

#endif//NINETJER_TABLES_HPP
