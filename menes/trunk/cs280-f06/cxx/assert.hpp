/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#ifndef MENES_CXX_ASSERT_HPP
#define MENES_CXX_ASSERT_HPP

#include "cxx/platform.hpp"
#include "cxx/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

_finline void _breakpoint() {
#if defined(MENES_ASM_X86_MSC)
    __asm { int 0x03 };
#elif defined(MENES_ASM_X86_GCC)
    __asm__ ("int $0x03");
#elif defined(MENES_ASM_PPC_GCC)
    __asm__ ("twge r2, r2");
#else
    *static_cast<int *>(NULL) = 0;
#endif
}

namespace be {

CXX_DECLARE extern void (*Failure_)(const char *msg);

_finline void Assert_(bool check, const char *msg = NULL) {
#ifdef MENES_COMPILER_GXX
    if (__builtin_expect(!check, 0))
#else
    if (!check)
#endif
        Failure_(msg);
}

}

#define _nodefault \
    default: \
        _assume(false); \
        throw;

#if defined(NASSERT)
#define _assert(b)
#else
#define _assert(b) ::be::Assert_(b, #b)
#endif

#endif//MENES_CXX_ASSERT_HPP
