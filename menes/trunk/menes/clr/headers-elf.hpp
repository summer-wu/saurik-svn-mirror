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

#ifndef MENES_ELF_HEADERS_HPP
#define MENES_ELF_HEADERS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/byteorder.hpp"

namespace elf {

typedef luint32_t Address;
typedef luint16_t Half;
typedef luint32_t Offset;
typedef lint32_t SignedWord;
typedef luint32_t UnsignedWord;

struct ElfHeader {
    luint8_t Identifier_[16];
    Half Type_;
    Half Machine_;
    Word Version_;
    Address Entry_;
    Offset ProgramHeaderOffset_;
    Offset SectionHeaderOffset_;
    UnsignedWord Flags_;
    Half ElfHeaderSize_;
    Half ProgramHeaderEntrySize_;
    Half ProgramHeaderNumber_;
    Half SectionHeaderEntrySize_;
    Half SectionHeaderNumber_;
    Half SectionHeaderStringIndex_;
};

struct ProgramHeader {
    UnsignedWord Type_;
    Offset Offset_;
    Address VirtualAddress_;
    Address PhysicalAddress_;
    UnsignedWord FileSize_;
    UnsignedWord MemorySize_;
    UnsignedWord Flags_;
    UnsignedWord Alignment_;
};

struct SectionHeader_ {
    UnsignedWord Name_;
    UnsignedWord Type_;
    UnsignedWord Flags_;
    Address Address_;
    Offset Offset_;
    UnsignedWord Size_;
    UnsignedWord Link_;
    UnsignedWord Info_;
    UnsignedWord AddressAlignment_;
    UnsignedWord EntrySize_;
};

}

#endif//MENES_ELF_HEADERS_HPP
