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

#ifndef MENES_API_WIN32_LOCKS_HPP
#define MENES_API_WIN32_LOCKS_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/error.hpp"
#include "ext/list.hpp"
#include "ext/refcount.hpp"
#include "ext/vector.hpp"

#include "api/win32/error.hpp"
#include "api/win32/handle.hpp"
#include "api/win32/types.hpp"

// XXX: this is _so mean_!!
#include <wtypes.h>
#include <winnt.h>
#include "api/win32/undefine.hpp"

/*
     Locks tend to not work correctly while using
  a Win32 debugger.  This is due to various side-
  effects of cool features of said debuggers :).

         Check the Knowledge Base article:
   "PRB: Synchronization Failure When Debugging"
*/

namespace api {
namespace Win32 {

// XXX: _abstract
class Waitable {
  public:
    virtual operator w32::Handle() const = 0;
};

API_DECLARE void sigwait(Waitable &signal, Waitable &wait);

class WaitSet {
  protected:
    typedef ext::Vector<w32::Handle> HandleSet_;
    HandleSet_ handles_;

  public:
    void Insert(const Waitable &wait) {
        Insert(static_cast<w32::Handle>(wait));
    }

    void Erase(const Waitable &wait) {
        Erase(static_cast<w32::Handle>(wait));
    }

    void Insert(w32::Handle wait) {
        handles_.InsertLast(wait);
    }

    void Erase(w32::Handle wait) {
        HandleSet_::Iterator value(ext::FindFirst(handles_, wait));
        _assert(value != handles_.End());
        handles_.RemoveAt(value);
    }

    w32::Handle Wait();

    w32::Handle operator [](size_t index) const {
        return handles_[index];
    }
};

class API_DECLARE Event :
    public Waitable
{
  protected:
    Handle handle_;

    operator w32::Handle() const;

  public:
    Event(const cse::String &name);

    explicit Event(bool signaled = false, bool manual = true);
    Event(const cse::String &name, bool signaled, bool manual = true);

    bool Wait(unsigned msecs = Infinite);
    void Pulse();
    void Set();
    void Reset();
};

class API_DECLARE Semaphore :
    public Waitable
{
  protected:
    Handle handle_;

    operator w32::Handle() const;

  public:
    Semaphore(const cse::String &name);

    explicit Semaphore(unsigned initial = 0, unsigned max = 0x7fffffff);
    explicit Semaphore(const cse::String &name, unsigned initial = 0, unsigned max = 0x7fffffff);

    bool Acquire(unsigned msecs = Infinite);
    unsigned Release(unsigned number = 1);
};

class API_DECLARE CriticalSection {
  protected:
    CRITICAL_SECTION section_;

  public:
    explicit CriticalSection(w32::DWord spin = 0);

    ~CriticalSection();

    /* // XXX: fix platform.hpp
    void spin(DWORD spin);*/

    void Lock();
    void Unlock();
};

class API_DECLARE ThreadMutex {
  protected:
    CriticalSection *section_;
    ext::IndirectCount count_;

  public:
    explicit ThreadMutex(w32::DWord spin = 0);

    ~ThreadMutex();

    /* // XXX: fix platform.hpp
    void spin(DWORD spin);*/

    void Lock();
    void Unlock();
};

class API_DECLARE ProcessMutex :
    public Waitable
{
  protected:
    Handle handle_;

    operator w32::Handle() const;

  public:
    ProcessMutex(const cse::String &name);

    explicit ProcessMutex(bool locked = false);
    explicit ProcessMutex(const cse::String &name, bool locked);

    bool Lock(unsigned msecs = Infinite);
    void Unlock();
};

class FastCondition {
  public:
    typedef ThreadMutex Mutex;

  protected:
    unsigned waiters_;
    ThreadMutex mutex_;
    Semaphore queue_;
    Event done_;
    bool broadcast_;

    Mutex lock_;

  public:
    explicit FastCondition(Mutex &Lock) :
        waiters_(0),
        broadcast_(false),
        done_(false, false),
        lock_(Lock)
    {
    }

    void Signal() {
        if (waiters_ != 0)
            queue_.Release();
    }

    void Broadcast() {
        if (waiters_ == 0)
            return;
        broadcast_ = true;
        queue_.Release(waiters_);
        done_.Wait();
        broadcast_ = false;
    }

    void Wait() {
        ++waiters_;

        lock_.Unlock();
        queue_.Acquire();

        mutex_.Lock();
        bool last(--waiters_ == 0);
        mutex_.Unlock();

        if (last) {
            done_.Set();
            lock_.Lock();
        } else lock_.Lock();
    }
};

class ThreadCondition {
  public:
    typedef ProcessMutex Mutex;

  protected:
    unsigned waiters_;
    ThreadMutex mutex_;
    Semaphore queue_;
    Event done_;
    bool broadcast_;

    Mutex lock_;

  public:
    explicit ThreadCondition(Mutex &Lock) :
        waiters_(0),
        broadcast_(false),
        done_(false, false),
        lock_(Lock)
    {
    }

    void Signal() {
        if (waiters_ != 0)
            queue_.Release();
    }

    void Broadcast() {
        if (waiters_ == 0)
            return;
        broadcast_ = true;
        queue_.Release(waiters_);
        done_.Wait();
        broadcast_ = false;
    }

    void Wait() {
        ++waiters_;

        sigwait(lock_, queue_);

        mutex_.Lock();
        bool last(--waiters_ == 0);
        mutex_.Unlock();

        if (last)
            sigwait(done_, lock_);
        else
            lock_.Lock();
    }
};

} }

#endif//MENES_API_WIN32_LOCKS_HPP
