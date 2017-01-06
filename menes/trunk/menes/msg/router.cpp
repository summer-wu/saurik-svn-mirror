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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/random.hpp"
#include "api/uuid.hpp"

#include "hop/bind.hpp"
#include "ext/uuid.hpp"
#include "ios/stdadapters.hpp"

#include "msg/object.hpp"
#include "msg/remote.hpp"
#include "msg/router.hpp"

#include "opt/reports.hpp"

#include "szf/binary.hpp"
#include "szf/saveload-ios.hpp"

namespace msg {

static const bool Delay_(false);

int Router::Route() {
    api::Thread::Current().SetName("Syllego:Router");
    opt::initialize("Syllego:Router");

    api::InternetAddress remote;

    while (online_) {
        _S<ios::Buffer> packet;

        try {
            sock_->Read(packet, &remote);
#ifdef _WIN32
        } catch (const api::Winsock::Error &e) {
            if (e.GetCode() == WSAECONNRESET)
                continue;
            // XXX: this code bounces between here and msg::Router::Route()
            if (e.GetCode() == WSAEINTR)
                return 0;
            return -1;
#endif
        } _catch {
            return -1;
        }

        objid_t id;
        _S<ios::Buffer> message;

        {
            _S<szf::BinaryMethod> method(&packet, _null<ios::Writer>());
            szf::Load(&method, id, "@id");
            szf::Load(&method, message, "message");
        }

        Delay_ ? delays_.push(Delayed_(id, message)) : Dispatch(id, message);
    }

    return 0;
}

int Router::Delay() {
    api::Thread::Current().SetName("Syllego:Dispatch");
    opt::initialize("Syllego:Dispatch");

    for (Delayed_ delay; delays_.pop(delay); Dispatch(delay.id, delay.msg));
    return 0;
}

void Router::Init(Address::Port port) {
    sock_ = new _H<api::UdpSocket>();
    sock_->SetLocalAddress(Address(Address::Any, port));

    delay_ = new _H<api::Thread>(hop::BindAll(&Router::Delay, this));
    route_ = new _H<api::Thread>(hop::BindAll(&Router::Route, this));
}

objid_t Router::Insert(Object &obj) {
    objid_t id;
    do id = api::Uuid::CreateSequential();
    while (!Insert(id, obj));
    return id;
}

bool Router::Insert(objid_t id, Object &obj) {
    ext::ScopeLock<api::ThreadMutex> objguard(objlock_);
    return objects_.Insert(id, &obj).Second();
}

void Router::Erase(objid_t id) {
    ext::ScopeLock<api::ThreadMutex> objguard(objlock_);
    bool erased(objects_.Remove(id));
    _assert(erased);
}

Router::Router() :
    online_(true)
{
    // XXX: this is ludicrously pathetic
    short port;
    ios::ReadFully(api::WeakRandom, reinterpret_cast<byte_t *>(&port), sizeof(port));
}

Router::Router(Address::Port port) :
    online_(true)
{
    Init(port);
}

Router::~Router() {
    online_ = false;
    /* // linux is broken
    sock_->sdsend();*/
    // unix is extra broken
    sock_.Clear();
    route_->Join();

    delays_.close();
    delay_->Join();
}

Router::Address Router::GetAddress() const {
    return sock_->GetLocalAddress();
}

Handle Router::Proxy(objid_t id, const Address &addr) {
    if (addr == GetAddress()) {
        Object *object(objects_[id]);
        _assert(object != NULL);
        return object;
    }

    return new _H<Remote>(*this, id, addr);
}

void Router::Dispatch(objid_t id, const MessageBuffer &message) {
    //LOG_THREAD() << "Router::Dispatch(" << id << ")" << ios::NewLine;
    ext::ScopeLock<api::ThreadMutex> objguard(objlock_);
    ObjectMap_::Iterator obj(objects_.Find(id));
    if (obj != objects_.End())
        (*obj->Second())->*message;
    else _assert(false);
}

void Router::Dispatch(objid_t id, const MessageBuffer &message, const Address &addr) {
    if (addr == GetAddress())
        return Dispatch(id, message);

    _S<ios::Buffer> packet;

    {
        _S<szf::BinaryMethod> method(_null<ios::Reader>(), &packet);
        szf::Save(&method, id, "@id");
        szf::Save(&method, message, "message");
    }

    // XXX: this used to be more efficient
    sock_->Write(packet.Begin(), packet.GetSize(), &addr);
}

}
