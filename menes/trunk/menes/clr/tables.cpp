/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "clr/tables.hpp"

#include "clr/utils.hpp"

namespace Tables {

uint32_t IColumn::Decode(uint32_t sarg, uint32_t value) {
    return value;
}

uint32_t IColumn::Encode(uint32_t sarg, uint32_t value) {
    return value;
}

namespace Coded {

/*0x00*/ uint32_t TypeDefOrRef[] = { 3,
    0x02, 0x01, 0x1b
};

/*0x01*/ uint32_t HasConstant[] = { 3,
    0x04, 0x08, 0x17
};

/*0x02*/ uint32_t HasCustomAttribute[] = { 19,
    0x06, 0x04, 0x01, 0x02, 0x08,
    0x09, 0x0a, 0x00, 0x0e, 0x17,
    0x14, 0x11, 0x1a, 0x1b, 0x20,
    0x23, 0x26, 0x27, 0x28
};

/*0x03*/ uint32_t HasFieldMarshal[] = { 2,
    0x04, 0x08
};

/*0x04*/ uint32_t HasDeclSecurity[] = { 3,
    0x02, 0x06, 0x20
};

/*0x05*/ uint32_t MemberRefParent[] = { 5,
    /*02*/
    0xff, 0x01, 0x1a, 0x06, 0x1b
};

/*0x06*/ uint32_t HasSemantics[] = { 2,
    0x14, 0x17
};

/*0x07*/ uint32_t MethodDefOrRef[] = { 2,
    0x06, 0x0a
};

/*0x08*/ uint32_t MemberForwarded[] = { 2,
    0x04, 0x06
};

/*0x09*/ uint32_t Implementation[] = { 3,
    0x26, 0x23, 0x27
};

/*0x0a*/ uint32_t CustomAttributeType[] = { 5,
    /*??*//*??*/            /*??*/
    0xff, 0xff, 0x06, 0x0a, 0xff
};

/*0x0b*/ uint32_t ResolutionScope[] = { 4,
    0x00, 0x1a, 0x23, 0x01
};

/*0x0c*/ uint32_t TypeOrMethodDef[] = { 2,
    0x02, 0x06
};

uint32_t *coded[] = {
    /*0x00*/ TypeDefOrRef,
    /*0x01*/ HasConstant,
    /*0x02*/ HasCustomAttribute,
    /*0x03*/ HasFieldMarshal,
    /*0x04*/ HasDeclSecurity,
    /*0x05*/ MemberRefParent,
    /*0x06*/ HasSemantics,
    /*0x07*/ MethodDefOrRef,
    /*0x08*/ MemberForwarded,
    /*0x09*/ Implementation,
    /*0x0a*/ CustomAttributeType,
    /*0x0b*/ ResolutionScope,
    /*0x0c*/ TypeOrMethodDef
};

}

namespace ColType {

class ConstantType : public IColumn {
  public:
    virtual unsigned Size(uint32_t sarg, uint32_t heaps, const ext::Vector<TableInfo> &tables) {
        return sarg;
    }
} Constant;

// 0x01=String, 0x02=GUID, 0x04=Blob
class HeapIndexType : public IColumn {
  public:
    virtual unsigned Size(uint32_t sarg, uint32_t heaps, const ext::Vector<TableInfo> &tables) {
        return (heaps & sarg) == 0 ? 2 : 4;
    }
} HeapIndex;

class TableIndexType : public IColumn {
  public:
    virtual unsigned Size(uint32_t sarg, uint32_t heaps, const ext::Vector<TableInfo> &tables) {
        return (tables[sarg].rows < (1 << 16)) ? 2 : 4;
    }

    virtual uint32_t Decode(uint32_t sarg, uint32_t value) {
        _assert(clr::TokenTable(value) == 0);
        return clr::MakeToken(sarg, value);
    }

    virtual uint32_t Encode(uint32_t sarg, uint32_t value) {
        _assert(false);
        return 0;
    }
} TableIndex;

const uint32_t bitlog2[] = { 0,
    0, 1, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5
};

class CodedIndexType : public IColumn {
  public:
    virtual unsigned Size(uint32_t sarg, uint32_t heaps, const ext::Vector<TableInfo> &tables) {
        uint32_t *codes(Coded::coded[sarg]);
        uint32_t max(1 << (16 - bitlog2[codes[0]]));
        for (int i(codes[0]); i > 0; --i)
            if (codes[i] != 0xff && tables[codes[i]].rows >= max)
                return 4;
        return 2;
    }

