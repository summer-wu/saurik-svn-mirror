/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2001-2004  Jay Freeman (saurik)
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

#ifndef MENES_MTA_TPQUEUE_HPP
#define MENES_MTA_TPQUEUE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/locks.hpp"
#include "ext/queues.hpp"

#include <algorithm>

namespace mta {

template <
    typename Elem_,
    typename Cont_ = ext::Vector<Elem_>,
    typename Pred_ = std::less<Elem_>
>
class ThreadPriorityQueue {
  private:
    typedef ThreadPriorityQueue<Elem_, Cont_, Pred_> This_;
    typedef PriorityQueue<Elem_, Cont_, Pred_> Queue_;

  public:
    typedef typename Queue_::Value Value;
    typedef typename Queue_::cont_type cont_type;

    typedef api::ThreadCondition::Mutex Lock;

  protected:
    mutable Lock lock_;
    api::ThreadCondition cond_;

    Queue_ queue_;
    bool closed_;

    std::auto_ptr<Value> bound_;

    bool Empty_() const {
        return queue_.empty() || bound_.get() != NULL && queue_.GetPredicate()(queue_.top(), *bound_);
    }

  public:
    ThreadPriorityQueue() :
        cond_(lock_),
        closed_(false)
    {
    }

    ThreadPriorityQueue(const This_ &rhs) :
        cond_(lock_)
    {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(rhs.lock_);
        queue_ = rhs.queue_;
        closed_ = rhs.closed_;
        bound_ = std::auto_ptr<Value>(rhs.bound_.get() == NULL ? NULL : new Value(*rhs.bound_));
    }

    bool Empty() const {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        return Empty_();
    }

    size_t GetSize() const {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        return queue_.size();
    }

    void Push(const Value &elem) {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        queue_.push(elem);
        cond_.Signal();
    }

    bool Pop(Value *value = NULL, bool block = true) {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        if (!closed_ && Empty_())
            if (!block)
                return false;
            else do
                cond_.Wait();
            while (!closed_ && Empty_());
        if (closed_)
            return false;
        if (value != NULL)
            *value = queue_.top();
        queue_.pop();
        return true;
    }

    void Reheap() {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        queue_.reheap();
    }

    Lock &GetLock() {
        return lock_;
    }

    // XXX: this is only safe if already locked anyway
    cont_type &GetContainer() {
        return queue_.cont();
    }

    void SetBound(const Value *bound) {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        if (bound_.get() == NULL) {
            if (bound != NULL)
                bound_ = std::auto_ptr<value_type>(new value_type(*bound));
        } else if (bound == NULL)
            bound_ = std::auto_ptr<value_type>(NULL);
        else
            *bound_ = *bound;
        cond_.Broadcast();
    }

    bool IsClosed() {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        return closed_;
    }

    void Close() {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        closed_ = true;
        cond_.Broadcast();
    }

    void Clear() {
        ext::ScopeLock<api::ThreadCondition::Mutex> guard(lock_);
        queue_.cont().Clear();
    }
};

}

#endif//MENES_MTA_TPQUEUE_HPP
