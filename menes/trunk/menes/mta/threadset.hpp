/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_MTA_THREADS_HPP
#define MENES_MTA_THREADS_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/locks.hpp"
#include "api/threads.hpp"

#include "cse/string.hpp"
#include "ext/vector.hpp"

namespace mta {

template <typename Thread_ = api::Thread>
class ThreadSet {
  protected:
    typedef _L< _R<Thread_> > ThreadList;
    ThreadList threads_;

    mutable api::ThreadMutex lock_;

  public:
    void Join() const {
        _synchronized (lock_)
            _tforeach (const ThreadList, thread, threads_)
                (*thread)->Join();
    }

    void Add(const typename Thread_::Function &function) {
        _synchronized (lock_)
            threads_.InsertLast(new _H<Thread_>(function));
    }

    void Name(const cse::String &name) {
        _synchronized (lock_) {
            typename ThreadList::Iterator thread(threads_.Begin());
            for (int i(0); thread != threads_.End(); ++thread, ++i)
                (*thread)->Name(_S<ios::String>() << name << "(" << i << ")");
        }
    }
};

template <typename Thread_ = api::Thread>
class ThreadPool {
  protected:
    ThreadSet<Thread_> threads_;
    typename Thread_::Function function_;

  public:
    ThreadPool(const typename Thread_::Function &function) :
        function_(function)
    {
    }

    void Join() const {
        threads_.Join();
    }

    void Add(unsigned number) {
        for (unsigned i(0); i < number; ++i)
            threads_.Add(function_);
    }

    void Name(const cse::String &name) {
        threads_.Name(name);
    }
};

}

#endif//MENES_MTA_THREADS_HPP
