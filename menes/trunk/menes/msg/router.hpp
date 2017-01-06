/* Menes Messaging - Remote Method Call for C++
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *·
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
 *·
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

#ifndef MENES_MSG_ROUTER_HPP
#define MENES_MSG_ROUTER_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/locks.hpp"
#include "api/socket.hpp"
#include "api/threads.hpp"

#include "ext/redblackset.hpp"

#include "msg/handle.hpp"
#include "msg/linkage.hpp"
#include "msg/message.hpp"
#include "msg/types.hpp"

#include "mta/tqueue.hpp"

namespace msg {

class Object;

class MSG_DECLARE Router {
    friend class Object;

  public:
    typedef api::InternetAddress Address;

  private:
    _R<api::UdpSocket> sock_;
    volatile bool online_;

    typedef ext::RedBlackMap<objid_t, Object *> ObjectMap_;
    ObjectMap_ objects_;

    api::ThreadMutex objlock_;

    struct Delayed_ {
        objid_t id;
        _S<MessageBuffer> msg;

        Delayed_() {
        }

        Delayed_(objid_t id, const MessageBuffer &msg) :
            id(id), msg(msg)
        {
        }
    };

    typedef menes::itqueue<Delayed_> DelayQueue_;
    DelayQueue_ delays_;

    _R<api::Thread> route_;
    int Route();

    _R<api::Thread> delay_;
    int Delay();

    void Init(Address::Port port);

  protected:
    objid_t Insert(Object &obj);
    bool Insert(objid_t id, Object &obj);
    void Erase(objid_t id);

  public:
    Router();
    explicit Router(Address::Port port);
    ~Router();

    Address GetAddress() const;

    Handle Proxy(objid_t id, const Address &addr);

    void Dispatch(objid_t id, const MessageBuffer &msg);
    void Dispatch(objid_t id, const MessageBuffer &msg, const Address &addr);
};

}

#endif//MENES_MSG_ROUTER_HPP
