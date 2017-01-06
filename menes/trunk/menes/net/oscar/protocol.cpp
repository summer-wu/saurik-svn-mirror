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

#include "net/oscar/protocol.hpp"
#include "net/oscar/session.hpp"

namespace net {
namespace Oscar {

ios::PrintWriter &operator <<(ios::PrintWriter &os, const TlvParser &tlv) {
    os << ios::SetBase(16);
    for (TlvParser::LvMap_::ConstIterator lv(tlv.lvs_.Begin()); lv != tlv.lvs_.End(); ++lv) {
        os << "   " << lv->First() << ":";
        // XXX: this makes me sad
        _S<ios::Buffer> data(lv->Second().operator _S<ios::Buffer>());
        for (size_t i(0); i != data.GetSize(); ++i)
            os << " " << static_cast<uint8_t>(data[i]) << "[" << data[i] << "]";
        os << ios::NewLine;
    }
    os << ios::SetBase(10);

    return os;
}

uint32_t SnacHeader::lastrequest_(0);

FlapStream::FlapStream(Session &session, const cse::String &address, const ios::Buffer &cookie) :
    session_(session),
    address_(address),
    cookie_(cookie)
{
    // XXX: nasty hack
    cse::String::ConstIterator colon(ext::FindFirst(address, ':'));
    cse::String host(address.Begin(), colon);
    cse::String port(colon + 1, address.End());

    if (session_.GetDebugging())
        _synchronized (session_.GetLock())
            api::Cout << host << "[" << port << "]" << ios::NewLine;

    _L<api::Address> addresses(api::Address::Resolve(host, port));
    _assert(addresses.GetSize() == 1);

    socket_.Connect(addresses[0]);
}

const FamilySet &FlapStream::GetFamilies() const {
    return families_;
}

const cse::String &FlapStream::GetAddress() const {
    return address_;
}

void FlapStream::Handle() {
    //api::Thread::Current().Name(("Syllego:In[" + address_ + "]").NullTerminate());

    _forever {
        _S<ios::Buffer> packet;
        uint8_t channel(Read(packet));

        if (session_.GetDebugging())
            _synchronized (session_.GetLock())
                api::Cout << "FLAP [Channel #" << static_cast<int>(channel) << "] " << address_ << ios::NewLine;

        switch (channel) {

            // >> FLAP Version
            case 1: {
                // << Send Cookie
                _S<ios::Buffer> segment;
                ios::PutBig<uint32_t>(segment, 0x00000001);

                if (cookie_.IsEmpty()) {
                    Write(1, segment);
                    session_.Setup(*this, 0x0017);
                } else {
                    if (session_.GetDebugging())
                        _synchronized (session_.GetLock())
                            api::Cout << " Cookie" << ios::NewLine;

                    TlvParser tlv;
                    tlv[0x0006] = cookie_;
                    tlv.WriteTo(segment);
                    Write(1, segment);
                }
            } break;

            case 2: {
                SnacHeader snac(packet);

                if (session_.GetDebugging())
                    _synchronized (session_.GetLock())
                        api::Cout << ios::SetBase(16) << " SNAC [Family: " << snac.family << ", Type: " << snac.subtype << ", Request: "  << snac.request << "]" << ios::NewLine << ios::SetBase(10);

                if (snac.family != Family)
                    session_.Process(*this, snac, packet);
                else switch (snac.subtype) {
                    // >> Server Ready
                    case 0x0003: {
                        // << Set Versions
                        _S<ios::Buffer> segment;

                        // XXX: when not debugging this lock shouldn't happen
                        _synchronized (session_.GetLock()) {
                            if (session_.GetDebugging())
                                api::Cout << ios::SetBase(16) << "  Supporting";

                            while (!packet.IsEmpty()) {
                                uint16_t family(ios::GetBig<uint16_t>(packet));
                                families_.Insert(family);

                                if (session_.GetDebugging())
                                    api::Cout << " " << family;

                                if (family == Family) {
                                    ios::PutBig<uint16_t>(segment, Family);
                                    ios::PutBig<uint16_t>(segment, 0x0003); // verrsion
                                } else {
                                    ToolVersion tool(session_.GetTool(family));
                                    if (tool.Second() != NULL) {
                                        ios::PutBig<uint16_t>(segment, family);
                                        ios::PutBig<uint16_t>(segment, tool.First());
                                    }
                                }
                            }

                            if (session_.GetDebugging())
                                api::Cout << ios::NewLine << ios::SetBase(10);
                        }

                        Write(SnacPacket(Family, 0x0017, segment));
                    } break;

                    // >> Rate Response
                    case 0x0007: {
                        for (uint16_t i(0), number(ios::GetBig<uint16_t>(packet)); i < number; ++i) {
                            RateClass_ &rc(rates_[ios::GetBig<uint16_t>(packet)]);
                            rc.windowsize = ios::GetBig<uint32_t>(packet);
                            rc.clear = ios::GetBig<uint32_t>(packet);
                            rc.alert = ios::GetBig<uint32_t>(packet);
                            rc.limit = ios::GetBig<uint32_t>(packet);
                            rc.disconnect = ios::GetBig<uint32_t>(packet);
                            rc.current = ios::GetBig<uint32_t>(packet);
                            rc.max = ios::GetBig<uint32_t>(packet);
                            packet.Read(rc.unknown, 5);
                        }

                        // << Rate Acknowledge
                        Write(SnacPacket(Family, 0x0008));

                        // << Client Ready
                        _S<ios::Buffer> segment;

                        for (FamilySet::ConstIterator family(families_.Begin()); family != families_.End(); ++family) {
                            ToolVersion tool(session_.GetTool(*family));
                            if (tool.Second() != NULL) {
                                ios::PutBig<uint16_t>(segment, *family);
                                ios::PutBig<uint16_t>(segment, tool.First());
                                ios::PutBig<uint16_t>(segment, tool.Second()->GetId());
                                ios::PutBig<uint16_t>(segment, tool.Second()->GetVersion());
                            }
                        }

                        Write(SnacPacket(Family, 0x0002, segment));
                        session_.Setup(*this);
                    } break;

                    /* // >> Rate Change
                    case 0x000a: {
                    } break;*/

                    // >> Server Pause
                    case 0x000b: {
                        // << Pause Acknowledgement
                        _S<ios::Buffer> segment;
                        for (FamilySet::ConstIterator family(families_.Begin()); family != families_.End(); ++family)
                            ios::PutBig<uint16_t>(segment, *family);
                        Write(SnacPacket(Family, 0x000c, segment));
                    } break;

                    // >> Message of the Day
                    case 0x0013: {
                        uint16_t type(ios::GetBig<uint16_t>(packet));
                        if (type == 5)
                            _synchronized (session_.GetLock())
                                api::Cout << "  MOTD [Type: 5] - Protocol Error" << ios::NewLine;
                        else {
                            TlvParser tlv(packet);
                            cse::String message(tlv[0x000b].operator cse::String());
                        }
                    } break;

                    // >> Host Versions
                    case 0x0018: {
                        // << Rate Request
                        Write(SnacPacket(Family, 0x0006));
                    } break;

                    default:
                        _synchronized (session_.GetLock())
                            api::Cout << "  Unknown SNAC" << ios::NewLine;
                    break;
                }
            } break;

            case 3:
            break;

            case 4:
                return;
            break;
        }
    }
}

} }
