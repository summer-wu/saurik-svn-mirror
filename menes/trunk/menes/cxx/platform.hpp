/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2001-2005  Jay Freeman (saurik)
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

#ifndef MENES_CXX_PLATFORM_HPP
#define MENES_CXX_PLATFORM_HPP

//#define private public

#ifdef CFG_HAS_CONFIG
#include "cfg/menes.h"
#include "cfg/menes.hpp"
#else
#include "cxx/autoconf.hpp"
#endif

// XXX: provide abstraction for http://predef.sourceforge.net/prearch.html

/////////////////////////
// Compiler: Visual C++

#if defined(_MSC_VER)

// C4181: qualifier applied to reference type; ignored
// C4200: nonstandard extension used : zero-sized array in struct/union
// C4231: nonstandard extension used : 'extern' before template explicit instantiation
// C4244: 'argument' : conversion from '...' to '...', possible loss of data
// C4250: '...' : inherits '...' via dominance
// C4251: '...' : class '...' needs to have dll-interface to be used by clients of class '...'
// C4258: '...' : definition from the for loop is ignored; the definition from the enclosing scope is used
// C4275: non dll-interface class '...' used as base for dll-interface class '...'
// C4345: behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
// C4355: 'this' : used in base member initializer list
// C4503: '...' : decorated name length exceeded, name was truncated
// C4554: '...' : check operator precedence for possible error; use parentheses to clarify precedence
// C4584: '...' : base-class '...' is already a base-class of '...'
// C4661: '...' : no suitable definition provided for explicit template instantiation request
// C4675: '...' : resolved overload was found by argument-dependent lookup
// C4786: '...' : identifier was truncated to '255' characters in the debug information
#pragma warning(disable: 4181 4200 4231 4244 4250 4251 4258 4275 4345 4355 4503 4554 4584 4661 4675 4786)

#define INT64_C(n) n ## i64
#define UINT64_C(n) n ## Ui64

#ifdef NDEBUG
#define _assume(e) __assume(e)
#else
#define _assume(e) _assert(e)
#endif

#if _MSC_VER < 1300
#define _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#endif

#if _MSC_VER > 1000
#define MENES_PRAGMA_ONCE
#endif

#define _finline __forceinline
#define _pure

#if _MSC_VER < 1300
#define _deprecated
#else
#define _deprecated __declspec(deprecated)
#endif

#define _abstract _declspec(novtable)
#define _aligned(value) __declspec(align(value))
#define _w32cdecl __cdecl

// ERR: error C2649: '?$SmartPointer@$DBAAB@PAVtemplate-parameter-BAAB@@V?$ThreadLocal@$DBAAB@$Q(*(_BAAC@))@Win32@api@@' : is not a 'class'
#define _w32class

#ifdef MENES_ALL_DECLARE_STATIC
#define _dllexport
#define _dllimport
#else
#define _dllexport __declspec(dllexport)
#define _dllimport __declspec(dllimport)
#endif

#define NULL 0

/////////////////////////////////
// Compiler: Intel C++ Compiler

#elif defined(__ICC)

// #21: type qualifiers are meaningless in this declaration
// #186: pointless comparison of unsigned integer with zero
// #327: NULL reference is not allowed
// #411: class "..." defines no constructor to initialize the following: reference member "..."
// #488: template parameter "..." is not used in declaring the parameter types of function template "..."
// #1505: size of class is affected by tail padding
// #1572: floating-point equality and inequality comparisons are unreliable
#pragma warning(disable: 21 186 327 411 488 1505 1572)

// XXX: icc has to support more of these... right?

#ifdef NDEBUG
#define _assume(e) static_cast<void>(0)
#else
#define _assume(e) _assert(e)
#endif

#ifdef NFINLINE
#define _finline inline
#else
#define _finline __forceinline
#endif

#define _pure
#define _deprecated
#define _abstract
#define _w32cdecl
#define _w32class class

#define NULL 0
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ offset_t;

/* // XXX: workaround for the following error:
/usr/include/netinet/in.h(488): error: expected either a definition or a tag name
  extern int setipv4sourcefilter (int __s, struct in_addr __interface, */
#define __interface __ninterface

//////////////////
// Compiler: g++

#elif __GNUC__

// XXX: I'd be willing to believe this is a bad idea
#define _packed __attribute__((packed))

