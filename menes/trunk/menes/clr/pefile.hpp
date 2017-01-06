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

#ifndef NINETJER_PEFILE_HPP
#define NINETJER_PEFILE_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/memorymap.hpp"
#include "clr/headers-clr.hpp"
#include "clr/headers-pe.hpp"

#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/shared.hpp"
#include "cse/string.hpp"

namespace clr {

class PeFile {
  private:
    struct Section_ {
        ext::SharedByteBlock data_;
        uint32_t rva_;
    };

    api::MemoryMap file_;

    typedef ext::RedBlackMap<cse::String, Section_> Sections_;
    Sections_ sections_;

    exe::FileHeader *filehdr_;
    exe::OptionalHeader *opthdr_;

  public:
    PeFile(const cse::String &path);

    const char *FindRva(uint32_t rva) const;
    const char *FindVA(uint32_t va) const;

    ext::SharedByteBlock operator [](const cse::String &name) const;
    ext::SharedByteBlock operator [](unsigned index) const;
};

}

#endif//NINETJER_PEFILE_HPP
