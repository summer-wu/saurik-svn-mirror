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

#ifndef MENES_API_PTHREADS_THREADLOCAL_HPP
#define MENES_API_PTHREADS_THREADLOCAL_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/pthreads/error.hpp"

#include <pthread.h>

namespace api {
namespace Pthreads {

template < typename Type_, bool Delete_ = true, typename Allocator_ = ext::StaticTypedAllocator<ext::NewAllocator> >
class API_DECLARE ThreadLocal :
    public ext::SmartPointer< Type_, Type_ *, ThreadLocal<Type_, Delete_> >
{
    friend class ext::SmartPointer< Type_, Type_ *, ThreadLocal<Type_, Delete_> >;

  private:
    typedef ThreadLocal<Type_, Delete_, Allocator_> This_;

  public:
    typedef typename Allocator_::template Template<Type_>::Result Allocator;

  private:
    pthread_key_t key_;

    // XXX: should this be deleted?
    Type_ *default_;

    void Init_() {
        int err(::pthread_key_create(&key_, Delete_ ? &Clear_ : NULL));
        if (err != 0)
            throw Error(err);
    }

    static void Clear_(void *value) {
        Allocator::DeallocateEx(reinterpret_cast<Type_ *>(value));
    }

    inline Type_ *GetSmartPointer_() const {
        return GetValue();
    }

  public:
    ThreadLocal() :
        default_(NULL)
    {
        Init_();
    }

    // this is for pointer compatibility only
    // XXX: think about a default_ parameter
    explicit ThreadLocal(Type_ *value) :
        default_(value)
    {
        Init_();
    }

    // XXX: Delete_ doesn't actually work
    ~ThreadLocal() {
        int err(::pthread_key_delete(key_));
        if (err != 0)
            throw Error(err);
    }

    This_ &operator =(Type_ *rhs) {
        int err(::pthread_setspecific(key_, rhs));
        if (err != 0)
            throw Error(err);
        return *this;
    }

    Type_ *GetValue() const {
        OPT_TIMER("api::Pthread::ThreadLocal::GetValue()")
        Type_ *value(reinterpret_cast<Type_ *>(::pthread_getspecific(key_)));
        return value == NULL ? default_ : value;
    }

    _finline operator Type_ *() const {
        return GetValue();
    }

    _finline pthread_key_t GetIndex() const {
        return key_;
    }
};

} }

#endif//MENES_API_PTHREADS_THREADLOCAL_HPP