#ifdef NDEBUG
#define _assume(e) static_cast<void>(0)
#else
#define _assume(e) _assert(e)
#endif

#ifdef NFINLINE
#define _finline inline
#else
#define _finline inline __attribute__((always_inline))
#endif

#ifdef NPURE
#define _pure
#else
#define _pure __attribute__ ((pure))
#endif

#ifdef NDEPRECATED
#define _deprecated
#else
#define _deprecated __attribute__((deprecated))
#endif

#define _abstract

// XXX: this isn't nearly as useful as one would hope as the gcc bastards
// decided to have their attributes at the end rather than at the beginning
// (where it just plain makes more sense, this is a modification of a type).
// Because of this stupid decision, you can't have a portable means of specifying
// alignment between gcc and the rest of the world (the latter being the group
// of people who tend to put special attributes on the left).

#define _aligned(value) __attribute__((__aligned__(value)))
#define _w32cdecl
#define _w32class class

#ifdef MENES_ALL_DECLARE_STATIC
#define _dllexport
#define _dllimport
#else
#define _dllexport __attribute__((visibility("default")))
#define _dllimport
#endif

#define NULL __null
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ offset_t;

#endif

//////////////////////
// Platform: Windows

#if defined(_WIN32)

#define uuid_t uuid_t
#define min min
#define max max

#if _MSC_VER < 1300
#define _cpp_min min
#define _cpp_max max
#endif

#define WINVER        0x0500
#define _WIN32_IE     0x0600
#define _WIN32_WINNT  0x0500

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define _WINSOCKAPI_

typedef signed char         int8_t;
typedef short int           int16_t;
typedef int                 int32_t;
typedef __int64             int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;

#if _MSC_VER < 1300
#define __w64
#endif

#define _INTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED

typedef __w64 int           intptr_t;
typedef __w64 unsigned int  uintptr_t;

typedef __w64 long          longptr_t;
typedef __w64 unsigned long ulongptr_t;

#define _SIZE_T_DEFINED

#ifdef  _WIN64
typedef uint64_t size_t;
typedef int64_t offset_t;
#else
typedef __w64 uint32_t size_t;
typedef __w64 int32_t offset_t;
#endif

////////////////////
// Platform: Linux

#elif defined(__linux__)

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>

typedef long          longptr_t;
typedef unsigned long ulongptr_t;

////////////////////
// Platform: FreeBSD

#elif defined(__FreeBSD__)

#define MENES_STL_NOWSTRING

#include <inttypes.h>

// XXX: build a complex system for automatically dealing with this like http://www-evasion.imag.fr/Membres/Frank.Perbet/extdox/boost/html/cstdint_8hpp-source.html

#define INT64_C(n) n ## LL
#define UINT64_C(n) n ## LL

typedef long          longptr_t;
typedef unsigned long ulongptr_t;

////////////////////
// Platform: Apple

#elif defined(__APPLE__)

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <machine/types.h>

typedef long          longptr_t;
typedef unsigned long ulongptr_t;

#include <cctype>

#undef _A
#undef _B
#undef _C
#undef _D
#undef _E
#undef _F
#undef _G
#undef _H
#undef _I
#undef _J
#undef _K
#undef _L
#undef _M
#undef _N
#undef _O
#undef _P
#undef _Q
#undef _R
#undef _S
#undef _T
#undef _U
#undef _V
#undef _W
#undef _X
#undef _Y
#undef _Z

// XXX: this seems sketchy
#ifndef CFG_DEFINE_TARGET_RT_MAC_CFM
#define TARGET_RT_MAC_CFM 0
#endif

#endif

////////////
// General

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include <cassert>
#undef assert
#undef _assert

typedef char byte_t;

#define _catch catch (...)

#ifndef __FUNCTION__
#define __FUNCTION__ "??"
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
#define MENES_ASM_X86_MSC
#elif defined(__GNUC__) && defined(__i386__)
#define MENES_ASM_X86_GCC
#elif defined(__GNUC__) && defined(__x86_64__)
#define MENES_ASM_X86_64_GCC
#elif defined(__GNUC__) && defined(_ARCH_PPC)
#define MENES_ASM_PPC_GCC
#endif

#endif//MENES_CXX_PLATFORM_HPP
