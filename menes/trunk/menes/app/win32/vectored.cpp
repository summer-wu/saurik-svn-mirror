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

#include "app/configuration.hpp"
#include "app/setup.hpp"

#include "ios/string.hpp"

#include <windows.h>
#include "api/win32/undefine.hpp"

class VectoredException :
    public ext::Exception
{
  private:
    EXCEPTION_RECORD record_;
    CONTEXT context_;

  public:
    VectoredException(EXCEPTION_POINTERS &pointers) :
        ext::Exception(true),
        record_(*pointers.ExceptionRecord),
        context_(*pointers.ContextRecord)
    {
    }

    virtual cse::String GetMessage() const {
        cse::String name;

        switch (record_.ExceptionCode) {
            case EXCEPTION_ACCESS_VIOLATION: name = _B("EXCEPTION_ACCESS_VIOLATION"); break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: name = _B("EXCEPTION_ARRAY_BOUNDS_EXCEEDED"); break;
            case EXCEPTION_BREAKPOINT: name = _B("EXCEPTION_BREAKPOINT"); break;
            case EXCEPTION_DATATYPE_MISALIGNMENT: name = _B("EXCEPTION_DATATYPE_MISALIGNMENT"); break;
            case EXCEPTION_FLT_DENORMAL_OPERAND: name = _B("EXCEPTION_FLT_DENORMAL_OPERAND"); break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO: name = _B("EXCEPTION_FLT_DIVIDE_BY_ZERO"); break;
            case EXCEPTION_FLT_INEXACT_RESULT: name = _B("EXCEPTION_FLT_INEXACT_RESULT"); break;
            case EXCEPTION_FLT_INVALID_OPERATION: name = _B("EXCEPTION_FLT_INVALID_OPERATION"); break;
            case EXCEPTION_FLT_OVERFLOW: name = _B("EXCEPTION_FLT_OVERFLOW"); break;
            case EXCEPTION_FLT_STACK_CHECK: name = _B("EXCEPTION_FLT_STACK_CHECK"); break;
            case EXCEPTION_FLT_UNDERFLOW: name = _B("EXCEPTION_FLT_UNDERFLOW"); break;
            case EXCEPTION_GUARD_PAGE: name = _B("EXCEPTION_GUARD_PAGE"); break;
            case EXCEPTION_ILLEGAL_INSTRUCTION: name = _B("EXCEPTION_ILLEGAL_INSTRUCTION"); break;
            case EXCEPTION_IN_PAGE_ERROR: name = _B("EXCEPTION_IN_PAGE_ERROR"); break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO: name = _B("EXCEPTION_INT_DIVIDE_BY_ZERO"); break;
            case EXCEPTION_INT_OVERFLOW: name = _B("EXCEPTION_INT_OVERFLOW"); break;
            case EXCEPTION_INVALID_DISPOSITION: name = _B("EXCEPTION_INVALID_DISPOSITION"); break;
            case EXCEPTION_INVALID_HANDLE: name = _B("EXCEPTION_INVALID_HANDLE"); break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION: name = _B("EXCEPTION_NONCONTINUABLE_EXCEPTION"); break;
            case EXCEPTION_PRIV_INSTRUCTION: name = _B("EXCEPTION_PRIV_INSTRUCTION"); break;
            case EXCEPTION_SINGLE_STEP: name = _B("EXCEPTION_SINGLE_STEP"); break;
            case EXCEPTION_STACK_OVERFLOW: name = _B("EXCEPTION_STACK_OVERFLOW"); break;
        }

        _S<ios::String> message;
        message << "[0x" << ios::SetBase(16) << record_.ExceptionCode << ios::SetBase(10);
        message << " @ 0x" << ios::SetBase(16) << record_.ExceptionAddress << ios::SetBase(10);
        message << "] " << name;

        return message;
    }
};

static LONG WINAPI VectoredHandler_(PEXCEPTION_POINTERS ExceptionInfo) {
    if ((ExceptionInfo->ExceptionRecord->ExceptionCode & 0xf0000000) == 0xc0000000)
        throw VectoredException(*ExceptionInfo);
    return EXCEPTION_CONTINUE_SEARCH;
}

namespace {
class PerProcess :
    public app::PerProcess
{
  private:
    void *handler_;

  public:
    PerProcess() :
        handler_(NULL)
    {
    }

    virtual void Start() {
        if (_C("menes/app/exceptions/@vectored", false)) {
            handler_ = ::AddVectoredExceptionHandler(0, &VectoredHandler_);
            if (handler_ == NULL)
                throw api::Win32::Error();
        }
    }

    virtual void Stop() {
        if (handler_ != NULL && 0 == ::RemoveVectoredExceptionHandler(handler_))
            throw api::Win32::Error();
    }
} perprocess_; }
