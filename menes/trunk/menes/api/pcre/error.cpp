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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/pcre/error.hpp"

#include <pcre.h>

namespace api {
namespace Pcre {

const char *ApiTraits::GetName() {
    return "pcre";
}

ApiTraits::ErrorCode ApiTraits::GetLastError() {
    throw;
}

cse::String ApiTraits::GetMessage(const ErrorCode &code) {
    switch (code) {
        case PCRE_ERROR_NOMATCH:        return "PCRE_ERROR_NOMATCH";
        case PCRE_ERROR_NULL:           return "PCRE_ERROR_NULL";
        case PCRE_ERROR_BADOPTION:      return "PCRE_ERROR_BADOPTION";
        case PCRE_ERROR_BADMAGIC:       return "PCRE_ERROR_BADMAGIC";
        case PCRE_ERROR_UNKNOWN_NODE:   return "PCRE_ERROR_UNKNOWN_NODE";
        case PCRE_ERROR_NOMEMORY:       return "PCRE_ERROR_NOMEMORY";
        case PCRE_ERROR_NOSUBSTRING:    return "PCRE_ERROR_NOSUBSTRING";

#ifdef PCRE_ERROR_MATCHLIMIT
        case PCRE_ERROR_MATCHLIMIT:     return "PCRE_ERROR_MATCHLIMIT";
#endif

#ifdef PCRE_ERROR_CALLOUT
        case PCRE_ERROR_CALLOUT:        return "PCRE_ERROR_CALLOUT";
#endif

#ifdef PCRE_ERROR_BADUTF8
        case PCRE_ERROR_BADUTF8:        return "PCRE_ERROR_BADUTF8";
#endif

#ifdef PCRE_ERROR_BADUTF8_OFFSET
        case PCRE_ERROR_BADUTF8_OFFSET: return "PCRE_ERROR_BADUTF8_OFFSET";
#endif
    }

    return "Unknown Error";
}

} }
