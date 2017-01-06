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
#include "net/oscar/protocol.hpp"
#include "net/oscar/session.hpp"

#include "net/oscar/tools/auth.hpp"
#include "net/oscar/tools/buddy.hpp"
#include "net/oscar/tools/chat.hpp"
#include "net/oscar/tools/icbm.hpp"
#include "net/oscar/tools/info.hpp"
#include "net/oscar/tools/ssi.hpp"

namespace net {
namespace Oscar {

void Session::Unqueue() {
    PacketList_::iterator packet(oldpackets.begin());
    while (packet != oldpackets.end()) {
        const Connections_ &family(families_[packet->snac.family]);
        if (family.IsEmpty())
            ++packet;
        else {
            FlapStream &flap(**family.Begin());

            if (debugging_)
                _synchronized (lock_) {
                    api::Cout << ios::SetBase(16) << "Collect [Family: " << packet->snac.family << ", Type: " << packet->snac.subtype << "]" << ios::NewLine << ios::SetBase(10);
                    api::Cout << " Using: " << flap.GetAddress() << ios::NewLine;
                }

            if (flap.Write(*packet))
                oldpackets.erase(packet++);
            else
                ++packet;
        }
    }
}

int Session::Dispatch_() {
    api::Thread::Current().SetName("Oscar:Dispatch");

    _forever {
        SnacPacket packet;

        _synchronized (dispatch_mutex) {
            if (newpackets.empty())
                dispatch_cond.Wait();
            packet = newpackets.front();
            newpackets.pop();
        }

        if (debugging_)
            _synchronized (lock_)
                api::Cout << ios::SetBase(16) << "Dispatch [Family: " << packet.snac.family << ", Type: " << packet.snac.subtype << ", Request: " << packet.snac.request << "]" << ios::NewLine << ios::SetBase(10);

        bool queued(false);

        _synchronized (family_mutex) {
            const Connections_ &family(families_[packet.snac.family]);
            if (family.IsEmpty()) {
                if (debugging_)
                    _synchronized (lock_)
                        api::Cout << " ReQueueing" << ios::NewLine;
            } else {
                FlapStream &flap(**family.Begin());
                if (debugging_)
                    _synchronized (lock_)
                        api::Cout << " Using: " << flap.GetAddress() << ios::NewLine;
                queued = flap.Write(packet);
            }
        }

        if (!queued)
            _synchronized (collect_mutex) {
                oldpackets.push_back(packet);
                collect_cond.Signal();
            }
    }

    return 0;
}

int Session::Collect_() {
    api::Thread::Current().SetName("Oscar:Collect");

    for (;;)
        _synchronized (collect_mutex) {
            while (oldpackets.empty())
                collect_cond.Wait();

            _synchronized (family_mutex) {
                Unqueue();

                while (!oldpackets.empty()) {
                    _desynchronized (collect_mutex)
                        family_cond.Wait();
                    Unqueue();
                }
            }
        }

    return 0;
}

int Session::Notify_() {
    api::Thread::Current().SetName("Oscar:Notify");

    for (;;) {
        Event_ event;

        _synchronized (eventlock_) {
            while (events_.IsEmpty())
                eventcond_.Wait();
            event = events_.First();
            events_.RemoveFirst();
        }

        event();
    }

    return 0;
}

Session::Session() :
    eventcond_(eventlock_),
    family_cond(family_mutex),
    dispatch_cond(dispatch_mutex),
    collect_cond(collect_mutex),
    debugging_(false)
{
    dispatch_thread = new _H<api::Thread>(hop::BindAll(&Session::Dispatch_, this));
    collect_thread = new _H<api::Thread>(hop::BindAll(&Session::Collect_, this));
    notify_ = new _H<api::Thread>(hop::BindAll(&Session::Notify_, this));
}

Session::~Session() {
    handle_threads.Join();
    collect_thread->Join();
    dispatch_thread->Join();
}

int Session::Handle(const cse::String &address, const ios::Buffer &cookie) {
    FlapStream conn(*this, address, cookie);

    connections_.Insert(&conn);

    conn.Handle();
    Remove(conn);
    return 0;
}

void Session::Process(FlapStream &flaps, const SnacHeader &header, ios::Buffer &packet) {
    _synchronized (requestlock_) {
        Requests_::Iterator request(requests_.Find(header.request));
        if (request != requests_.End()) {
            if (debugging_)
                api::Cout << "  Returning..." << ios::NewLine;

            request->Second().reply_ = SnacPacket(header, packet);

            _synchronized (request->Second().eventl_)
                request->Second().event_.Signal();

            return;
        }
    }

    ToolVersion tool(GetTool(header.family));
    if (tool.Second() == NULL)
        api::Cout << "  Missing Tool" << ios::NewLine;
    else if (!tool.Second()->Process(flaps, header, packet))
        api::Cout << "  Failed Process" << ios::NewLine;
}

void Session::Connect(const cse::String &address, const ios::Buffer &cookie) {
    // XXX: that sucked...
    handle_threads.Add(hop::BindAll(&Session::Handle, this, address, _S<ios::Buffer>(cookie)));
}

void Session::Queue(const SnacPacket &packet) {
    _synchronized (dispatch_mutex) {
        newpackets.push(packet);
        dispatch_cond.Signal();
    }
}

SnacPacket Session::Query(const SnacPacket &packet) {
    SnacPacket reply;

    _synchronized (requestlock_) {
        Request_ &request(requests_[packet.snac.request]);
        Queue(packet);

        _desynchronized (requestlock_)
            _synchronized (request.eventl_)
                request.event_.Wait();

        reply = request.reply_;
        requests_.Remove(packet.snac.request);
    }

    return reply;
}

void Session::Notify(const Event_ &event) {
    _synchronized (eventlock_) {
        events_.InsertLast(event);
        eventcond_.Signal();
    }
}

void Session::Setup(FlapStream &flaps) {
    _synchronized (family_mutex) {
        const FamilySet &families(flaps.GetFamilies());
        for (FamilySet::ConstIterator family(families.Begin()); family != families.End(); ++family)
            families_[*family].Insert(&flaps);
        family_cond.Signal();
    }
}

void Session::Setup(FlapStream &flaps, uint16_t family) {
    _synchronized (family_mutex) {
        families_[family].Insert(&flaps);
        family_cond.Signal();
    }
}

void Session::Remove(FlapStream &flaps) {
    _synchronized (family_mutex) {
        const FamilySet &families(flaps.GetFamilies());
        for (FamilySet::ConstIterator family(families.Begin()); family != families.End(); ++family) {
            if (debugging_)
                api::Cout << &families_[*family] << ios::NewLine;
            families_[*family].Remove(&flaps);
        }

        connections_.Remove(&flaps);
    }
}

void Session::SetTool(uint16_t family, uint16_t version, const _R<Tool> &tool) {
    _synchronized (toollock_) {
        bool inserted(tools_.Insert(family, ToolVersion(version, tool)).Second());
        _assert(inserted);
    }
}

ToolVersion Session::GetTool(uint16_t family) {
    _synchronized (toollock_) {
      //find:
        Tools_::ConstIterator tool(tools_.Find(family));
        if (tool != tools_.End())
            return tool->Second();
        /*else switch (family) {
            case AuthorizationFamily:
                SetTool(family, 0x0001, new AuthTool(*this));
            goto find;

            case BuddyListFamily:
                SetTool(family, 0x0001, new BuddyTool(*this));
            goto find;

            case ChatFamily:
                SetTool(family, 0x0001, new ChatTool(*this));
            goto find;

            case IcbmFamily:
                SetTool(family, 0x0001, new IcbmTool(*this));
            goto find;

            case InformationFamily:
                SetTool(family, 0x0001, new InfoTool(*this));
            goto find;

            case ServerSideInformationFamily:
                SetTool(family, 0x0001, new SsiTool(*this));
            goto find;
        }*/
    }

    return ToolVersion(0x0000, _R<Tool>());
}

api::ThreadMutex &Session::GetLock() {
    return lock_;
}

bool Session::GetDebugging() const {
    return debugging_;
}

} }
