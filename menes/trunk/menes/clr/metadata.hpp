/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_CLR_METADATA_HPP
#define MENES_CLR_METADATA_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/element.hpp"
#include "clr/headers-clr.hpp"
#include "clr/pefile.hpp"
#include "clr/tables.hpp"

namespace clr {

class Assembly;
class Module;
class Token;

class Metadata {
  private:
    typedef ext::RedBlackMap<cse::String, ext::SharedByteBlock> StreamMap_;
    StreamMap_ streams_;

    typedef ext::Vector<Tables::TableInfo> tableinfo_;
    tableinfo_ tables_;

    typedef ext::Vector<Token *> TokenList_;
    typedef ext::Vector<TokenList_> CacheList_;
    CacheList_ cache_;

    PeFile pefile_;

    const CliFormat::CliHeader *clihdr_;

    const char *blobheap_;
    const char *guidheap_;
    const char *stringsheap_;
    const char *usheap_;

  private:
    Token *GetToken_(uint32_t token);

  public:
    explicit Metadata(const cse::String &path);
    ~Metadata();

    const PeFile &GetFile() const;

    ext::SharedByteBlock GetStream(const cse::String &name) const;
    bool GetRow(uint32_t token, uint32_t cols[]) const;
    uint32_t GetRows(uint32_t table) const;

    template <typename Token_>
    Token_ *GetToken(uint32_t token) {
        return dynamic_cast<Token_ *>(GetToken<Token>(token));
    }

    ext::SharedByteBlock GetBlob(uint32_t index) const;
    ext::Uuid GetUuid(uint32_t index) const;
    cse::String GetString(uint32_t index) const;
    cse::String GetUserString(uint32_t index) const;

    Assembly *GetAssembly();
    Module *GetModule();
};

template <>
inline Token *Metadata::GetToken(uint32_t token) {
    return GetToken_(token);
}

}

#endif//MENES_CLR_METADATA_HPP
