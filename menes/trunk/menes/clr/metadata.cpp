/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "clr/ecma.hpp"
#include "clr/metadata.hpp"
#include "clr/utils.hpp"

#include "clr/tokens/assembly.hpp"
#include "clr/tokens/method.hpp"
#include "clr/tokens/module.hpp"

#include "ext/uuid.hpp"
#include "ios/getput.hpp"

namespace clr {

class Method;

Metadata::Metadata(const cse::String &path) :
    pefile_(path)
{
    OPT_TIMER("clr::Module::Parse_()")

    clihdr_ = reinterpret_cast<const CliFormat::CliHeader *>(pefile_[14].Begin());
    _assert(clihdr_ != NULL);

    EcmaAssert("Partition II, 24.3.3", clihdr_->Cb == sizeof(CliFormat::CliHeader));
    EcmaAssert("Partition II, 24.3.3", clihdr_->CodeManagerTable == 0);
    EcmaAssert("Partition II, 24.3.3", clihdr_->ExportAddressTableJumps == 0);
    EcmaAssert("Partition II, 24.3.3", clihdr_->ManagedNativeHeader == 0);

    EcmaAssert("Partition II, 24.3.3.1", (clihdr_->Flags & CliFormat::ILOnly) != 0)
    EcmaAssert("Partition II, 24.3.3.1", (clihdr_->Flags & CliFormat::TrackDebugData) == 0)

    const char *base(pefile_.FindRva(clihdr_->Metadata));
    const char *meta(base);

    uint32_t Signature(ios::GetLittle<uint32_t>(meta));
    uint16_t MajorVersion(ios::GetLittle<uint16_t>(meta));
    uint16_t MinorVersion(ios::GetLittle<uint16_t>(meta));
    uint32_t Reserved(ios::GetLittle<uint32_t>(meta));

    EcmaAssert("Partition II, 23.2.1", Signature == 0x424a5342);
    EcmaAssert("Partition II, 23.2.1", MajorVersion == 1);
    EcmaAssert("Partition II, 23.2.1", MinorVersion == 0);
    EcmaAssert("Partition II, 23.2.1", Reserved == 0);

    uint32_t Length(ios::GetLittle<uint32_t>(meta));
    cse::String Version(meta, Length, ext::ReadOnlyExternal);
    meta += Length;
    // XXX: standards compliance on null padding
    meta += (4 - Length) % 4;

    uint16_t Flags(ios::GetLittle<uint16_t>(meta));
    EcmaAssert("Partition II, 23.2.1", Flags == 0);

    uint16_t Streams(ios::GetLittle<uint16_t>(meta));
    for (uint16_t i(0); i < Streams; ++i) {
        uint32_t Offset(ios::GetLittle<uint32_t>(meta));
        uint32_t Size(ios::GetLittle<uint32_t>(meta));

        ext::Buffer Name;
        Name.Reserve(8);
        Name.Assign(meta);

        meta += Name.GetSize() + 1 + (3 - Name.GetSize()) % 4;
        //std::cout << Name << "(" << std::setbase(16) << Size << ")" << std::endl;

        const char *heap(base + Offset);
        if (Name == _B("#Blob"))
            blobheap_ = heap;
        else if (Name == _B("#GUID"))
            guidheap_ = heap;
        else if (Name == _B("#Strings"))
            stringsheap_ = heap;
        else if (Name == _B("#US"))
            usheap_ = heap;

        bool unique(streams_.Insert(Name, ext::SharedByteBlock(heap, Size, ext::ReadOnly)).Second());
        EcmaAssert("Partition II, 23.2.2", unique);
    }

    const CliFormat::Squiggly *squiggly(reinterpret_cast<const CliFormat::Squiggly *>(GetStream(_B("#~")).Begin()));
    EcmaAssert("Partition II, 23.2.6", squiggly->Reserved0 == 0);
    EcmaAssert("Partition II, 23.2.6", squiggly->MajorVersion == 1);
    EcmaAssert("Partition II, 23.2.6", squiggly->MinorVersion == 0);
    EcmaAssert("Partition II, 23.2.6", squiggly->Reserved1 == 1);

    //std::cout << std::setbase(10) << static_cast<int>(squiggly->HeapSizes) << std::endl;

    const char *tilde(reinterpret_cast<const char *>(squiggly + 1));

    tables_.Reserve(64);

    for (uint64_t i(1), table(0x00); i != 0; i <<= 1, ++table) {
        if ((squiggly->Valid & i) != 0) {
            EcmaAssert("Partition II, 23.2.6", table <= Tables::maxtable && Tables::tabledefs[table] != NULL);
            tables_.SetSize(table);
            uint32_t rows(ios::GetLittle<uint32_t>(tilde));
            tables_.InsertLast(Tables::TableInfo(rows));
        }
    }

    cache_.SetSize(tables_.GetSize());

    _assert(tables_.GetSize() <= 64);
    tables_.SetSize(64);

    for (uint32_t i(0); i != tables_.GetSize(); ++i) {
        Tables::TableInfo &table(tables_[i]);
        if (table.rows == 0)
            continue;

        cache_[i].SetSize(table.rows, _null<Token>());

        _assert(i <= Tables::maxtable);

        Tables::TableDef &def(*Tables::tabledefs[i]);
        table.cols = new uint32_t[def.width];

        for (uint32_t i(0); i != def.width; ++i) {
            Tables::ColumnDef &col(def.cols[i]);
            uint32_t width(col.type->Size(col.sarg, squiggly->HeapSizes, tables_));
            table.width += width;
            table.cols[i] = width;
        }

        table.base = tilde;
        tilde += table.rows * table.width;
    }

    GetToken<Module>(0x00000001)->SetEntryPoint(GetToken<Method>(clihdr_->EntryPointToken));
}

Metadata::~Metadata() {
    // XXX: this is undesired behavior
    _foreach (CacheList_, cache, cache_)
        _foreach (TokenList_, token, *cache)
            delete *token;
}

const PeFile &Metadata::GetFile() const {
    return pefile_;
}

ext::SharedByteBlock Metadata::GetStream(const cse::String &name) const {
    return streams_.GetOr(name, ext::SharedByteBlock());
}

bool Metadata::GetRow(uint32_t token, uint32_t cols[]) const {
    OPT_TIMER("clr::Module::GetRow()")

    uint32_t row(TokenRow(token));
    if (row == 0) {
        //_assert(false);
        return false;
    }

    uint32_t table(TokenTable(token));
    if (table >= tables_.GetSize()) {
        //_assert(false);
        return false;
    }

    const Tables::TableInfo &info(tables_[table]);
    if (row > info.rows) {
        //_assert(false);
        return false;
    }

    const char *data(info.base + (row - 1) * info.width);

    Tables::TableDef &def(*Tables::tabledefs[table]);
    for (size_t i(0); i < def.width; ++i) {
        uint32_t value(0);
        uint32_t width(info.cols[i]);

        switch (width) {
            case 1: value = ios::GetLittle<uint8_t>(data);  break;
            case 2: value = ios::GetLittle<uint16_t>(data); break;
            case 4: value = ios::GetLittle<uint32_t>(data); break;
            default: _assume(false);
        }

        Tables::ColumnDef &col(def.cols[i]);
        cols[i] = col.type->Decode(col.sarg, value);
    }

    return true;
}

uint32_t Metadata::GetRows(uint32_t table) const {
    return table < tables_.GetSize() ? tables_[table].rows : 0;
}

ext::SharedByteBlock Metadata::GetBlob(uint32_t index) const {
    OPT_TIMER("clr::Module::GetBlob()")

    ext::SharedByteBlock blob;
    const char *heap(blobheap_ + index);
    uint32_t size(Uncompress<uint32_t>(heap));
    blob.Assign(heap, size, ext::ReadOnlyExternal);
    return blob;
}

ext::Uuid Metadata::GetUuid(uint32_t index) const {
    // XXX: this isn't endian-safe
    return *(reinterpret_cast<const ext::Uuid *>(guidheap_) + (index - 1));
}

cse::String Metadata::GetString(uint32_t index) const {
    OPT_TIMER("clr::Module::GetString()")
    //std::cout << static_cast<void *>(stringsheap_ + index) << std::endl;
    return cse::String(stringsheap_ + index, ext::ReadOnlyExternal);
}

cse::String Metadata::GetUserString(uint32_t index) const {
    OPT_TIMER("clr::Module::GetUserString()")

    const char *heap(usheap_ + index);
    uint32_t size(Uncompress<uint32_t>(heap));

    _assert((size % 2) == 1);
    //_assert(heap[size] == 0);

    cse::String string;
    for (size_t i(0); i != size - 1; i += 2)
        string.InsertLast(heap[i]);
    return string;
}

Assembly *Metadata::GetAssembly() {
    return GetToken<Assembly>(0x20000001);
}

Module *Metadata::GetModule() {
    return GetToken<Module>(0x00000001);
}

}
