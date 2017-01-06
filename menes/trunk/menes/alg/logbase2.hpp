/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003  Jay Freeman (saurik)
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

#ifndef MENES_ALG_LOGBASE2_HPP
#define MENES_ALG_LOGBASE2_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/template.hpp"
#include "etl/traits.hpp"

namespace alg {

/* // this function is worthless and conflicts anyway
inline int32_t ilog2(float value) {
    uint32_t ivalue(reinterpret_cast<uint32_t &>(value));
    uint32_t exp((ivalue >> 23) & 0xff);
    return int32_t(exp) - 127;
} */

#if defined(MENES_ASM_X86_MSC)

inline int32_t ilog2(uint32_t value) {
    int32_t result;
    __asm bsr eax, value
    __asm mov result, eax
    return result;
}

#elif defined(MENES_ASM_X86_GCC) || defined(MENES_ASM_X86_64_GCC)

inline int32_t ilog2(uint32_t value) {
    int32_t result;
    __asm__ ("bsr %1, %%eax; mov %%eax, %0;" : "=r"(result) : "r"(value) : "%eax");
    return result;
}

#elif defined(MENES_ASM_PPC_GCC) && defined(_ARCH_PPC64)

// XXX: why is this a template again? ambiguous overload? bastards...
template <typename Type_>
inline typename etl::IfTrue<
    etl::IsIntegral<Type_>::value && sizeof(Type_) == 4,
    int32_t
>::Result ilog2(Type_ value) {
    int32_t result;
    __asm__ ("cntlzw %0, %1" : "=r" (result) : "r" (value));
    return 31 - result;
}

inline int32_t ilog2(uint64_t value) {
    int32_t result;
    __asm__ ("cntlzd %0, %1" : "=r" (result) : "r" (value));
    return 63 - result;
}

#else

/* // XXX: this counts the number of set bits
_finline int ilog2(uint32_t value) {
    value = (value & 0x55555555) + ((value >>  1) & 0x55555555);
    value = (value & 0x33333333) + ((value >>  2) & 0x33333333);
    value = (value & 0x0f0f0f0f) + ((value >>  4) & 0x0f0f0f0f);
    value = (value & 0x00ff00ff) + ((value >>  8) & 0x00ff00ff);
    value = (value & 0x0000ffff) + ((value >> 16)& 0x0000ffff);
    return value;
} */

inline int32_t ilog2(uint32_t value) {
    return value == 0 ? -1 : 1 + ilog2(value >> 1);
}

#endif

}

#endif//MENES_ALG_LOGBASE2_HPP
