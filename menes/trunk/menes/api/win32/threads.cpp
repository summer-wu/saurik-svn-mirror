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

#include "api/win32/error.hpp"
#include "api/win32/locks.hpp"
#include "api/win32/threadlocal.hpp"
#include "api/win32/threads.hpp"

#include "app/entrypoint.hpp"
#include "ext/singleton.hpp"
#include "log/logging.hpp"

#include <process.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {
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
    ThreadLocal< _R<Thread> > current_;
};

} } }

extern "C" {
static unsigned __stdcall start_thread(void *arg) {
    DWORD id(::GetCurrentThreadId());
    LOG_THREAD() << "Thread #" << id << ": Starting." << ios::NewLine;

    using api::Win32::StartInfo_;
    ext::Pointer< StartInfo_, ext::ObjectAllocator<> > start(reinterpret_cast<StartInfo_ *>(arg));
    _1<api::Win32::ThreadInfo_>().current_ = new _R<api::Win32::Thread>(start->thread_);

    unsigned result(app::be::ThreadMain_(start->function_));
    LOG_THREAD() << "Thread #" << id << ": Stopping." << ios::NewLine;
    return result;
} }

namespace api {
namespace Win32 {

Thread::Thread(HANDLE handle, Identifier id) :
    id_(id)
{
    handle_.Mend(handle);
}

Thread::Thread() :
    id_(0)
{
}

Thread::Thread(const Identifier &id) :
    id_(id)
{
    HANDLE handle(::OpenThread(THREAD_ALL_ACCESS, FALSE, id));
    if (handle == NULL)
        throw Error();
    handle_.Mend(handle);
}

Thread::Thread(const Function &function) {
    ext::Pointer< StartInfo_, ext::ObjectAllocator<> > info(new api::Win32::StartInfo_(this, function));

    handle_.Mend(reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, start_thread, info, 0, reinterpret_cast<unsigned *>(&id_))));
    // XXX: this needs to use errno instead...
    if (handle_.IsEmpty())
        throw Error();

    info.Release();
}

void Thread::Swap(Thread &thread) {
    ext::Swap(id_, thread.id_);
    ext::Swap(handle_, thread.handle_);
}

const Thread::Identifier &Thread::GetId() const {
    return id_;
}

const Handle &Thread::GetHandle() const {
    return handle_;
}

void Thread::GetTimingInfo(uint64_t &kernel, uint64_t &user) const {
    FILETIME fcreation, fexit;
    ::GetThreadTimes(handle_, &fcreation, &fexit, reinterpret_cast<LPFILETIME>(&kernel), reinterpret_cast<LPFILETIME>(&user));
}

void Thread::Join() const {
    if (!handle_.IsEmpty())
        if (WAIT_FAILED == ::WaitForSingleObject(handle_, Infinite))
            throw Error();
}

void Thread::SetName(const char *name) {
    LOG_THREAD() << "Thread #" << id_ << ": Renaming [\"" << name << "\"]." << ios::NewLine;

    struct {
        DWORD   dwType;     // must be 0x1000
        LPCSTR  szName;     // pointer to name (in user addr space)
        DWORD   dwThreadID; // thread ID (-1=caller thread)
        DWORD   dwFlags;    // reserved for future use, must be zero
    } info = {
        0x1000, name, id_, 0
    };

    __try {
        ::RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (DWORD *) &info);
    } __except (EXCEPTION_CONTINUE_EXECUTION) {
    }
}

void Thread::Sleep(unsigned msecs) {
    ::Sleep(msecs);
}

Thread &Thread::Current() {
    ThreadInfo_ &info(_1<ThreadInfo_>());

    if (_R<Thread> *current = info.current_)
        return **current;

    _R<Thread> current(new _H<Thread>(::GetCurrentThreadId()));
    info.current_ = new _R<Thread>(current);
    return *current;
}

} }
