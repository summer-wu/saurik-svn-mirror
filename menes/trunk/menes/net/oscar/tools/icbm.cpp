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

#include "api/random.hpp"
#include "hop/bind.hpp"

#include "net/oscar/families.hpp"
#include "net/oscar/protocol.hpp"
#include "net/oscar/session.hpp"
#include "net/oscar/userinfo.hpp"

#include "net/oscar/tools/icbm.hpp"

#include <algorithm>
#include <string>

namespace net {
namespace Oscar {

std::string collapse(std::string screenname) {
    std::transform(screenname.begin(), screenname.end(), screenname.begin(), tolower);
    for (size_t space; (space = screenname.find_first_of(' ')) != std::string::npos; screenname.erase(space, 1));
    return screenname;
}

IcbmTool::IcbmTool(Session &session) :
    Tool(session, 0x0110, 0x047b, 0x0004),
    session_(session)
{
}

bool IcbmTool::Process(FlapStream &flaps, const SnacHeader &snac, ios::Buffer &segment) {
    _assert(snac.family == GetFamily());

    switch (snac.subtype) {
        // >> Incoming ICBM
        case 0x0007: {
            _S<ios::Buffer> cookie(segment.GetIterator(), 8);
            uint16_t channel(ios::GetBig<uint16_t>(segment));
            UserInfo user(segment);

            if (session_.GetDebugging())
                api::Cout << "  ICBM [Channel: " << channel << ", User: '" << user.screenname << "']" << ios::NewLine;

            if (channel == 1) {
                TlvParser tlv(segment);
                TlvParser fragment(tlv[0x0002]);

                Capabilities features(fragment[0x0501]);
                // XXX: this makes me sad
                _S<ios::Buffer> message(fragment[0x0101].operator _S<ios::Buffer>());
                _assert(!message.IsEmpty());

                uint16_t charset(ios::GetBig<uint16_t>(message));
                uint16_t subset(ios::GetBig<uint16_t>(message));

                if (session_.GetDebugging())
                    api::Cout << ios::SetBase(16) << "   Charset: " << charset << ", Subset: " << subset << ios::NewLine << ios::SetBase(10);

                // XXX: this isn't charset aware
                cse::String line(message);

                if (session_.GetDebugging())
                    api::Cout << "   Message: " << line << ios::NewLine;

                session_.Notify(hop::BindAll(OnReceive, user.screenname, line));
            }
        } break;

        default:
            return false;
        break;
    }

    return true;
}

IcbmParams IcbmTool::GetParams() {
    SnacPacket reply(session_.Query(SnacPacket(GetFamily(), 0x0004)));
    _assert(reply.snac.family == GetFamily());
    _assert(reply.snac.subtype == 0x0005);

    return IcbmParams(reply.data);
}

void IcbmTool::SetParams(const IcbmParams &params) {
    _S<ios::Buffer> segment;
    params.WriteTo(segment);
    session_.Queue(SnacPacket(GetFamily(), 0x0002, segment));
}

void IcbmTool::Send(const cse::String &screenname, uint16_t channel, uint16_t type, const ios::Buffer &data) {
    _S<ios::Buffer> segment;

    // Message Cookie
    char cookie[8];
    ios::ReadFully(api::WeakRandom, cookie, sizeof(cookie));
    segment.WriteFully(cookie, sizeof(cookie));

    // Channel ID
    ios::PutBig<uint16_t>(segment, channel);

    // Screen Name
    ios::PutBig<uint8_t>(segment, screenname.GetData().GetSize());
    segment.Write(screenname.GetData().Begin(), screenname.GetData().GetSize());

    TlvParser tlv;
    tlv[type] = data;
    tlv.WriteTo(segment);

    session_.Queue(SnacPacket(GetFamily(), 0x0006, segment));
}

void IcbmTool::Simple(const cse::String &screenname, const cse::String &text) {
    _S<ios::Buffer> segment;

    // Features
    ios::PutBig<uint16_t>(segment, 0x0501); // Magic
    ios::PutBig<uint16_t>(segment, 4);
    ios::PutBig<uint32_t>(segment, 0x01010102);

    // Message Block
    ios::PutBig<uint16_t>(segment, 0x0101); // Magic
    ios::PutBig<uint16_t>(segment, static_cast<uint16_t>(text.GetData().GetSize() + 4));
    ios::PutBig<uint16_t>(segment, 0x0000); // Encoding
    ios::PutBig<uint16_t>(segment, 0x0000); // Sub-Encoding
    segment.Write(text.GetData().Begin(), text.GetData().GetSize());

    Send(screenname, 0x0001, 0x0002, segment);
}

void IcbmTool::Special(const cse::String &screenname, const Capability &capability, const ios::Buffer &data) {
    uint8_t cookie[8];

    _S<ios::Buffer> segment;

    ios::PutBig<uint16_t>(segment, 0x0000); // from invite()
    segment.Write(reinterpret_cast<const char *>(cookie), sizeof(cookie));

    // XXX: this might need to take into account endian-ness... no?
    segment.Write(reinterpret_cast<const char *>(capability.GetData()), sizeof(Capability));

    data.WriteTo(segment);

    Send(screenname, 0x0002, 0x0005, segment);
}

void addroom(TlvParser &tlv, uint16_t type, uint16_t exchange, const cse::String &room, uint16_t instance) {
    _S<ios::Buffer> segment;

    ios::PutBig<uint16_t>(segment, exchange);
    ios::PutBig<uint8_t>(segment, room.GetSize());
    segment.Write(room.GetData().Begin(), room.GetData().GetSize());
    ios::PutBig<uint16_t>(segment, instance);

    tlv[type] = segment;
}

void IcbmTool::Invite(const cse::String &screenname, const cse::String &text, const cse::String &room, uint16_t instance) {
    _S<ios::Buffer> segment;

    TlvParser tlv;
    tlv[0x000a] = uint16_t(0x0001);
    tlv[0x000f].Clear();
    tlv[0x000c] = text;
    //addroom(tlv, 0x2711, exchange, room, instance);

    Special(screenname, ChatCapability, tlv.operator _S<ios::Buffer>());
}

} }
