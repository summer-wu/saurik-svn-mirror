/* Oscar - Multi-Threaded Implementation of OSCAR
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "enc/hash/md5.hpp"

#include "ios/filter.hpp"
#include "ios/helpers.hpp"

#include "net/oscar/session.hpp"
#include "net/oscar/families.hpp"

#include "net/oscar/tools/auth.hpp"

namespace net {
namespace Oscar {

cse::String LoginException::GetMessage() const {
    switch (code_) {
        case 0x0001: return _B("Invalid nick or password");
        case 0x0002: return _B("Service temporarily unavailable");
        case 0x0003: return _B("All other errors");
        case 0x0004: return _B("Incorrect nick or password, re-enter");
        case 0x0005: return _B("Mismatch nick or password, re-enter");
        case 0x0006: return _B("Internal client error (bad input to authorizer)");
        case 0x0007: return _B("Invalid account");
        case 0x0008: return _B("Deleted account");
        case 0x0009: return _B("Expired account");
        case 0x000A: return _B("No access to database");
        case 0x000B: return _B("No access to resolver");
        case 0x000C: return _B("Invalid database fields");
        case 0x000D: return _B("Bad database status");
        case 0x000E: return _B("Bad resolver status");
        case 0x000F: return _B("Internal error");
        case 0x0010: return _B("Service temporarily offline");
        case 0x0011: return _B("Suspended account");
        case 0x0012: return _B("DB send error");
        case 0x0013: return _B("DB link error");
        case 0x0014: return _B("Reservation map error");
        case 0x0015: return _B("Reservation link error");
        case 0x0016: return _B("The users num connected from this IP has reached the maximum");
        case 0x0017: return _B("The users num connected from this IP has reached the maximum (reservation)");
        case 0x0018: return _B("Rate limit exceeded (reservation). Please try to reconnect in a few minutes");
        case 0x0019: return _B("User too heavily warned");
        case 0x001A: return _B("Reservation timeout");
        case 0x001B: return _B("You are using an older version of ICQ. Upgrade required");
        case 0x001C: return _B("You are using an older version of ICQ. Upgrade recommended");
        case 0x001D: return _B("Rate limit exceeded. Please try to reconnect in a few minutes");
        case 0x001E: return _B("Can't register on the ICQ network. Reconnect in a few minutes");
        case 0x0020: return _B("Invalid SecurID");
        case 0x0022: return _B("Account suspended because of your age (age < 13)");

        default:
            return cse::String();
    }
}

AuthTool::AuthTool(Session &session) :
    Tool(session, 0x0110, 0x047b, 0x0017),
    session_(session)
{
}

bool AuthTool::Process(FlapStream &flaps, const SnacHeader &snac, ios::Buffer &segment) {
    _assert(snac.family == GetFamily());

    switch (snac.subtype) {
        default:
            return false;
        break;
    }

    return true;
}

void AuthTool::Login(const cse::String &screenname, const cse::String &password) {
    TlvParser tlv;
    SnacPacket reply;

    screenname_ = screenname;
    password_ = password;

    //session.Connect("127.0.0.1:5190");
    session_.Connect("64.12.161.153:5190");
    //session.Connect("login.oscar.aol.com:5190");

    tlv.Clear();
    tlv[0x0001] = screenname_;

    reply = session_.Query(SnacPacket(GetFamily(), 0x0006, tlv.operator _S<ios::Buffer>()));
    _assert(reply.snac.family == GetFamily());
    _assert(reply.snac.subtype == 0x0007);

    uint16_t length(ios::GetBig<uint16_t>(reply.data));
    cse::String key(reply.data.GetIterator(), length);

    tlv.Clear();
    tlv[0x0001] = screenname_;
    tlv[0x0003] = "AOL Instant Messenger (SM), version 3.5.1670/WIN32";
    tlv[0x0016] = uint16_t(0x0004); // ClientID
    tlv[0x0017] = uint16_t(0x0003); // Major
    tlv[0x0018] = uint16_t(0x0005); // Minor
    tlv[0x0019] = uint16_t(0x0000); // Point
    tlv[0x001a] = uint16_t(0x0686); // Build
    tlv[0x000e] = "us"; // Country
    tlv[0x000f] = "en"; // Language
    tlv[0x0009] = uint16_t(0x0015);

    tlv[0x0025] = ios::Filter<enc::Md5Hasher>(_S<ios::Buffer>() << key << password_ << "AOL Instant Messenger (SM)");

    reply = session_.Query(SnacPacket(GetFamily(), 0x0002, tlv.operator _S<ios::Buffer>()));
    _assert(reply.snac.family == GetFamily());
    _assert(reply.snac.subtype == 0x0003);

    tlv.ReadFrom(reply.data);
    if (!tlv[0x0008].IsEmpty())
        throw LoginException(tlv[0x0008], tlv[0x0004]);
    else {
        if (session_.GetDebugging())
            api::Cout << tlv[0x0005].operator cse::String() << ios::NewLine;
        session_.Connect(tlv[0x0005], tlv[0x0006].operator _S<ios::Buffer>());
    }
}

} }
