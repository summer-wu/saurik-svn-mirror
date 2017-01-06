/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
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

#include "api/process.hpp"
#include "app/simple.hpp"

int Main(const app::Options &options) {
    cse::String source(*options[0]);

    cse::String destination(*options[1]);
    cse::String::Iterator colon(ext::FindFirst(destination, ':'));
    _assert(colon != destination.End());

    cse::String address(destination.Begin(), colon);
    cse::String path(colon + 1, destination.End());

    _L<cse::String> args;
    args.InsertLast(address);
    args.InsertLast(_B("net-sync"));
    args.InsertLast(_B("--server"));

    {
        _S<api::Process> ssh(_B("/usr/bin/ssh"), args);
        ssh.ClearWriter();
        ios::ReadToWrite(*ssh.GetReader(), api::Cout);
    }

    return 0;
}
