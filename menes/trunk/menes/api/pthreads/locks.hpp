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

#ifndef MENES_API_PTHREADS_LOCKS_HPP
#define MENES_API_PTHREADS_LOCKS_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/posix/error.hpp"
#include "api/pthreads/error.hpp"

#include "ext/refcount.hpp"

#include <pthread.h>

namespace api {
namespace Pthreads {

class CriticalSection {
  protected:
    mutable pthread_mutex_t mutex_;

  public:
    CriticalSection() {
        int err(::pthread_mutex_init(&mutex_, NULL));
        if (err != 0)
            throw Error(err);
    }

    ~CriticalSection() {
        int err(::pthread_mutex_destroy(&mutex_));
        if (err != 0)
            throw Error(err);
    }

    void Lock() {
        int err(::pthread_mutex_lock(&mutex_));
        if (err != 0)
            throw Error(err);
    }

    void Unlock() {
        int err(::pthread_mutex_unlock(&mutex_));
        if (err != 0)
            throw Error(err);
    }

    operator pthread_mutex_t *() const {
        return &mutex_;
    }
};

class ThreadMutex {
  protected:
    CriticalSection *section_;
    ext::IndirectCount count_;

  public:
    ThreadMutex() :
        section_(new CriticalSection)
    {
    }

    ~ThreadMutex() {
        if (count_.Alone())
            delete section_;
    }

    void Lock() {
        section_->Lock();
    }

    void Unlock() {
        section_->Unlock();
    }

    operator pthread_mutex_t *() const {
        return *section_;
    }
};

class ThreadCondition {
  public:
    typedef ThreadMutex Mutex;

  protected:
    mutable pthread_cond_t cond_;
    Mutex lock_;
    ext::IndirectCount count_;

  public:
    explicit ThreadCondition(Mutex &lock) :
        lock_(lock)
    {
        int err(::pthread_cond_init(&cond_, NULL));
        if (err != 0)
            throw Error(err);
    }

    ~ThreadCondition() {
		if (count_.Alone()) {
	        int err(::pthread_cond_destroy(&cond_));
	        if (err != 0)
	            throw Error(err);
		}
    }

    void Signal() {
        int err(::pthread_cond_signal(&cond_));
        if (err != 0)
            throw Error(err);
    }

    void Broadcast() {
        int err(::pthread_cond_broadcast(&cond_));
        if (err != 0)
            throw Error(err);
    }

    void Wait() {
        int err(::pthread_cond_wait(&cond_, lock_));
        if (err != 0)
            throw Error(err);
    }

    operator pthread_cond_t *() const {
        return &cond_;
    }
};

} }

#endif//MENES_API_PTHREADS_LOCKS_HPP
