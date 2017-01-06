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

#include "net/oscar/session.hpp"
#include "net/oscar/families.hpp"

#include "net/oscar/tools/ssi.hpp"

namespace net {
namespace Oscar {

SsiTool::SsiTool(Session &session) :
    Tool(session, 0x0110, 0x047b, 0x0013),
    session_(session)
{
}

bool SsiTool::Process(FlapStream &flaps, const SnacHeader &snac, ios::Buffer &segment) {
    _assert(snac.family == GetFamily());

    switch (snac.subtype) {
        // >> Rights Response
        case 0x0003: {
            // XXX: figure this out
        } break;

        // >> Data Response
        case 0x0006: {
            uint8_t version(ios::GetBig<uint8_t>(segment));
            _assert(version == 0x00);

            uint16_t revision(ios::GetBig<uint16_t>(segment));
            while (segment.GetSize() > sizeof(uint32_t)) {
                cse::String name(segment.GetIterator(), ios::GetBig<uint16_t>(segment));

                uint16_t gid(ios::GetBig<uint16_t>(segment));
                uint16_t bid(ios::GetBig<uint16_t>(segment));
                uint16_t type(ios::GetBig<uint16_t>(segment));

                if (session_.GetDebugging())
                    api::Cout << "  " << name << " [gid: " << gid << ", bid: " << bid << " Type: " << type << "]" << ios::NewLine;

                _S<ios::Buffer> extra(segment.GetIterator(), ios::GetBig<uint16_t>(segment));
                TlvParser tlv(extra);

                if (session_.GetDebugging())
                    api::Cout << tlv << ios::NewLine;
            }

            uint32_t timestamp(ios::GetBig<uint32_t>(segment));
            if (session_.GetDebugging())
                api::Cout << "  TimeStamp: " << timestamp << ios::NewLine;
        } break;

        default:
            return false;
        break;
    }

    return true;
}

void SsiTool::req_rights() {
    session_.Queue(SnacPacket(GetFamily(), 0x0002));
}

void SsiTool::req_data(uint32_t timestamp, int16_t revision) {
    _S<ios::Buffer> segment;
    ios::PutBig<uint32_t>(segment, timestamp);
    ios::PutBig<uint16_t>(segment, revision);
    session_.Queue(SnacPacket(GetFamily(), 0x0005, segment));
}

} }
