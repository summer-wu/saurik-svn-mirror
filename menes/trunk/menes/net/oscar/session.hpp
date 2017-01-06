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

#ifndef MENES_NET_OSCAR_SESSION_HPP
#define MENES_NET_OSCAR_SESSION_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/threads.hpp"

#include "ext/listset.hpp"
#include "mta/threadset.hpp"

#include "net/oscar/protocol.hpp"
#include "net/oscar/tool.hpp"

namespace net {
namespace Oscar {

class NET_DECLARE Session {
    friend class FlapStream;

  protected:
    typedef ext::ListSet<FlapStream *> Connections_;
    Connections_ connections_;

    typedef ext::ListMap<uint16_t, Connections_> Families_;
    Families_ families_;

    typedef ext::ListMap<uint16_t, ToolVersion> Tools_;
    Tools_ tools_;
    api::ThreadMutex toollock_;

    typedef std::queue<SnacPacket> PacketQueue_;
    PacketQueue_ newpackets;

    typedef std::list<SnacPacket> PacketList_;
    PacketList_ oldpackets;

    struct Request_ {
        api::ThreadCondition::Mutex eventl_;
        api::ThreadCondition event_;

        SnacPacket reply_;

        Request_() :
            event_(eventl_)
        {
        }
    };

    typedef ext::RedBlackMap<uint32_t, Request_> Requests_;
    Requests_ requests_;
    api::ThreadMutex requestlock_;

    typedef hop::Function<void ()> Event_;
    typedef ext::Vector<Event_> Events_;
    Events_ events_;

    api::ThreadCondition::Mutex eventlock_;
    api::ThreadCondition eventcond_;

    api::ThreadMutex lock_;

    mta::ThreadSet<> handle_threads;

    _R<api::Thread> dispatch_thread;
    _R<api::Thread> collect_thread;
    _R<api::Thread> notify_;

    api::ThreadCondition::Mutex family_mutex;
    api::ThreadCondition family_cond;

    api::ThreadCondition::Mutex dispatch_mutex;
    api::ThreadCondition dispatch_cond;

    api::ThreadCondition::Mutex collect_mutex;
    api::ThreadCondition collect_cond;

    bool debugging_;

    void Unqueue();

    int Dispatch_();
    int Collect_();
    int Notify_();

    int Handle(const cse::String &address, const ios::Buffer &cookie);
    void Process(FlapStream &flaps, const SnacHeader &header, ios::Buffer &packet);

  public:
    Session();
    ~Session();

    void Connect(const cse::String &address, const ios::Buffer &cookie = _S<ios::Buffer>());

    void Queue(const SnacPacket &packet);
    SnacPacket Query(const SnacPacket &packet);

    void Notify(const hop::Function<void ()> &event);

    void Setup(FlapStream &flaps);
    void Setup(FlapStream &flaps, uint16_t family);
    void Remove(FlapStream &flap);

    void SetTool(uint16_t family, uint16_t version, const _R<Tool> &tool);
    ToolVersion GetTool(uint16_t family);

    api::ThreadMutex &GetLock();
    bool GetDebugging() const;
};

} }

#endif//MENES_NET_OSCAR_SESSION_HPP
