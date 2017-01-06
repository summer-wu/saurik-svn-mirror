/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#ifndef MENES_WAF_PROCESSOR_HPP
#define MENES_WAF_PROCESSOR_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/module.hpp"
#include "api/path.hpp"

namespace api {
namespace Jni {
    EXT_HANDLESTUB(class Object)
} }

namespace net {
namespace Http {
    class Request;
    class Response;
    class Server;
} }

namespace waf {

typedef void (*WafProcessor)(net::Http::Server &server, const net::Http::Request &_request, net::Http::Response &_response);

class Processor {
  private:
    _R<api::Jni::Object> taglib_;

    struct Compiled_ {
        ::time_t modified_;
        _R<api::Module> module_;
        WafProcessor process_;

        Compiled_();
    };

    ext::RedBlackMap<cse::String, Compiled_> compiled_;

  public:
    Processor();

    void Process(const api::Path &file, net::Http::Server &server, const net::Http::Request &request, net::Http::Response &response);
};

}

#endif//MENES_WAF_PROCESSOR_HPP
