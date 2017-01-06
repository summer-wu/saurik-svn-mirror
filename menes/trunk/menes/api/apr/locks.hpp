/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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

#ifndef MENES_API_APR_LOCKS_HPP
#define MENES_API_APR_LOCKS_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/apr/pool.hpp"

namespace api {
namespace Apr {

class ThreadMutex {
  protected:
    Pool context_;
    ::apr_thread_mutex_t *mutex_;

  public:
    explicit ThreadMutex(bool locked = false) {
        ::apr_thread_mutex_create(&mutex_, APR_THREAD_MUTEX_DEFAULT, context_);
    }

    ~ThreadMutex() {
        ::apr_thread_mutex_destroy(mutex_);
    }

    void Lock() {
        ::apr_thread_mutex_lock(mutex_);
    }

    void Unlock() {
        ::apr_thread_mutex_unlock(mutex_);
    }

    operator ::apr_thread_mutex_t *() const {
        return mutex_;
    }
};

class ThreadCondition {
  public:
    typedef thread_mutex Mutex;

  protected:
    Pool context_;
    Mutex &mutex_;
    ::apr_thread_cond_t *cond_;

  public:
    explicit ThreadCondition(Mutex &mutex) :
        lock_(lock)
    {
        ::apr_thread_cond_create(&cond_, context_);
    }

    ~ThreadCondition() {
        ::apr_thread_cond_destroy(cond_);
    }

    void Signal() {
        ::apr_thread_cond_signal(cond_);
    }

    void Broadcast() {
        ::apr_thread_cond_broadcast(cond_);
    }

    void Wait() {
        ::apr_thread_cond_wait(cond_, mutex_);
    }

    operator ::apr_thread_cond_t *() const {
        return cond_;
    }
};

} }

#endif//MENES_API_APR_LOCKS_HPP
