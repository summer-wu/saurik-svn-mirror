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

#include "api/threads.hpp"
#include "hop/bind.hpp"
#include "ios/filter.hpp"

#include "net/http/headers.hpp"
#include "net/http/ssl.hpp"
#include "net/http/uri.hpp"
#include "net/http/urlencode.hpp"

#include "net/msn/connection.hpp"
#include "net/msn/notification.hpp"
#include "net/msn/session.hpp"
#include "net/msn/switchboard.hpp"
#include "net/msn/transaction.hpp"

namespace net {
namespace MsnMessenger {

EXT_HANDLESHIM(Connection)

namespace be {

void ParseArgs_(_M<cse::String, cse::String> &args, const cse::String &string) {
    _foreach (const _L<cse::String>, pair, ext::SplitAll(string, _B(","))) {
        cse::String::ConstIterator equals(ext::FindFirst(*pair, '='));
        _assert(equals != pair->End());
        cse::String name(pair->Begin(), equals);
        cse::String value(equals, pair->End());
        args[ios::Filter<net::Http::UrlDecoder>(name)] = ios::Filter<net::Http::UrlDecoder>(value);
    }
}

_L<api::Address> ResolveHost_(const cse::String &address) {
    cse::String::ConstIterator colon(ext::FindFirst(address, ':'));
    _assert(colon != address.End());

    cse::String host(address.Begin(), colon);
    cse::String port(colon + 1, address.End());

    _L<api::Address> addresses(api::Address::Resolve(host, port));
    _assert(addresses.GetSize() == 1);

    return addresses;
}

_M< cse::String, _L<cse::String> > Head_(const cse::String &url, _M<cse::String, cse::String> &request) {
    net::Http::Uri uri(url);

    cse::String host(uri.GetAuthority());
    request[_B("Host")] = host;

    _S<api::TcpSocket> socket;
    socket.Connect(ResolveHost_(host)[0]);

    /*_S<net::Http::Ssl> ssl(socket, socket);
    ios::ReadLine(ssl);*/

    return net::Http::Headers(&socket /* &ssl */);
}

}

int Connection::Handle_() {
    for (ext::Buffer line; ios::ReadLine(socket_, line); ) {
        api::Cout << "> " << line << ios::NewLine;

        _L<cse::String> words(ext::SplitAll(cse::String(line), _B(" ")));

        unsigned transactionid(lexical_cast<unsigned>(words[1]));
        _R<ext::ReferenceCounted> completion;

        if (words[0] == _B("CVR")) {
            Send(_B("USR"), _Vs(_B("TWN"), _B("I"), GetSession().GetEmail()));
        } else if (words[0] == _B("USR")) {
            cse::String method(words[2]);

            if (method == _B("MD5")) {
                /*String unknown = tokens.nextToken();
                String challenge = tokens.nextToken();
                String hash = md5hash(challenge + password_);
                send("USR", "MD5 " + unknown + " " + hash);*/
            } else if (method == _B("TWN")) {
                _assert(words[3] == _B("S"));
                _M<cse::String, cse::String> args;
                be::ParseArgs_(args, words[4]);

                _M<cse::String, cse::String> request;
                request[_B("Connection")] = _B("close");

                _M<cse::String, cse::String> urls;
                be::ParseArgs_(urls, be::Head_(_B("https://nexus.passport.com/rdr/pprdr.asp"), request)[_B("PassportURLs")][0]);

                _S<ios::String> dalogin;
                dalogin << _B("https://") << urls[_B("DALogin")];

                api::Cout << dalogin << ios::NewLine;
            } else _assert(false);
        } else if (words[0] == _B("VER")) {
            Send(_B("CVR"), _Vs(
                _B("0x0409"),
                _B("winnt"),
                _B("5.1"),
                _B("i386"),
                _B("MSNMSGR"),
                _B("6.0.0602"),
                _B("MSMSGS"),
                GetSession().GetEmail()
            ));
        } else if (words[0] == _B("XFR")) {
            cse::String type(words[2]);
            cse::String address(words[3]);
            cse::String method(words[4]);
            cse::String data(words[5]);

            _L<api::Address> addresses(be::ResolveHost_(address));

            _R<Connection> connection;

            if (type == _B("NS")) {
                _assert(method == _B("0"));
                connection = new _H<NotificationServer>(GetSession(), addresses[0]);
            } else if (type == _B("SB")) {
                _assert(method == _B("CKI"));
                connection = new _H<SwitchBoard>(GetSession(), addresses[0], data);
            } else _assert(false);
        }

        if (transactionid_ != 0)
            _synchronized (transactionlock_) {
                transactions_[transactionid]->SetValue(completion);
                transactions_.Remove(transactionid);
            }
    }

    api::Cout << "!--!" << ios::NewLine;

    // XXX: this should be more forceful; maybe a deconstructor?
    session_.RemoveConnection(this);

    return 0;
}

Connection::Connection(Session &session, const api::Address &address) :
    session_(session),
    transactionid_(0),
    online_(false)
{
    socket_.Connect(address);
    session_.InsertConnection(this);

    new _H<api::Thread>(hop::BindAll(&Connection::Handle_, this));
}

Connection::~Connection() {
    SendLine(_B("OUT"));
}

Session &Connection::GetSession() {
    return session_;
}

bool Connection::IsOnline() const {
    return *online_;
}

_R<Transaction> Connection::Send(const cse::String &command) {
    return Send(command, _L<cse::String>());
}

_R<Transaction> Connection::Send(const cse::String &command, const _L<cse::String> &args) {
    _S<ios::String> line;

    _R<Transaction> transaction(new _H<Transaction>());

    unsigned transactionid;
    _synchronized (transactionlock_) {
        transactionid = ++transactionid_;
        transactions_.Insert(transactionid, transaction);
    }

    line << command << " " << transactionid;

    _foreach (const _L<cse::String>, arg, args)
        line << " " << *arg;

    SendLine(line);

    return transaction;
}

void Connection::SendLine(const cse::String &line) {
    api::Cout << "< " << line << ios::NewLine;

    _synchronized (socketlock_) {
        socket_.WriteFully(line.GetData().Begin(), line.GetData().GetSize());
        socket_.WriteFully("\r\n", 2);
    }
}

void Connection::Wait() {
    _synchronized (online_)
        while (!*online_)
            online_.Wait();
}

void Connection::Ping() {
    SendLine("PING");
}

} }
