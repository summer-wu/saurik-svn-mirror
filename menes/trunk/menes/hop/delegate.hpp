/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2004  Jay Freeman (saurik)
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

#ifndef MENES_ETL_DELEGATE_HPP
#define MENES_ETL_DELEGATE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/locks.hpp"
#include "hop/function.hpp"

#include "ext/foreach.hpp"
#include "ext/vector.hpp"

namespace hop {

template <typename Prototype_>
class Delegate :
    protected Function<Prototype_>
{
  private:
    typedef Delegate<Prototype_> This_;
    typedef Function<Prototype_> Base_;

  public:
    typedef typename Base_::Traits Traits;

  private:
    typedef ext::Vector<Base_> FunctionList_;

    struct Helper_ {
        FunctionList_ functions_;
        api::CriticalSection lock_;

        Helper_() {
        }

        Helper_(const Helper_ &rhs) :
            functions_(rhs.functions_)
        {
        }

        bool operator ==(const Helper_ &rhs) const {
            return functions_ == rhs.functions_;
        }

        bool operator !=(const Helper_ &rhs) const {
            return functions_ == rhs.functions_;
        }

        bool operator <(const Helper_ &rhs) const {
            return functions_ < rhs.functions_;
        }
    };

  private:
    static typename Traits::Return Invoke_(const Base_ *this_, ArgStruct<typename Traits::Args> args) {
        Helper_ &helper(*reinterpret_cast<Helper_ *>(this_->GetValue().data));
        ext::ScopeLock<api::CriticalSection> guard(helper.lock_);
        _tforeach (FunctionList_, i, helper.functions_)
            i->InvokeReference(args);
    }

    Helper_ &GetHelper_() const {
        return *reinterpret_cast<Helper_ *>(Base_::GetValue().data);
    }

  public:
    Delegate() {
        static etl::TypeVTableImpl<Helper_> table;
        Helper_ helper;
        Assign(typename Base_::Value(reinterpret_cast<typename Base_::Value::Invoker>(&Invoke_), &helper), &table);
    }

    using Base_::operator ();

    // XXX: these are needed only because you can't autocast through Function to Handle<>
    bool operator ==(const This_ &rhs) const {
        return Base_::operator ==(rhs);
    }

    bool operator !=(const This_ &rhs) const {
        return Base_::operator !=(rhs);
    }

    bool operator <(const This_ &rhs) const {
        return Base_::operator <(rhs);
    }

    This_ &operator +=(const Base_ &rhs) {
        Helper_ &helper(GetHelper_());
        ext::ScopeLock<api::CriticalSection> guard(helper.lock_);
        helper.functions_.InsertLast(rhs);
        return *this;
    }

    This_ &operator -=(const Base_ &rhs) {
        Helper_ &helper(GetHelper_());
        ext::ScopeLock<api::CriticalSection> guard(helper.lock_);
        helper.functions_.RemoveFirst(rhs);
        return *this;
    }

    size_t GetSize() const {
        Helper_ &helper(GetHelper_());
        ext::ScopeLock<api::CriticalSection> guard(helper.lock_);
        return helper.functions_.GetSize();
    }

    const Base_ &GetFunction() const {
        return *this;
    }
};

}

#endif//MENES_ETL_DELEGATE_HPP
