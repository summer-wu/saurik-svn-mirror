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

#include "net/msn/notification.hpp"
#include "net/msn/session.hpp"
#include "net/msn/switchboard.hpp"
#include "net/msn/transaction.hpp"

namespace net {
namespace MsnMessenger {

Session::Session(const cse::String &email, const cse::String &password) :
    email_(email),
    password_(password),
    online_(false)
{
    _L<api::Address> addresses(api::Address::Resolve(_B("messenger.hotmail.com"), _B("1863")));
    _assert(!addresses.IsEmpty());
    new _H<NotificationServer>(*this, addresses[0]);
}

void Session::InsertConnection(const _R<Connection> &connection) {
    _synchronized (connections_) {
        (*connections_)[&typeid(connection.GetValue())].Insert(connection);
        connections_.Broadcast();
    }
}

void Session::RemoveConnection(const _R<Connection> &connection) {
    _synchronized (connections_)
        (*connections_)[&typeid(connection.GetValue())].Remove(connection);
}

void Session::UpdateConnections() {
    _synchronized (connections_)
        connections_.Broadcast();
}

_R<Connection> Session::GetConnection(const std::type_info &type) {
    _synchronized (connections_)
        for (;; connections_.Wait())
            _foreach (const ConnectionSet_, connection, (*connections_)[&type])
                if ((*connection)->IsOnline())
                    return *connection;
    _assert(false);
    return _R<Connection>();
}

_R<NotificationServer> Session::GetNotificationServer() {
    return dynamic_cast<NotificationServer *>(GetConnection(typeid(NotificationServer)).GetValue());
}

_R<SwitchBoard> Session::NewSwitchBoard() {
    return dynamic_cast<SwitchBoard *>(GetNotificationServer()->Send(_B("XFR"), _Vs(_B("SB")))->GetValue().GetValue());
}

const cse::String &Session::GetEmail() const {
    return email_;
}

} }
