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

#include "api/pcre/regex.hpp"
#include "hop/bind.hpp"

#include "ios/filter.hpp"
#include "ios/helpers.hpp"
#include "ios/limit.hpp"

#include "net/http/chunked.hpp"
#include "net/http/request.hpp"
#include "net/http/response.hpp"
#include "net/http/server.hpp"
#include "net/http/urlencode.hpp"

#include "xml/xmlencode.hpp"

namespace net {
namespace Http {

void Server::Handle_(const _R<api::Socket> &socket) {
    try {
        bool closed(false);

        do {
            Request request;
            request.client_ = socket->GetRemoteAddress();

            cse::String uri;

            {
                ext::Buffer command;

                try {
                    do ios::ReadLine(*socket, command);
                    while (command.IsEmpty());
                } _catch {
                    // XXX: maybe this needs to look at the exception?
                    return;
                }

                static api::Pcre::RegEx modern("([^ ]*) (.*) HTTP/([0-9]+).([0-9]+)");
                static api::Pcre::RegEx ancient("([^ ]*) (.*)");

                if (api::Pcre::RegEx::Match match = modern(command)) {
                    request.method_ = match[1];
                    uri = match[2];
                    request.version_ = Version(match[3], match[4]);
                } else if (api::Pcre::RegEx::Match match = ancient(command)) {
                    request.method_ = match[1];
                    uri = match[2];
                } else {
                    // XXX: return Bad Request error
                    _assert(false);
                }
            }

            if (request.version_.AtLeast(1, 0))
                request.headers_ = Headers(socket);

            {
                Headers::ConstIterator cookies(request.headers_.Find("Cookie"));
                if (cookies != request.headers_.End())
                    _foreach (const _L<cse::String>, cookie, ext::SplitAll(cookies->Second().First(), _B(";"))) {
                        // XXX: syntax handling?
                        cse::String::ConstIterator equal(ext::FindFirst(*cookie, '='));
                        cse::String name(cookie->Begin(), equal);
                        cse::String value(equal + 1, cookie->End());
                        // XXX: now what?
                    }
            }

            {
                Headers::ConstIterator length(request.headers_.Find("Content-Length"));
                if (length != request.headers_.End())
                    request.content_ = new _H<ios::LimitReader>(socket, lexical_cast<unsigned>(length->Second()[0]));
            }

            {
                Headers::ConstIterator encodings(request.headers_.Find("Transfer-Encoding"));
                if (encodings != request.headers_.End()) {
                    if (request.content_.IsEmpty())
                        request.content_ = socket;
                    _foreach (const _L<cse::String>, encoding, encodings->Second())
                        if (*encoding == _B("chunked"))
                            request.content_ = new _H<ChunkedReader>(request.content_);
                        else
                            _assert(false);
                }
            }

            _S<Response> response(request.version_, socket);

            if (ext::Contains(request.headers_["Connection"], _B("keep-alive")))
                response.AddHeader("Connection", "keep-alive");
            else {
                closed = true;
                response.AddHeader("Connection", "close");
            }

            response.AddHeader("Server", "Menes/0.9");

            if (uri != "*") {
                request.uri_ = uri;

                if (
                    request.uri_.GetPaths().IsEmpty() ||
                    !request.uri_.GetPaths().First().IsEmpty()
                ) {
                    response.SetStatus(400);
                    return;
                }
            }

            try {
                Process(request, response);
            } catch (const ext::Exception &exception) {
                if (!response.IsCommitted()) {
                    response.SetStatus(500);
                    response.ClearHeader("Content-Type");
                    response.AddHeader("Content-Type", "text/html");

                    response << "<html><head><title>Internal Server Error</title>" << ios::NewLine;
                    response << "    <style>" << ios::NewLine;
                    response << "        body {" << ios::NewLine;
                    response << "            font-family: verdana, sans-serif;" << ios::NewLine;
                    response << "        }" << ios::NewLine;
                    response << "    </style>" << ios::NewLine;
                    response << "</head><body><pre>" << ios::NewLine;

                    {
                        _S<xml::XmlEncoder> encoder(&response);
                        _S<ios::FormatWriter> response(encoder);
                        response << exception.GetMessage() << ios::NewLine;
                        response << *exception.GetTrace() << ios::NewLine;
                    }

                    response << "</pre></body>" << ios::NewLine;
                }

                throw;
            }
        } while (!closed && !managers_.IsEmpty());

        // XXX: maybe I _shouldn't_ keep the socket if I need to shutdown the server
        // if (closed)

        // XXX: this should be rethought
        //socket->Keep();

        socket->ShutdownWrite();
        ios::Discard(*socket);
    } catch (const ext::Exception &exception) {
        api::Cout << exception.GetMessage() << ios::NewLine;
        api::Cout << *exception.GetTrace() << ios::NewLine;
    }
}

} }