    virtual uint32_t Decode(uint32_t sarg, uint32_t value) {
        uint32_t *codes(Coded::coded[sarg]);
        unsigned bits(bitlog2[codes[0]]);
        uint32_t code(value & ~(0xffffffff << bits));
        return (codes[code + 1] << 24) | (value >> bits);
    }

    virtual uint32_t Encode(uint32_t sarg, uint32_t value) {
        _assert(false);
        return 0;
    }
} CodedIndex;

}

namespace Cols {

/*0x20*/ ColumnDef Assembly[] = {
    /*0*/ { "HashAlgId",            0x04, &ColType::Constant },
    /*1*/ { "MajorVersion",         0x02, &ColType::Constant },
    /*2*/ { "MinorVersion",         0x02, &ColType::Constant },
    /*3*/ { "BuildNumber",          0x02, &ColType::Constant },
    /*4*/ { "RevisionNumber",       0x02, &ColType::Constant },
    /*5*/ { "Flags",                0x04, &ColType::Constant },
    /*6*/ { "PublicKey",            0x04, &ColType::HeapIndex },
    /*7*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*8*/ { "Culture",              0x01, &ColType::HeapIndex },
    /*9*/ { NULL,                   0x00, NULL }
};

/*0x22*/ ColumnDef AssemblyOS[] = {
    /*0*/ { "OSPlatformID",         0x04, &ColType::Constant },
    /*1*/ { "OSMajorVersion",       0x04, &ColType::Constant },
    /*2*/ { "OSMinorVersion",       0x04, &ColType::Constant },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x21*/ ColumnDef AssemblyProcessor[] = {
    /*0*/ { "Processor",            0x04, &ColType::Constant },
    /*1*/ { NULL,                   0x00, NULL }
};

/*0x23*/ ColumnDef AssemblyRef[] = {
    /*0*/ { "MajorVersion",         0x02, &ColType::Constant },
    /*1*/ { "MinorVersion",         0x02, &ColType::Constant },
    /*2*/ { "BuildNumber",          0x02, &ColType::Constant },
    /*3*/ { "RevisionNumber",       0x02, &ColType::Constant },
    /*4*/ { "Flags",                0x04, &ColType::Constant },
    /*5*/ { "PublicKeyOrToken",     0x04, &ColType::HeapIndex },
    /*6*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*7*/ { "Culture",              0x01, &ColType::HeapIndex },
    /*8*/ { "HashValue",            0x04, &ColType::HeapIndex },
    /*9*/ { NULL,                   0x00, NULL }
};

/*0x25*/ ColumnDef AssemblyRefOS[] = {
    /*0*/ { "OSPlatformID",         0x04, &ColType::Constant },
    /*1*/ { "OSMajorVersion",       0x04, &ColType::Constant },
    /*2*/ { "OSMinorVersion",       0x04, &ColType::Constant },
    /*3*/ { "AssemblyRef",          0x23, &ColType::TableIndex },
    /*4*/ { NULL,                   0x00, NULL }
};

/*0x24*/ ColumnDef AssemblyRefProcessor[] = {
    /*0*/ { "Processor",            0x04, &ColType::Constant },
    /*1*/ { "AssemblyRef",          0x23, &ColType::TableIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x0f*/ ColumnDef ClassLayout[] = {
    /*0*/ { "PackingSize",          0x02, &ColType::Constant },
    /*1*/ { "ClassSize",            0x04, &ColType::Constant },
    /*2*/ { "Parent",               0x02, &ColType::TableIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x0b*/ ColumnDef Constant[] = {
    /*0*/ { "Type",                 0x02, &ColType::Constant },
    /*1*/ { "Parent",               0x01, &ColType::CodedIndex },
    /*2*/ { "Value",                0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x0c*/ ColumnDef CustomAttribute[] = {
    /*0*/ { "Parent",               0x02, &ColType::CodedIndex },
    /*1*/ { "Type",                 0x0a, &ColType::CodedIndex },
    /*2*/ { "Value",                0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x0e*/ ColumnDef DeclSecurity[] = {
    /*0*/ { "Action",               0x02, &ColType::Constant },
    /*1*/ { "Parent",               0x04, &ColType::CodedIndex },
    /*2*/ { "PermissionSet",        0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x14*/ ColumnDef Event[] = {
    /*0*/ { "EventFlags",           0x02, &ColType::Constant },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "EventType",            0x00, &ColType::CodedIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x12*/ ColumnDef EventMap[] = {
    /*0*/ { "Parent",               0x02, &ColType::TableIndex },
    /*1*/ { "EventList",            0x14, &ColType::TableIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x27*/ ColumnDef ExportedType[] = {
    /*0*/ { "Flags",                0x04, &ColType::Constant },
    /*1*/ { "TypeDefId",            0x04, &ColType::Constant },
    /*2*/ { "TypeName",             0x01, &ColType::HeapIndex },
    /*3*/ { "TypeNamespace",        0x01, &ColType::HeapIndex },
    /*4*/ { "Implementation",       0x09, &ColType::CodedIndex },
    /*5*/ { NULL,                   0x00, NULL }
};

/*0x04*/ ColumnDef Field[] = {
    /*0*/ { "Flags",                0x02, &ColType::Constant },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "Signature",            0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x10*/ ColumnDef FieldLayout[] = {
    /*0*/ { "Offset",               0x04, &ColType::Constant },
    /*1*/ { "Field",                0x04, &ColType::TableIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x0d*/ ColumnDef FieldMarshal[] = {
    /*0*/ { "Parent",               0x03, &ColType::CodedIndex },
    /*1*/ { "NativeType",           0x04, &ColType::HeapIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x1d*/ ColumnDef FieldRva[] = {
    /*0*/ { "Rva",                  0x04, &ColType::Constant },
    /*1*/ { "Field",                0x04, &ColType::TableIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x26*/ ColumnDef File[] = {
    /*0*/ { "Flags",                0x04, &ColType::Constant },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "HashValue",            0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x2a*/ ColumnDef GenericParam[] = {
    /*0*/ { "Number",               0x02, &ColType::Constant },
    /*1*/ { "Flags",                0x02, &ColType::Constant },
    /*2*/ { "Owner",                0x0c, &ColType::CodedIndex },
    /*3*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*4*/ { "Kind",                 0x00, &ColType::CodedIndex },
    /*5*/ { "DeprecatedConstraint", 0x00, &ColType::CodedIndex },
    /*4*/ { NULL,                   0x00, NULL }
};

/*0x2c*/ ColumnDef GenericParamConstraint[] = {
    /*0*/ { "Owner",                0x2a, &ColType::TableIndex },
    /*0*/ { "Constraint",           0x00, &ColType::CodedIndex },
    /*4*/ { NULL,                   0x00, NULL }
};

/*0x1c*/ ColumnDef ImplMap[] = {
    /*0*/ { "MappingFlags",         0x02, &ColType::Constant },
    /*1*/ { "MemberForwarded",      0x08, &ColType::CodedIndex },
    /*2*/ { "ImportName",           0x01, &ColType::HeapIndex },
    /*3*/ { "ImportScope",          0x1a, &ColType::TableIndex },
    /*4*/ { NULL,                   0x00, NULL }
};

/*0x09*/ ColumnDef InterfaceImpl[] = {
    /*0*/ { "Class",                0x02, &ColType::TableIndex },
    /*1*/ { "Interface",            0x00, &ColType::CodedIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x28*/ ColumnDef ManifestResource[] = {
    /*0*/ { "Offset",               0x04, &ColType::Constant },
    /*1*/ { "Flags",                0x04, &ColType::Constant },
    /*2*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*3*/ { "Implementation",       0x09, &ColType::CodedIndex },
    /*4*/ { NULL,                   0x00, NULL }
};

/*0x0a*/ ColumnDef MemberRef[] = {
    /*0*/ { "Class",                0x05, &ColType::CodedIndex },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "Signature",            0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x06*/ ColumnDef Method[] = {
    /*0*/ { "Rva",                  0x04, &ColType::Constant },
    /*1*/ { "ImplFlags",            0x02, &ColType::Constant },
    /*2*/ { "Flags",                0x02, &ColType::Constant },
    /*3*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*4*/ { "Signature",            0x04, &ColType::HeapIndex },
    /*5*/ { "ParamList",            0x08, &ColType::TableIndex },
    /*6*/ { NULL,                   0x00, NULL }
};

/*0x19*/ ColumnDef MethodImpl[] = {
    /*0*/ { "Class",                0x02, &ColType::TableIndex },
    /*1*/ { "MethodBody",           0x07, &ColType::CodedIndex },
    /*2*/ { "MethodDeclaration",    0x07, &ColType::CodedIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x18*/ ColumnDef MethodSemantics[] = {
    /*0*/ { "Semantics",            0x02, &ColType::Constant },
    /*1*/ { "Method",               0x06, &ColType::TableIndex },
    /*2*/ { "Association",          0x06, &ColType::CodedIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x2b*/ ColumnDef MethodSpec[] = {
    /*0*/ { "Method",               0x07, &ColType::CodedIndex },
    /*1*/ { "Signature",            0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x00*/ ColumnDef Module[] = {
    /*0*/ { "Generation",           0x02, &ColType::Constant },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "Mvid",                 0x02, &ColType::HeapIndex },
    /*3*/ { "EncId",                0x02, &ColType::HeapIndex },
    /*4*/ { "EncBaseId",            0x02, &ColType::HeapIndex },
    /*5*/ { NULL,                   0x00, NULL }
};

/*0x1a*/ ColumnDef ModuleRef[] = {
    /*0*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*1*/ { NULL,                   0x00, NULL }
};

/*0x29*/ ColumnDef NestedClass[] = {
    /*0*/ { "NestedClass",          0x02, &ColType::TableIndex },
    /*1*/ { "EnclosingClass",       0x02, &ColType::TableIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x08*/ ColumnDef Param[] = {
    /*0*/ { "Flags",                0x02, &ColType::Constant },
    /*1*/ { "Sequence",             0x02, &ColType::Constant },
    /*2*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x17*/ ColumnDef Property[] = {
    /*0*/ { "Flags",                0x02, &ColType::Constant },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "Type",                 0x04, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x15*/ ColumnDef PropertyMap[] = {
    /*0*/ { "Parent",               0x02, &ColType::TableIndex },
    /*1*/ { "PropertyList",         0x17, &ColType::TableIndex },
    /*2*/ { NULL,                   0x00, NULL }
};

/*0x11*/ ColumnDef StandAloneSig[] = {
    /*0*/ { "Signature",            0x04, &ColType::HeapIndex },
    /*1*/ { NULL,                   0x00, NULL }
};

/*0x02*/ ColumnDef TypeDef[] = {
    /*0*/ { "Flags",                0x04, &ColType::Constant },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "Namespace",            0x01, &ColType::HeapIndex },
    /*3*/ { "Extends",              0x00, &ColType::CodedIndex },
    /*4*/ { "FieldList",            0x04, &ColType::TableIndex },
    /*5*/ { "MethodList",           0x06, &ColType::TableIndex },
    /*6*/ { NULL,                   0x00, NULL }
};

/*0x01*/ ColumnDef TypeRef[] = {
    /*0*/ { "ResolutionScope",      0x0b, &ColType::CodedIndex },
    /*1*/ { "Name",                 0x01, &ColType::HeapIndex },
    /*2*/ { "Namespace",            0x01, &ColType::HeapIndex },
    /*3*/ { NULL,                   0x00, NULL }
};

/*0x1b*/ ColumnDef TypeSpec[] = {
    /*0*/ { "Signature",            0x04, &ColType::HeapIndex },
    /*1*/ { NULL,                   0x00, NULL }
};

}

namespace Defs {

TableDef Assembly = {
    0x20, "Assembly",
    9, Cols::Assembly
};

TableDef AssemblyOS = {
    0x22, "AssemblyOS",
    3, Cols::AssemblyOS
};

TableDef AssemblyProcessor = {
    0x21, "AssemblyProcessor",
    1, Cols::AssemblyProcessor
};

TableDef AssemblyRef = {
    0x23, "AssemblyRef",
    9, Cols::AssemblyRef
};

TableDef AssemblyRefOS = {
    0x25, "AssemblyRefOS",
    4, Cols::AssemblyRefOS
};

TableDef AssemblyRefProcessor = {
    0x24, "AssemblyRefProcessor",
    2, Cols::AssemblyRefProcessor
};

TableDef ClassLayout = {
    0x0f, "ClassLayout",
    3, Cols::ClassLayout
};

TableDef Constant = {
    0x0b, "Constant",
    3, Cols::Constant
};

TableDef CustomAttribute = {
    0x0c, "CustomAttribute",
    3, Cols::CustomAttribute
};

TableDef DeclSecurity = {
    0x0e, "DeclSecurity",
    3, Cols::DeclSecurity
};

TableDef Event = {
    0x14, "Event",
    3, Cols::Event
};

TableDef EventMap = {
    0x12, "EventMap",
    2, Cols::EventMap
};

TableDef ExportedType = {
    0x27, "ExportedType",
    5, Cols::ExportedType
};

TableDef Field = {
    0x04, "Field",
    3, Cols::Field
};

TableDef FieldLayout = {
    0x10, "FieldLayout",
    2, Cols::FieldLayout
};

TableDef FieldMarshal = {
    0x0d, "FieldMarshal",
    2, Cols::FieldMarshal
};

TableDef FieldRva = {
    0x1d, "FieldRva",
    2, Cols::FieldRva
};

TableDef File = {
    0x26, "File",
    3, Cols::File
};

TableDef GenericParam = {
    0x2a, "GenericParam",
    6, Cols::GenericParam
};

TableDef GenericParamConstraint = {
    0x2c, "GenericParamConstraint",
    2, Cols::GenericParamConstraint
};

TableDef ImplMap = {
    0x1c, "ImplMap",
    4, Cols::ImplMap
};

TableDef InterfaceImpl = {
    0x09, "InterfaceImpl",
    2, Cols::InterfaceImpl
};

TableDef ManifestResource = {
    0x28, "ManifestResource",
    4, Cols::ManifestResource
};

TableDef MemberRef = {
    0x0a, "MemberRef",
    3, Cols::MemberRef
};

TableDef Method = {
    0x06, "Method",
    6, Cols::Method
};

TableDef MethodImpl = {
    0x19, "MethodImpl",
    3, Cols::MethodImpl
};

TableDef MethodSemantics = {
    0x18, "MethodSemantics",
    3, Cols::MethodSemantics
};

TableDef MethodSpec = {
    0x2b, "MethodSpec",
    2, Cols::MethodSpec
};

TableDef Module = {
    0x00, "Module",
    5, Cols::Module
};

TableDef ModuleRef = {
    0x1a, "ModuleRef",
    1, Cols::ModuleRef
};

TableDef NestedClass = {
    0x29, "NestedClass",
    2, Cols::NestedClass
};

TableDef Param = {
    0x08, "Param",
    3, Cols::Param
};

TableDef Property = {
    0x17, "Property",
    3, Cols::Property
};

TableDef PropertyMap = {
    0x15, "PropertyMap",
    2, Cols::PropertyMap
};

TableDef StandAloneSig = {
    0x11, "StandAloneSig",
    1, Cols::StandAloneSig
};

TableDef TypeDef = {
    0x02, "TypeDef",
    6, Cols::TypeDef
};

TableDef TypeRef = {
    0x01, "TypeRef",
    3, Cols::TypeRef
};

TableDef TypeSpec = {
    0x1b, "TypeSpec",
    1, Cols::TypeSpec
};

}

IColumn &coder(ColType::CodedIndex);

const unsigned maxtable(0x2c);
TableDef * const tabledefs[maxtable + 1] = {
    /*0x00+*/ &Defs::Module,
    /*0x01+*/ &Defs::TypeRef,
    /*0x02+*/ &Defs::TypeDef,
    /*0x03+*/ NULL,
    /*0x04+*/ &Defs::Field,
    /*0x05+*/ NULL,
    /*0x06+*/ &Defs::Method,
    /*0x07+*/ NULL,
    /*0x08+*/ &Defs::Param,
    /*0x09+*/ &Defs::InterfaceImpl,
    /*0x0a+*/ &Defs::MemberRef,
    /*0x0b */ &Defs::Constant,
    /*0x0c+*/ &Defs::CustomAttribute,
    /*0x0d+*/ &Defs::FieldMarshal,
    /*0x0e+*/ &Defs::DeclSecurity,
    /*0x0f+*/ &Defs::ClassLayout,

    /*0x10+*/ &Defs::FieldLayout,
    /*0x11+*/ &Defs::StandAloneSig,
    /*0x12 */ &Defs::EventMap,
    /*0x13+*/ NULL,
    /*0x14 */ &Defs::Event,
    /*0x15+*/ &Defs::PropertyMap,
    /*0x16+*/ NULL,
    /*0x17+*/ &Defs::Property,
    /*0x18+*/ &Defs::MethodSemantics,
    /*0x19+*/ &Defs::MethodImpl,
    /*0x1a+*/ &Defs::ModuleRef,
    /*0x1b+*/ &Defs::TypeSpec,
    /*0x1c+*/ &Defs::ImplMap,
    /*0x1d+*/ &Defs::FieldRva,
    /*0x1e+*/ NULL,
    /*0x1f+*/ NULL,

    /*0x20+*/ &Defs::Assembly,
    /*0x21-*/ &Defs::AssemblyProcessor,
    /*0x22-*/ &Defs::AssemblyOS,
    /*0x23+*/ &Defs::AssemblyRef,
    /*0x24-*/ &Defs::AssemblyRefProcessor,
    /*0x25-*/ &Defs::AssemblyRefOS,
    /*0x26+*/ &Defs::File,
    /*0x27+*/ &Defs::ExportedType,
    /*0x28+*/ &Defs::ManifestResource,
    /*0x29+*/ &Defs::NestedClass,
    /*0x2a+*/ &Defs::GenericParam,
    /*0x2b+*/ &Defs::MethodSpec,
    /*0x2c+*/ &Defs::GenericParamConstraint
};

}
