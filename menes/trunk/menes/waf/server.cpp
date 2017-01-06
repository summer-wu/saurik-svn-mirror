/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
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

#include "api/files.hpp"
#include "api/process.hpp"

#include "ios/helpers.hpp"

#include "net/http/request.hpp"
#include "net/http/response.hpp"

#include "waf/server.hpp"

#include <sys/types.h>
#include <dirent.h>

namespace waf {

void Server::Process(const net::Http::Request &request, net::Http::Response &response) {
    api::Cout << request.uri_ << ios::NewLine;

    const _L<cse::String> &paths(request.uri_.GetPaths());

    if (paths.IsEmpty()) {
        response.SetStatus(403);
        return;
    }

    api::Path file(base_);
    unsigned depth(0);

    _foreach (const _L<cse::String>, path, paths) {
        if (path->IsEmpty() || *path == ".");
        else if (*path == "..") {
            if (depth == 0) {
                response.SetStatus(400);
                return;
            }

            --depth;
            file = file.GetParent();
        } else {
            ++depth;
            file = file.GetChild(*path);

            if (!file.Exists()) {
                response.SetStatus(404);
                return;
            }

            if (!file.IsDirectory()) {
                _L<cse::String> remain(path + 1, paths.End());

                if (path->EndsWithAll(".cgi")) {
                    // XXX: implement

                    response.SetStatus(200);

                    _L<cse::String> args;

                    _S<api::Process> cgi(file.GetPath(), args);
                    cgi.ClearWriter();

                    response.GetHeaders() = net::Http::Headers(cgi.GetReader());

                    ios::ReadToWrite(*cgi.GetReader(), response);
                    cgi.Join();
                } else if (path->EndsWithAll(".msp")) {
                    processor_.Process(file, *this, request, response);
                } else {
                    {
                        _S<api::FileReader> magic(CFG_MENES_ROOT "/menes/waf/extensions.dat");

                        ext::Buffer line;
                        while (ios::ReadLine(magic, line)) {
                            _L<ext::Buffer> pair(ext::SplitAll(line, _B(" ")));

                            if (path->GetData().EndsWithAll(pair[0])) {
                                response.AddHeader(_B("Content-Type"), pair[1]);
                                break;
                            }
                        }
                    }

                    _S<api::FileReader> fin(file.GetPath());
                    response.SetStatus(200);
                    response.AddHeader(_B("Content-Length"), lexical_cast<cse::String>(file.GetSize()));
                    ios::ReadToWrite(fin, response);
                }

                return;
            }
        }
    }

    if (!paths.Last().IsEmpty()) {
        response.SetStatus(301);
        response.AddHeader(_B("Location"), _S<ios::String>() << ext::JoinAll<cse::String>(paths, _B("/")) << "/");
    } else {
        api::Cout << file.GetPath() << ios::NewLine;
        _assert(request.uri_.GetPaths().Last().IsEmpty());

        // XXX: abstract
        ::DIR *dir(::opendir(file.GetPath().NullTerminate()));

        if (::DIR *dir = ::opendir(file.GetPath().NullTerminate())) {
            response.SetStatus(200);
            response.AddHeader(_B("Content-Type"), _B("text/html"));

            while (::dirent *entry = ::readdir(dir)) {
                cse::String name(entry->d_name);
                if (file.GetChild(name).IsDirectory())
                    name << "/";
                response << "<a href=\"" << name << "\">" << name << "</a><br/>";
            }

            ::closedir(dir);
        } else {
            response.SetStatus(500);
        }
    }
}

Server::Server(const cse::String &base) :
    base_(base)
{
}

}
