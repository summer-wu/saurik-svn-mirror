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

#ifndef MENES_EXE_HEADERS_HPP
#define MENES_EXE_HEADERS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/byteorder.hpp"

namespace exe {

enum MachineType {
    UnknownMachine = 0x0000,
    i386           = 0x014c,
    i486           = 0x014d,
    Pentium        = 0x014e,
    MIPS_MarkI     = 0x0162,
    MIPS_MarkII    = 0x0163,
    MIPS_MarkIII   = 0x0166,
    MIPS_R10000    = 0x0168,
    MIPS_WCEv2     = 0x0169,
    Alpha          = 0x0184,
    PowerPC        = 0x01f0,
    SH3            = 0x01a2,
    SH3E           = 0x01a4,
    SH4            = 0x01a6,
    ARM            = 0x01c0,
    Thumb          = 0x01c2,
    IA64           = 0x0200,
    MIPS_16        = 0x0266,
    MIPS_FPU       = 0x0366,
    MIPS_FPU16     = 0x0466,
    Alpha64        = 0x0284
    /* AXP64       = Alpha64 */
};

enum FileCharacteristics {
    /* Program           = 0x0000, */
    RelocsStripped       = 0x0001,
    ExecutableImage      = 0x0002,
    LineNumsStripped     = 0x0004,
    LocalSymsStripped    = 0x0008,
    AggressiveWSTrim     = 0x0010,
    LargeAddressAware    = 0x0020,
    BytesReversedLo      = 0x0080,
    Machine32Bit         = 0x0100,
    DebugStripped        = 0x0200,
    RemovableRunFromSwap = 0x0400,
    NetRunFromSwap       = 0x0800,
    System               = 0x1000,
    DLL                  = 0x2000,
    UpSystemOnly         = 0x4000,
    BytesReversedHi      = 0x8000
};

enum Subsystem {
    UnknownSubsystem = 0,
    Native           = 1,
    Windows_GUI      = 2,
    Windows_CUI      = 3,
    OS2_CUI          = 5,
    POSIX_CUI        = 7,
    NativeWindows    = 8,
    WindowsCE_GUI    = 9
};

enum DLLFlags {
    PerProcessInit = 0x0001,
    PerProcessTerm = 0x0002,
    PerThreadInit  = 0x0004,
    PerThreadTerm  = 0x0008
};

enum ObjectFlags {
    Code        = 0x00000020,
    InitData    = 0x00000040,
    UninitData  = 0x00000080,
    NoCache     = 0x04000000,
    NotPagable  = 0x08000000,
    IsShared    = 0x10000000,
    Executable  = 0x20000000,
    Readable    = 0x40000000,
    Writable    = 0x80000000
};

struct DosHeader {
    luint16_t e_magic;
    luint16_t e_cblp;
    luint16_t e_cp;
    luint16_t e_crlc;
    luint16_t e_cparhdr;
    luint16_t e_minalloc;
    luint16_t e_maxalloc;
    luint16_t e_ss;
    luint16_t e_sp;
    luint16_t e_csum;
    luint16_t e_ip;
    luint16_t e_cs;
    luint16_t e_lfarlc;
    luint16_t e_ovno;
    luint16_t e_res[4];
    luint16_t e_oemid;
    luint16_t e_oeminfo;
    luint16_t e_res2[10];
    luint32_t e_lfanew;
};

struct FileHeader {
    luint32_t SignatureStamp;
    ext::Ordered<ext::LittleEndianOrder, MachineType, uint16_t> Machine;
    luint16_t NumberOfSections;
    luint32_t TimeDateStamp;
    luint32_t PointerToSymbolTable;
    luint32_t NumberOfSymbols;
    luint16_t SizeOfOptionalHeader;
    ext::Ordered<ext::LittleEndianOrder, FileCharacteristics, uint16_t> Characteristics;
};

struct DataDirectory {
    luint32_t VirtualAddress;
    luint32_t Size;

    enum Entries {
        Export        =  0,
        Import        =  1,
        Resource      =  2,
        Exception     =  3,
        Security      =  4,
        BaseReloc     =  5,
        Debug         =  6,
        //Copyright   =  7,
        Architecture  =  7,
        GlobalPtr     =  8,
        TLS           =  9,
        LoadConfig    = 10,
        BoundImport   = 11,
        IAT           = 12,
        DelayImport   = 13,
        COMDescriptor = 14,
        Reserved      = 15
    };
};

struct OptionalHeader {
    // Standard Info
    luint16_t Magic;
    luint8_t MajorLinkerVersion;
    luint8_t MinorLinkerVersion;
    luint32_t SizeOfCode;
    luint32_t SizeOfInitializedData;
    luint32_t SizeOfUninitializedData;
    luint32_t AddressOfEntryPoint;
    luint32_t BaseOfCode;
    luint32_t BaseOfData;

    // NT Specific
    luint32_t ImageBase;
    luint32_t SectionAlignment;
    luint32_t FileAlignment;

    luint16_t MajorOperatingSystemVersion;
    luint16_t MinorOperatingSystemVersion;
    luint16_t MajorImageVersion;
    luint16_t MinorImageVersion;
    luint16_t MajorSubsystemVersion;
    luint16_t MinorSubsystemVersion;

    luint32_t Win32VersionValue;
    luint32_t SizeOfImage;
    luint32_t SizeOfHeaders;
    luint32_t CheckSum;

    luint16_t Subsystem;
    luint16_t DllCharacteristics;

    luint32_t SizeOfStackReserve;
    luint32_t SizeOfStackCommit;
    luint32_t SizeOfHeapReserve;
    luint32_t SizeOfHeapCommit;
    luint32_t LoaderFlags;
    luint32_t NumberOfRvaAndSizes;

    DataDirectory DataDirectories[];
};

struct SectionHeader {
    char Name[8];
    union {
        luint32_t PhysicalAddress;
        luint32_t VirtualSize;
    };
    luint32_t VirtualAddress;
    luint32_t SizeOfRawData;
    luint32_t PointerToRawData;
    luint32_t PointerToRelocations;
    luint32_t PointerToLinenumbers;
    luint16_t NumberOfRelocations;
    luint16_t NumberOfLinenumbers;
    luint32_t Characteristics;
};

struct ImportDescriptor {
    union {
        luint32_t Characteristics;
        luint32_t OriginalFirstThunk;
    };

    luint32_t TimeDateStamp;
    luint32_t ForwarderChain;
    luint32_t Name;
    luint32_t FirstThunk;
};

struct ImportLookupTable {
};

struct ImportByName {
    luint16_t Hint;
    char Name[];
};

}

#endif//MENES_EXE_HEADERS_HPP
