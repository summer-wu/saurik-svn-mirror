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

#ifndef MENES_MTA_TQUEUE_HPP
#define MENES_MTA_TQUEUE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include <queue>

#include "api/locks.hpp"

namespace menes {

template <typename Elem_>
class basic_tqueue {
  public:
    typedef size_t size_type;

  protected:
    mutable api::ThreadCondition::Mutex lock_;
    api::ThreadCondition cond_;
    volatile bool closed_;

    std::queue<Elem_> queue_;

    basic_tqueue() :
        lock_(),
        cond_(lock_),
        closed_(false)
    {
    }

  public:
    size_type size() const {
        ext::ScopeLock<api::ThreadCondition::Mutex> lockguard(lock_);
        return queue_.size();
    }

    bool empty() const {
        ext::ScopeLock<api::ThreadCondition::Mutex> lockguard(lock_);
        return queue_.empty();
    }

    void clear() {
        lock_.Lock();
        while (!queue_.empty())
            queue_.pop();
        lock_.Unlock();
    }

    bool closed() const {
        return closed_;
    }

    void close() {
        lock_.Lock();
        closed_ = true;
        cond_.Broadcast();
        lock_.Unlock();
    }
};

template <typename Elem_>
class itqueue :
    public basic_tqueue<Elem_>
{
  private:
    typedef basic_tqueue<Elem_> Base_;

  public:
    class closed_tqueue {
    };

  public:
    bool pop(Elem_ &elem) {
        ext::ScopeLock<api::ThreadCondition::Mutex> lockguard(Base_::lock_);
        while (Base_::closed_ || Base_::queue_.empty())
            if (Base_::closed_)
                return false;
            else Base_::cond_.Wait();
        elem = Base_::queue_.front();
        Base_::queue_.pop();
        return true;
    }

    Elem_ pop() {
        ext::ScopeLock<api::ThreadCondition::Mutex> lockguard(Base_::lock_);
        while (Base_::closed_ || Base_::queue_.empty())
            if (Base_::closed_)
                throw Base_::closed_tqueue();
            else Base_::cond_.Wait();
        Elem_ elem(Base_::queue_.front());
        Base_::queue_.pop();
        return elem;
    }

    void push(const Elem_ &elem) {
        Base_::lock_.Lock();
        Base_::queue_.push(elem);
        Base_::cond_.Signal();
        Base_::lock_.Unlock();
    }
};

template <typename Elem_>
class otqueue :
    public basic_tqueue<Elem_>
{
  private:
    typedef basic_tqueue<Elem_> Base_;

  public:
    class closed_tqueue {
    };

  protected:
    size_t size_;

  public:
    otqueue(size_t size) :
        size_(size)
    {
    }

    bool pop(Elem_ &elem) {
        ext::ScopeLock<api::ThreadCondition::Mutex> lockguard(Base_::lock_);
        if (Base_::closed_ || Base_::queue_.empty())
            return false;
        elem = Base_::queue_.front();
        Base_::queue_.pop();
        Base_::cond_.Signal();
        return true;
    }

    void push(const Elem_ &elem) {
        ext::ScopeLock<api::ThreadCondition::Mutex> lockguard(Base_::lock_);
        while (Base_::closed_ || Base_::queue_.size() == size_)
            if (Base_::closed_)
                throw closed_tqueue();
            else Base_::cond_.Wait();
        Base_::queue_.push(elem);
    }
};

}

#endif//MENES_MTA_TQUEUE_HPP
