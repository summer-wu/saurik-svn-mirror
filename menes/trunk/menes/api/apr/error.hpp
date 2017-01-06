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

#ifndef MENES_API_APR_ERROR_HPP
#define MENES_API_APR_ERROR_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/error.hpp"

#include <apr_errno.h>

namespace api {
namespace Apr {

struct API_DECLARE ApiTraits :
    public ErrorTraits< ::apr_status_t >
{
    static const unsigned ErrorBase = 10;

    static const char *GetName() {
        return "APR";
    }

    static ErrorCode GetLastError() {
        throw ext::NotImplementedException();
    }

    static cse::String GetMessage(const ErrorCode &code) {
        char message[1024];
        return ::apr_strerror(code, message, sizeof(message));
    }
};

API_DECLARE typedef ErrorImpl<ApiTraits> Error;

_finline void CheckError(apr_status_t status) {
    if (status != APR_SUCCESS)
        throw Error(status);
}

} }

#endif//MENES_API_APR_ERROR_HPP
