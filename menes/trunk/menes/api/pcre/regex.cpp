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

#include "api/pcre/error.hpp"
#include "api/pcre/regex.hpp"

namespace api {
namespace Pcre {

RegEx::Match::Match(const cse::String::Data &data, Substrings_ &substrings) :
    data_(data)
{
    ext::Swap(substrings_, substrings);
}

RegEx::Match::Match() {
}

RegEx::Match::operator bool() const {
    return !substrings_.IsEmpty();
}

cse::String RegEx::Match::operator [](size_t substring) const {
    size_t index(substring * 2);

    if (index >= substrings_.GetSize())
        return cse::String();

    // XXX: I think substrings_[index] can be -1...
    return cse::String::Data(data_.Begin() + substrings_[index], substrings_[index + 1] - substrings_[index]);
}

RegEx::RegEx(const cse::String &pattern) :
    code_(NULL),
    study_(NULL),
    capture_(-1)
{
    const char *error;
    int offset;

    code_ = ::pcre_compile(pattern.NullTerminate(), PCRE_DOTALL | PCRE_DOLLAR_ENDONLY, &error, &offset, NULL);
    int err(::pcre_fullinfo(code_, study_, PCRE_INFO_CAPTURECOUNT, &capture_));

    if (err != 0) {
        _assert(err < 0);
        throw Error(err);
    }

    _assert(capture_ >= 0);
}

RegEx::~RegEx() {
    ::pcre_free(code_);
}

RegEx::Match RegEx::operator ()(const cse::String &data) const {
    Substrings_ substrings((capture_ + 1) * 3);
    // XXX: this is NullTerminate() instead of GetData().Begin() because it _must_ be non-NULL
    int err(::pcre_exec(code_, study_, data.NullTerminate(), data.GetData().GetSize(), 0, 0, substrings.Begin(), substrings.GetSize()));

    _assert(err != 0);
    if (err == PCRE_ERROR_NOMATCH)
        err = 0;
    if (err < 0)
        throw Error(err);

    substrings.SetSize(err * 2);
    return Match(data.GetData(), substrings);
}

RegEx::Match RegEx::operator ()(const char *data, size_t length) const {
    return operator ()(cse::String(data, length));
}

} }
