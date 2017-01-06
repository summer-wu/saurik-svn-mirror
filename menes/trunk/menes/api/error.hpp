/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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

#ifndef MENES_API_ERROR_HPP
#define MENES_API_ERROR_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/string.hpp"
#include "ext/exception.hpp"
#include "ios/string.hpp"

#ifdef MENES_DEBUG_APIERROR_STRING
#include "win32/dbgstream.hpp"
#endif

namespace api {

class NotImplementedException :
    public ext::Exception
{
};

class UnexpectedErrorException :
    public ext::Exception
{
};

class API_DECLARE Error :
    public ext::Exception
{
  protected:
    explicit Error(bool trace = true) :
        Exception(trace)
    {
    }

    ~Error() {
    }

  public:
    virtual int GetNumber() const = 0;
};

template <typename Code_>
struct ErrorTraits {
    typedef Code_ ErrorCode;

    static int ToInteger(const Code_ &code) {
        return static_cast<int>(code);
    }
};

template <typename Traits_>
class ErrorImpl :
    public Error
{
  public:
    typedef Traits_ Traits;
    typedef typename Traits::ErrorCode ErrorCode;

  protected:
    ErrorCode code_;

  public:
    ErrorImpl() :
        code_(Traits::GetLastError())
    {
        cse::String msg(GetMessage());
    }

    explicit ErrorImpl(ErrorCode code /*, const char *file = NULL, unsigned line = 0*/) :
        Error(/*file, line*/),
        code_(code)
    {
        cse::String msg(GetMessage());
    }

    virtual int GetNumber() const {
        return Traits_::ToInteger(code_);
    }

    virtual ErrorCode GetCode() const {
        return code_;
    }

    virtual cse::String GetMessage() const {
        _S<ios::String> message;
        message << Traits::GetName() << "[#";
        message << ios::SetBase(Traits::ErrorBase) << code_ << ios::SetBase(10);
        message << "] " << Traits_::GetMessage(code_);
        return message;
    }
};

}

#endif//MENES_API_ERROR_HPP
