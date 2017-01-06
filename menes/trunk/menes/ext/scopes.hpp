/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#ifndef MENES_EXT_SCOPES_HPP
#define MENES_EXT_SCOPES_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/algorithm.hpp"
#include "ext/vector.hpp"

namespace ext {

namespace be {
    struct Destruct {
        virtual ~Destruct() {
        }
    };

    // XXX: this should support allocator templates
    template <typename Type_>
    struct DestructImpl :
        public Destruct
    {
      private:
        Type_ *object_;

      public:
        DestructImpl(Type_ *object) :
            object_(object)
        {
        }

        ~DestructImpl() {
            delete object_;
        }
    };
}

class Scope {
  public:
    _finline operator bool() const {
        return false;
    }
};

// XXX: this should be generalized as it sounds too specific to even be useful
class ScopeDestruct {
  private:
    typedef Vector<be::Destruct *> DestructList_;
    DestructList_ destructs_;

  public:
    template <typename Type_>
    void Insert(Type_ *object) {
        destructs_.InsertLast(static_cast<be::Destruct *>(new be::DestructImpl<Type_>(object)));
    }

    ~ScopeDestruct() {
        Delete(destructs_.Begin(), destructs_.End());
    }
};

template <typename Type_, typename Temp_ = Type_>
class ScopeIncrement {
  private:
    Temp_ rhs_;
    Type_ &lhs_;
    bool keep_;

  public:
    _finline ScopeIncrement(Type_ &lhs) :
        rhs_(lhs),
        lhs_(lhs),
        keep_(false)
    {
        ++lhs;
    }

    _finline ~ScopeIncrement() {
        if (!keep_)
            --lhs_;
    }

    _finline void Keep() {
        keep_ = true;
    }

    operator const Temp_ &() const {
        return rhs_;
    }
};

template <typename Type_, typename Temp_ = Type_>
class ScopeSet {
  private:
    Temp_ rhs_;
    Type_ &lhs_;
    bool keep_;

  public:
    _finline ScopeSet(Type_ &lhs, const Temp_ &rhs) :
        rhs_(lhs),
        lhs_(lhs),
        keep_(false)
    {
        lhs = rhs;
    }

    _finline ~ScopeSet() {
        if (!keep_)
            lhs_ = rhs_;
    }

    _finline const Temp_ &GetValue() const {
        return rhs_;
    }

    _finline void Keep() {
        keep_ = true;
    }

    operator const Temp_ &() const {
        return GetValue();
    }
};

template <typename Type_>
class ScopeSwap {
  private:
    Type_ &lhs_;
    Type_ &rhs_;

  public:
    _finline ScopeSwap(Type_ &lhs, Type_ &rhs) :
        lhs_(lhs), rhs_(rhs)
    {
        ext::Swap(lhs_, rhs_);
    }

    _finline ~ScopeSwap() {
        ext::Swap(lhs_, rhs_);
    }
};

// XXX: move this out into the world somewhere
class FreeLock {
  public:
    _finline static void Lock() {
    }

    _finline static void Unlock() {
    }
};

template <typename Lock_>
class ScopeLock :
    public Scope
{
  protected:
    Lock_ *lock_;

  public:
    explicit ScopeLock(Lock_ &lock) :
        lock_(&lock)
    {
        lock_->Lock();
    }

    ScopeLock(Lock_ &lock, bool locked) {
        if (!locked)
            lock_ = NULL;
        else {
            lock_ = &lock;
            lock_->Lock();
        }
    }

    ~ScopeLock() {
        Release();
    }

    void Release() {
        if (lock_ != NULL) {
            lock_->Unlock();
            lock_ = NULL;
        }
    }
};

namespace be {
template <typename Lock_>
_finline ScopeLock<Lock_> ScopeLock_(Lock_ &lock) {
    return ScopeLock<Lock_>(lock);
} }

#define _synchronized(lock) if (const ext::Scope &_scope = ext::be::ScopeLock_(lock)); else

template <typename Lock_>
class ScopeUnlock :
    public Scope
{
  protected:
    Lock_ *lock_;
    bool unlocked_;

  public:
    explicit ScopeUnlock(Lock_ &lock) :
        lock_(&lock)
    {
        lock_->Unlock();
    }

    ScopeUnlock(Lock_ &lock, bool unlocked) {
        if (!unlocked)
            lock_ = NULL;
        else {
            lock_ = &lock;
            lock_->Unlock();
        }
    }

    ~ScopeUnlock() {
        Release();
    }

    void Release() {
        if (lock_ != NULL) {
            lock_->Lock();
            lock_ = NULL;
        }
    }
};

namespace be {
template <typename Lock_>
_finline ScopeUnlock<Lock_> ScopeUnlock_(Lock_ &lock) {
    return ScopeUnlock<Lock_>(lock);
} }

#define _desynchronized(lock) if (const ext::Scope &_scope = ext::be::ScopeUnlock_(lock)); else

template <typename Lock_>
class ScopeReadLock :
    public Scope
{
  private:
    Lock_ &lock_;

  public:
    ScopeReadLock(Lock_ &lock) :
        lock_(lock)
    {
        lock_.ReadLock();
    }

    ~ScopeReadLock() {
        lock_.ReadUnlock();
    }
};

template <typename Lock_>
class ScopeWriteLock :
    public Scope
{
  private:
    Lock_ &lock_;

  public:
    ScopeWriteLock(Lock_ &lock) :
        lock_(lock)
    {
        lock_.WriteLock();
    }

    ~ScopeWriteLock() {
        lock_.WriteUnlock();
    }
};

}

#endif//MENES_EXT_SCOPES_HPP
