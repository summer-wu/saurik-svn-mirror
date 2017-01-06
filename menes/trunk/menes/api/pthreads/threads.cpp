/* Menes - C++ High-Level Utility Library
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

#include "api/pthreads/error.hpp"
#include "api/pthreads/threadlocal.hpp"
#include "api/pthreads/threads.hpp"

#include "app/entrypoint.hpp"

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/singleton.hpp"
#include "cse/string.hpp"

#include "log/logging.hpp"

#include <cstdio>

namespace api {
namespace Pthreads {

namespace {

struct StartInfo_ {
    _R<Thread> thread_;
    Thread::Function function_;

    StartInfo_(Thread *thread, const Thread::Function &function) :
        thread_(thread),
        function_(function)
    {
    }
};

struct ThreadInfo_ {
    ThreadLocal<_R<Thread>, true, ext::ConstructorAllocator> current_;
};

static void *ThreadStart_(void *arg) {
    ::pthread_t id(::pthread_self());
    LOG_THREAD() << "Thread #" << id << ": Starting." << ios::NewLine;

    ext::Pointer< StartInfo_, ext::ObjectAllocator<> > start(reinterpret_cast<StartInfo_ *>(arg), true);
    _1<ThreadInfo_>().current_ = new _R<Thread>(start->thread_);

    void *result(reinterpret_cast<void *>(app::be::ThreadMain_(start->function_)));
    LOG_THREAD() << "Thread #" << id << ": Stopping." << ios::NewLine;
    return result;
}

}

namespace {
    ext::ListMap<Thread::Identifier, cse::String> names_;
}

Thread::Thread(const Identifier &id) :
    id_(id)
{
}

Thread::Thread(const Function &function) :
    joinable_(true)
{
    _R<Thread> this_(this);

    ext::Pointer< StartInfo_, ext::ObjectAllocator<> > info(new StartInfo_(this, function), true);

    int err(::pthread_create(&id_, NULL, &ThreadStart_, info));
    if (err != 0)
        throw Error(err);

    info.Release();
}

Thread::~Thread() {
    if (joinable_) {
        int err(::pthread_detach(id_));
        if (err != 0)
            throw Error(err);
    }
}

Thread::Identifier Thread::GetId() const {
    return id_;
}

void Thread::GetTimingInfo(uint64_t &kernel, uint64_t &user) const {
    kernel = user = 0;
}

void Thread::Join() const {
    // XXX: thread return value?
    int err(::pthread_join(id_, NULL));
    if (err != 0)
        throw Error(err);
    joinable_ = false;
}

cse::String Thread::GetName() const {
    return names_[id_];
}

void Thread::SetName(const char *name) {
    names_[id_] = name;
}

Thread &Thread::Current() {
    ThreadInfo_ &info(_1<ThreadInfo_>());

    if (_R<Thread> *current = info.current_)
        return **current;

    _R<Thread> current(new _H<Thread>(::pthread_self()));
    info.current_ = new _R<Thread>(current);
    return *current;
}

} }
