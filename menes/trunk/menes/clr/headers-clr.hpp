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

#ifndef MENES_CLR_HEADERS_HPP
#define MENES_CLR_HEADERS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/byteorder.hpp"

extern const char DosStub1[];
extern const char DosStub2[];
extern const unsigned DosStub1Size;
extern const unsigned DosStub2Size;

namespace CliFormat {

enum CliFlags {
    ILOnly           = 0x00000001,
    Required32Bit    = 0x00000002,
    StrongNameSigned = 0x00000008,
    TrackDebugData   = 0x00010000
};

struct CliHeader {
    luint32_t Cb;
    luint16_t MajorRuntimeVersion;
    luint16_t MinorRuntimeVersion;
    luint64_t Metadata;
    ext::Ordered<ext::LittleEndianOrder, CliFlags, uint32_t> Flags;
    luint32_t EntryPointToken;
    luint64_t Resources;
    luint64_t StrongNameSignature;
    luint64_t CodeManagerTable;
    luint64_t VTableFixups;
    luint64_t ExportAddressTableJumps;
    luint64_t ManagedNativeHeader;
};

struct Squiggly {
    luint32_t Reserved0;
    luint8_t MajorVersion;
    luint8_t MinorVersion;
    luint8_t HeapSizes;
    luint8_t Reserved1;
    luint64_t Valid;
    luint64_t Sorted;
};

}

#endif//MENES_CLR_HEADERS_HPP
