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

#ifndef MENES_NET_HTTP_RESPONSE_HPP
#define MENES_NET_HTTP_RESPONSE_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/types.hpp"

#include "net/http/headers.hpp"
#include "net/http/status.hpp"
#include "net/http/version.hpp"

namespace net {
namespace Http {

class Response :
    public ios::PrintWriter
{
  private:
    Version version_;
    _R<ios::Writer> client_;

    Status status_;

    // XXX: I hate C++ with a passion
    mutable Headers headers_;

    _R<ios::Writer> response_;

    void Commit_();

    virtual void WriteFully_(const byte_t *data, size_t length);

  public:
    Response(const Version &version, const _R<ios::Writer> &client);
    ~Response();

    bool IsCommitted() const;

    void SetStatus(const Status &status);
    const Status &GetStatus() const;

    void SetHeader(const cse::String &name, const _L<cse::String> &values);
    void AddHeader(const cse::String &name, const cse::String &value);
    const _L<cse::String> &GetHeader(const cse::String &name) const;
    void ClearHeader(const cse::String &name);

    Headers &GetHeaders();

    virtual void Flush();
};

} }

#endif//MENES_NET_HTTP_RESPONSE_HPP
