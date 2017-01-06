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

#include "hop/bind.hpp"

#include "net/oscar/families.hpp"
#include "net/oscar/session.hpp"
#include "net/oscar/userinfo.hpp"

#include "net/oscar/tools/buddy.hpp"

namespace net {
namespace Oscar {

BuddyTool::BuddyTool(Session &session) :
    Tool(session, 0x0110, 0x047b, 0x0003),
    session_(session)
{
}

bool BuddyTool::Process(FlapStream &flaps, const SnacHeader &snac, ios::Buffer &segment) {
    _assert(snac.family == GetFamily());

    switch (snac.subtype) {
        // >> Buddy Online
        case 0x000b: {
            while (!segment.IsEmpty()) {
                UserInfo user(segment);
                if (session_.GetDebugging())
                    api::Cout << "  Online: " << user.screenname << ios::NewLine;
                session_.Notify(hop::BindAll(OnOnline, user));
            }
        } break;

        // >> Buddy Offline
        case 0x000c: {
            UserInfo user(segment);
            if (session_.GetDebugging())
                api::Cout << "  Offline: " << user.screenname << ios::NewLine;
            session_.Notify(hop::BindAll(OnOffline, user));
        } break;

        default:
            return false;
        break;
    }

    return true;
}

void BuddyTool::Insert(const cse::String &screenname) {
    StringSet buddies;
    buddies.Insert(screenname);
    return Insert(buddies);
}

void BuddyTool::Insert(const StringSet &buddies) {
    _S<ios::Buffer> segment;

    for (StringSet::ConstIterator buddy(buddies.Begin()); buddy != buddies.End(); ++buddy) {
        ios::PutBig<uint8_t>(segment, buddy->GetData().GetSize());
        segment.Write(buddy->GetData().Begin(), buddy->GetData().GetSize());
    }

    session_.Queue(SnacPacket(GetFamily(), 0x0004, segment));
}

void BuddyTool::Remove(const cse::String &screenname) {
    _S<ios::Buffer> segment;
    ios::PutBig<uint8_t>(segment, screenname.GetData().GetSize());
    segment.Write(screenname.GetData().Begin(), screenname.GetData().GetSize());
    session_.Queue(SnacPacket(GetFamily(), 0x0005, segment));
}

} }
