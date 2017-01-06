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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "net/http/request.hpp"
#include "net/http/response.hpp"

// XXX: this might be nice
//#include "waf/processor.hpp"

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

class ApacheWriter :
    public ios::Writer
{
    // XXX: implement
};

struct Request :
    public net::Http::Request
{
    Request(const ::request_rec &request) //:
        // XXX: grr!
	{
        // XXX: implement
	}
};

class Response :
    public net::Http::Response
{
  private:
    // XXX: implement
  public:
    Response(const net::Http::Version &version, const _R<ios::Writer> &client) :
        net::Http::Response(version, client)
    {
    }
};

extern "C" {

static int MenesPostConfig(::apr_pool_t *config, ::apr_pool_t *log, ::apr_pool_t *temp, ::server_rec * server)
{
    ::ap_add_version_component(config, "Menes/0.9");

    return OK;
}

static int MenesHandler(::request_rec *request)
{
    if (cse::String(request->handler) != "menes")
        return DECLINED;

    // XXX: stuff

    return OK;
}

static void MenesRegisterHooks(::apr_pool_t *pool)
{
    ::ap_hook_post_config(MenesPostConfig, NULL, NULL, APR_HOOK_MIDDLE);
    ::ap_hook_handler(MenesHandler, NULL, NULL, APR_HOOK_MIDDLE);
}

::module AP_MODULE_DECLARE_DATA menes_module = {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MenesRegisterHooks
};

}
