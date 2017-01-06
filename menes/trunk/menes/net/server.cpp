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

#include "hop/bind.hpp"
#include "net/server.hpp"

namespace net {

int Server::Client_(_R<api::Socket> &socket) {
    ext::ScopeReadLock<mta::ReaderWriterLock> active(active_);
    Handle_(socket);
    return 0;
}

int Server::Manager_(api::InternetAddress::Port port) {
    _S<api::TcpSocket> socket;

    socket.SetLocalAddress(api::InternetAddress(api::InternetAddress::Any, port), true);
    socket.Listen();

    api::Cout << "Listening..." << ios::NewLine;

    _forever {
        api::Address address;
        _R<api::Socket> client(new _H<api::Socket>(socket, &address));
        new _H<api::Thread>(hop::BindAll(&Server::Client_, this, client));
    }

    return 0;
}

void Server::_finalize() {
    Block();
}

void Server::AddPort(const api::InternetAddress::Port &port) {
    managers_.InsertLast(new _H<api::Thread>(hop::BindAll(&Server::Manager_, this, port)));
}

void Server::Block() {
    _foreach (_L< _R<api::Thread> >, manager, managers_)
        (*manager)->Join();
    managers_.Clear();
    ext::ScopeWriteLock<mta::ReaderWriterLock> active(active_);
}

}
