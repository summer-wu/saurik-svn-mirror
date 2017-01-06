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

#include "api/win32/locks.hpp"
#include "cse/utf16.hpp"
#include "opt/profile.hpp"

#include <windows.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

void sigwait(Waitable &signal, Waitable &wait) {
    if (-1 == ::SignalObjectAndWait(signal, wait, Infinite, FALSE))
        throw Error();
}

w32::Handle WaitSet::Wait() {
    DWORD wait(::WaitForMultipleObjectsEx(static_cast<w32::DWord>(handles_.GetSize()), handles_.Begin(), FALSE, Infinite, FALSE));
    if (wait == -1)
        throw Error();
    return handles_[wait - WAIT_OBJECT_0];
}

Event::operator HANDLE() const {
    return handle_;
}

Event::Event(const cse::String &name) {
    handle_.Mend(::OpenEventW(EVENT_ALL_ACCESS, FALSE, cse::Utf16String(name).NullTerminate()));
    if (handle_ == NULL)
        throw Error();
}

Event::Event(bool signaled, bool manual) {
    handle_.Mend(::CreateEventW(NULL, manual, signaled, NULL));
    if (handle_ == NULL)
        throw Error();
}

Event::Event(const cse::String &name, bool signaled, bool manual) {
    handle_.Mend(::CreateEventW(NULL, manual, signaled, cse::Utf16String(name).NullTerminate()));
    if (handle_ == NULL)
        throw Error();
}

bool Event::Wait(unsigned msecs) {
    return handle_.Wait(msecs);
}

void Event::Pulse() {
    if (FALSE == ::PulseEvent(handle_))
        throw Error();
}

void Event::Set() {
    if (FALSE == ::SetEvent(handle_))
        throw Error();
}

void Event::Reset() {
    if (FALSE == ::ResetEvent(handle_))
        throw Error();
}

Semaphore::operator HANDLE() const {
    return handle_;
}

Semaphore::Semaphore(const cse::String &name) {
    handle_.Mend(::OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, cse::Utf16String(name).NullTerminate()));
    if (handle_ == NULL)
        throw Error();
}

Semaphore::Semaphore(unsigned initial, unsigned max) {
    _assert(initial <= max);
    handle_.Mend(::CreateSemaphoreW(NULL, initial, max, NULL));
    if (handle_ == NULL)
        throw Error();
}

Semaphore::Semaphore(const cse::String &name, unsigned initial, unsigned max) {
    _assert(initial <= max);
    handle_.Mend(::CreateSemaphoreW(NULL, initial, max, cse::Utf16String(name).NullTerminate()));
    if (handle_ == NULL)
        throw Error();
}

bool Semaphore::Acquire(unsigned msecs) {
    return handle_.Wait(msecs);
}

unsigned Semaphore::Release(unsigned number) {
    LONG count;
    if (0 == ::ReleaseSemaphore(handle_, number, &count))
        throw Error();
    return count;
}

CriticalSection::CriticalSection(DWORD spin) {
    __try {
        /* // XXX: fix platform.hpp
        if (FALSE == ::InitializeCriticalSectionAndSpinCount(&section_, spin))
            throw Error();*/
        ::InitializeCriticalSection(&section_);
    } __except (STATUS_NO_MEMORY == ::GetExceptionCode() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        // XXX: throw a memory error
    }
}

CriticalSection::~CriticalSection() {
    ::DeleteCriticalSection(&section_);
}

/* // XXX: fix platform.hpp
void CriticalSection::Spin(DWORD spin) {
    ::SetCriticalSectionSpinCount(&section_, spin);
}*/

namespace {
void LockCriticalSection(CRITICAL_SECTION &section) {
    __try {
        ::EnterCriticalSection(&section);
    } __except (STATUS_INVALID_HANDLE == ::GetExceptionCode() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        // XXX: throw a memory error
    }
} }

void CriticalSection::Lock() {
    LockCriticalSection(section_);
}

void CriticalSection::Unlock() {
    ::LeaveCriticalSection(&section_);
}

ThreadMutex::ThreadMutex(DWORD spin) :
    section_(new CriticalSection(spin))
{
}

ThreadMutex::~ThreadMutex() {
    if (count_.Alone())
        delete section_;
}

/* // XXX: fix platform.hpp
void ThreadMutex::Spin(DWORD spin) {
    section_->spin(spin);
}*/

void ThreadMutex::Lock() {
    section_->Lock();
}

void ThreadMutex::Unlock() {
    section_->Unlock();
}

ProcessMutex::operator HANDLE() const {
    return handle_;
}

ProcessMutex::ProcessMutex(const cse::String &name) {
    handle_.Mend(::OpenMutexW(MUTEX_ALL_ACCESS, FALSE, cse::Utf16String(name).NullTerminate()));
    if (handle_ == NULL)
        throw Error();
}

ProcessMutex::ProcessMutex(bool locked) {
    handle_.Mend(::CreateMutexW(NULL, locked, NULL));
    if (handle_ == NULL)
        throw Error();
}

ProcessMutex::ProcessMutex(const cse::String &name, bool locked) {
    handle_.Mend(::CreateMutexW(NULL, locked, cse::Utf16String(name).NullTerminate()));
    if (handle_ == NULL)
        throw Error();
}

bool ProcessMutex::Lock(unsigned msecs) {
    OPT_TIMER("api::Win32::ProcessMutex::Lock()")
    return handle_.Wait(msecs);
}

void ProcessMutex::Unlock() {
    OPT_TIMER("api::Win32::ProcessMutex::Unlock()")
    if (FALSE == ::ReleaseMutex(handle_))
        throw Error();
}

} }
