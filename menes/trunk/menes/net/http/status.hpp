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

#ifndef MENES_NET_HTTP_STATUS_HPP
#define MENES_NET_HTTP_STATUS_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/string.hpp"

namespace net {
namespace Http {

class Status {
  private:
    unsigned code_;
    cse::String reason_;

  public:
    Status() :
        code_(0)
    {
    }

    Status(unsigned code) :
        code_(code)
    {
        switch (code_) {
            case 100: reason_ = _B("Continue"); break;
            case 101: reason_ = _B("Switching Protocols"); break;

            case 200: reason_ = _B("OK"); break;
            case 201: reason_ = _B("Created"); break;
            case 202: reason_ = _B("Accepted"); break;
            case 203: reason_ = _B("Non-Authoritative Information"); break;
            case 204: reason_ = _B("No Content"); break;
            case 205: reason_ = _B("Reset Content"); break;
            case 206: reason_ = _B("Partial Content"); break;

            case 300: reason_ = _B("Multiple Choices"); break;
            case 301: reason_ = _B("Moved Permanently"); break;
            case 302: reason_ = _B("Found"); break;
            case 303: reason_ = _B("See Other"); break;
            case 304: reason_ = _B("Not Modified"); break;
            case 305: reason_ = _B("Use Proxy"); break;
            case 307: reason_ = _B("Temporary Redirect"); break;

            case 400: reason_ = _B("Bad Request"); break;
            case 401: reason_ = _B("Unauthorized"); break;
            case 402: reason_ = _B("Payment Required"); break;
            case 403: reason_ = _B("Forbidden"); break;
            case 404: reason_ = _B("Not Found"); break;
            case 405: reason_ = _B("Method Not Allowed"); break;
            case 406: reason_ = _B("Not Acceptable"); break;
            case 407: reason_ = _B("Proxy Authentication Required"); break;
            case 408: reason_ = _B("Request Timeout"); break;
            case 409: reason_ = _B("Conflict"); break;
            case 410: reason_ = _B("Gone"); break;
            case 411: reason_ = _B("Length Required"); break;
            case 412: reason_ = _B("Precondition Failed"); break;
            case 413: reason_ = _B("Request Entity Too Large"); break;
            case 414: reason_ = _B("Request-URI Too Long"); break;
            case 415: reason_ = _B("Unsupported Media Type"); break;
            case 416: reason_ = _B("Requested Range Not Satisfiable"); break;
            case 417: reason_ = _B("Expectation Failed"); break;

            case 500: reason_ = _B("Internal Server Error"); break;
            case 501: reason_ = _B("Not Implemented"); break;
            case 502: reason_ = _B("Bad Gateway"); break;
            case 503: reason_ = _B("Service Unavailable"); break;
            case 504: reason_ = _B("Gateway Timeout"); break;
            case 505: reason_ = _B("HTTP Version Not Supported"); break;
        }
    }

    Status(unsigned code, const cse::String &reason) :
        code_(code),
        reason_(reason)
    {
    }

    unsigned GetCode() const {
        return code_;
    }

    const cse::String &GetReason() const {
        return reason_;
    }
};


} }

#endif//MENES_NET_HTTP_STATUS_HPP
