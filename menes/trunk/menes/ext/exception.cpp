/* Menes - C++ High-Level Utility Library
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/stacktrace.hpp"
#include "app/configuration.hpp"

#include "ext/exception.hpp"
#include "cse/string.hpp"

namespace ext {

Exception::Exception(bool trace) :
    trace_(NULL)
{
    if (trace)
        Trace();
    if (_C("menes/ext/exceptions/@break", false))
        _breakpoint();
}

Exception::Exception(const Exception &rhs) :
    trace_(rhs.trace_ == NULL ? NULL : new api::StackTrace(*rhs.trace_))
{
}

Exception::~Exception() {
    delete trace_;
}

cse::String Exception::GetMessage() const {
    return cse::EmptyString;
}

const api::StackTrace *Exception::GetTrace() const {
    return trace_;
}

void Exception::Trace() {
    trace_ = new api::StackTrace();
}

EXT_DECLARE ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const Exception &rhs) {
    lhs << "[" << typeid(rhs).name() << "]: \"" << rhs.GetMessage() << "\"";

    const api::StackTrace *trace(rhs.GetTrace());
    if (trace != NULL && !trace->IsEmpty()) try {
        lhs << ios::NewLine << *trace;
    } _catch {
    }

    return lhs;
}

StringException::StringException() {
}

StringException::StringException(const cse::String &string) :
    string_(string)
{
}

cse::String StringException::GetMessage() const {
    return string_;
}

NotImplementedException::NotImplementedException() {
}

NotImplementedException::NotImplementedException(const cse::String &string) :
    StringException(string)
{
}

}
