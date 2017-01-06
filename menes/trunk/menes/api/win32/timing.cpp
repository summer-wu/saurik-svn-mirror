/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2004  Jay Freeman (saurik)
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

#include "api/win32/error.hpp"
#include "api/win32/timing.hpp"

#include "opt/profile.hpp"

#include <wtypes.h>
#include <winbase.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

uint64_t QueryPerformanceTimer::GetValue() {
    OPT_TIMER("api::Win32::QueryPerfTimer::GetValue()")

    uint64_t counter;
    if (w32::False == ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&counter)))
        throw Error();
    return counter;
}

uint64_t QueryPerformanceTimer::GetFrequency() {
    static uint64_t frequency(0);
    if (frequency == 0)
        if (w32::False == ::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&frequency)))
            throw Error();
    return frequency;
}

} }
