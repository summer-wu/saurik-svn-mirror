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

#ifndef NINETJER_FLAGS_HPP
#define NINETJER_FLAGS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "cse/string.hpp"
#include "ext/variant.hpp"

namespace clr {

typedef ext::ListMap<cse::String, ext::Variant> FlagMap;

namespace Flags {

namespace AssemblyHashAlgorithm {
enum Values {
    None     = 0x0000,
    Reserved = 0x8003,

    MD5      = 0x8003,
    SHA1     = 0x8004
}; }

namespace AssemblyFlags {
enum Values {
    PublicKey                  = 0x0001,
    SideBySideCompatible       = 0x0000,
    Reserved                   = 0x0030,
    EnableJitCompileTracking   = 0x8000,
    DisableJitCompileOptimizer = 0x4000
}; }

namespace EventAttributes {
enum Values {
    SpecialName   = 0x0200,
    RTSpecialName = 0x0400
}; }

namespace FieldAttributes {
enum Values {
    FieldAccessMask    = 0x0007,
    Compilercontrolled = 0x0000,
    Private            = 0x0001,
    FamANDAssem        = 0x0002,
    Assembly           = 0x0003,
    Family             = 0x0004,
    FamORAssem         = 0x0005,
    Public             = 0x0006,

    Static             = 0x0010,
    InitOnly           = 0x0020,
    Literal            = 0x0040,
    NotSerialized      = 0x0080,
    SpecialName        = 0x0200,

    // InterOp
    PInvokeImpl        = 0x2000,

    // Additional Flags
    RTSpecialName      = 0x0400,
    HasFieldMarshal    = 0x1000,
    HasDefault         = 0x8000,
    HasFieldRva        = 0x0100
}; }

namespace FileAttributes {
enum Values {
    ContainsMetadata   = 0x0000,
    ContainsNoMetadata = 0x0001
}; }

namespace PInvokeAttributes {
enum Values {
    NoMangle          = 0x0001,

    // Character Set
    CharSetMask       = 0x0006,
    CharSetNotSpec    = 0x0000,
    CharSetAnsi       = 0x0002,
    CharSetUnicode    = 0x0004,
    CharSetAuto       = 0x0006,

    SupportsLastError = 0x0040,

    // Calling Convention
    CallConvMask      = 0x0700,
    CallConvWinapi    = 0x0100,
    CallConvCdecl     = 0x0200,
    CallConvStdcall   = 0x0300,
    CallConvThiscall  = 0x0400,
    CallConvFastcall  = 0x0500
}; }

namespace ManifestResourceAttributes {
enum Values {
    VisibilityMask = 0x0007,
    Public         = 0x0001,
    Private        = 0x0002
}; }

namespace MethodImplAttributes {
enum Values {
    CodeTypeMask     = 0x0003,
    IL               = 0x0000,
    Native           = 0x0001,
    OPTIL            = 0x0002,
    Runtime          = 0x0003,

    ManagedMask      = 0x0004,
    Unmanaged        = 0x0004,
    Managed          = 0x0000,

    // Implementation / InterOp
    ForwardRef       = 0x0010,
    PreserveSig      = 0x0080,
    InternalCall     = 0x1000,
    Synchronized     = 0x0020,
    NoInlining       = 0x0008,
    MaxMethodImplVal = 0xffff
}; }

namespace MethodSemanticsAttributes {
enum Values {
    Setter   = 0x0001,
    Getter   = 0x0002,
    Other    = 0x0004,
    AddOn    = 0x0008,
    RemoveOn = 0x0010,
    Fire     = 0x0020
}; }

namespace ParamAttributes {
enum Values {
    In              = 0x0001,
    Out             = 0x0002,
    Optional        = 0x0004,
    HasDefault      = 0x1000,
    HasFieldMarshal = 0x2000,
    Unused          = 0xcfe0
}; }

namespace PropertyAttributes {
enum Values {
    SpecialName   = 0x0200,
    RTSpecialName = 0x0400,
    HasDefault    = 0x1000,
    Unused        = 0xe9ff
}; }

namespace TypeAttributes {
enum Values {
    // Visibility
    VisibilityMask     = 0x00000007,
    NotPublic          = 0x00000000,
    Public             = 0x00000001,
    NestedPublic       = 0x00000002,
    NestedPrivate      = 0x00000003,
    NestedFamily       = 0x00000004,
    NestedAssembly     = 0x00000005,
    NestedFamANDAssem  = 0x00000006,
    NestedFamORAssem   = 0x00000007,

    // Class Layout
    LayoutMask         = 0x00000018,
    AutoLayout         = 0x00000000,
    SequentialLayout   = 0x00000008,
    ExplicitLayout     = 0x00000010,

    // Class Semantics
    ClassSemanticsMask = 0x00000020,
    Class              = 0x00000000,
    Interface          = 0x00000020,

    // Special Semantics
    Abstract           = 0x00000080,
    Sealed             = 0x00000100,
    SpecialName        = 0x00000400,

    // Implementation
    Import             = 0x00001000,
    Serializable       = 0x00002000,

    // String Formatting
    StringFormatMask   = 0x00030000,
    AnsiClass          = 0x00000000,
    UnicodeClass       = 0x00010000,
    AutoClass          = 0x00020000,

    // Class Initialization
    BeforeFieldInit    = 0x00100000,

    // Additional Flags
    RTSpecialName      = 0x00000800,
    HasSecurity        = 0x00040000
}; }

namespace CallingConvention {
enum Values {
    Mask         = 0x0f,
    Default      = 0x00,
    C            = 0x01,
    StdCall      = 0x02,
    ThisCall     = 0x03,
    FastCall     = 0x04,
    VarArg       = 0x05,
    Field        = 0x06,
    LocalSig     = 0x07,
    Property     = 0x08,
    // Unmanaged = 0x09,
    // Max       = 0x10,

    HasThis      = 0x20,
    ExplicitThis = 0x40
}; }

} }

#endif//NINETJER_FLAGS_HPP
