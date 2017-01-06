/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "ios/streambuf.hpp"

#include "net/http/chunked.hpp"
#include "net/http/response.hpp"

namespace net {
namespace Http {

void Response::Commit_() {
    _assert(response_.IsEmpty());

    _S<ios::FormatWriter> out(*client_);
    out << ios::SetLineSeparator("\r\n");
    out << "HTTP/" << version_.GetMajor() << "." << version_.GetMinor() << " ";
    out << ios::SetFill('0') << ios::SetWidth(3) << status_.GetCode();
    out << " " << status_.GetReason() << ios::NewLine;

    response_ = client_;

    if (
        version_.AtLeast(1,1) &&
        GetHeader("Transfer-Encoding").IsEmpty() &&
        GetHeader("Content-Length").IsEmpty()
    ) {
        response_ = new _H<ChunkedWriter>(response_);
        AddHeader(_B("Transfer-Encoding"), _B("chunked"));
    }

    response_ = new _H<ios::WriterBuffer>(response_);

    _foreach (Headers, header, headers_)
        _foreach (_L<cse::String>, value, header->Second())
            out << header->First() << ": " << *value << ios::NewLine;

    out << ios::NewLine;
}

void Response::WriteFully_(const byte_t *data, size_t length) {
    if (response_.IsEmpty())
        Commit_();
    response_->WriteFully(data, length);
}

Response::Response(const Version &version, const _R<ios::Writer> &client) :
    version_(version),
    client_(client)
{
}

Response::~Response() {
    if (response_.IsEmpty())
        Commit_();
}

bool Response::IsCommitted() const {
    return !response_.IsEmpty();
}

void Response::SetStatus(const Status &status) {
    status_ = status;
}

const Status &Response::GetStatus() const {
    return status_;
}

void Response::SetHeader(const cse::String &name, const _L<cse::String> &values) {
    headers_[name] = values;
}

void Response::AddHeader(const cse::String &name, const cse::String &value) {
    headers_[name].InsertLast(value);
}

const _L<cse::String> &Response::GetHeader(const cse::String &name) const {
    return headers_[name];
}

void Response::ClearHeader(const cse::String &name) {
    headers_[name].Clear();
}

Headers &Response::GetHeaders() {
    return headers_;
}

void Response::Flush() {
    response_->Flush();
}

} }
