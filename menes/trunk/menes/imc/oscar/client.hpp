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

#ifndef MENES_XXX_FILE_HPP
#define MENES_XXX_FILE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "net/oscar/tools/auth.hpp"
#include "net/oscar/tools/buddy.hpp"
#include "net/oscar/tools/chat.hpp"
#include "net/oscar/tools/icbm.hpp"
#include "net/oscar/tools/info.hpp"

namespace imc {
namespace Oscar {

class Client {
  private:
    net::Oscar::Session session_;

    _S<net::Oscar::AuthTool> auth_;
    _S<net::Oscar::BuddyTool> buddy_;
    _S<net::Oscar::ChatTool> chat_;
    _S<net::Oscar::IcbmTool> icbm_;
    _S<net::Oscar::InfoTool> info_;

  public:
    hop::Delegate<void(const cse::String &)> OnBuddyOnline;
    hop::Delegate<void(const cse::String &, const cse::String &)> OnMessageReceive;

  private:
    void OnBuddyOnline_(const net::Oscar::UserInfo &user) {
        OnBuddyOnline(user.screenname);
    }

    void OnIcbmReceive_(const cse::String &screenname, const cse::String &message) {
        OnMessageReceive(screenname, message);
    }

  public:
    Client(const cse::String &username, const cse::String &password) :
        auth_(session_),
        buddy_(session_),
        chat_(session_),
        icbm_(session_),
        info_(session_)
    {
        buddy_.OnOnline += hop::Bind<0>(&Client::OnBuddyOnline_, this);
        icbm_.OnReceive += hop::Bind<0>(&Client::OnIcbmReceive_, this);

        auth_.Login(username, password);

        net::Oscar::IcbmParams params(icbm_.GetParams());
        params.maxmsglen_ = 8000;
        params.minmsginterval_ = 0;
        icbm_.SetParams(params);

        net::Oscar::Capabilities capabilities;
        capabilities.Insert(net::Oscar::ChatCapability);
        info_.SetProfile("bob", "ate", capabilities);
    }

    void AddBuddy(const cse::String &username) {
        buddy_.Insert(username);
    }

    void AddBuddies(const net::Oscar::StringSet &usernames) {
        buddy_.Insert(usernames);
    }

    void SendMessage(const cse::String &username, const cse::String &message) {
        icbm_.Simple(username, message);
    }

    cse::String GetAwayMessage(const cse::String &username) {
        return info_.GetAwayMessage(username);
    }
};

} }

#endif//MENES_XXX_FILE_HPP
