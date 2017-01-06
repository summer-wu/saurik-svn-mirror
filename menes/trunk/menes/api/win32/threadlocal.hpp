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

#ifndef MENES_API_WIN32_THREADLOCAL_HPP
#define MENES_API_WIN32_THREADLOCAL_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/win32/error.hpp"
#include "ext/utility.hpp"

#include <wtypes.h>
#include <winbase.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

// XXX: this ignores the Allocator_ argument
template < typename Type_, bool Delete_ = true, typename Allocator_ = ext::StaticTypedAllocator<ext::NewAllocator> >
class API_DECLARE ThreadLocal :
    public ext::SmartPointer< Type_, Type_ *, ThreadLocal<Type_, Delete_, Allocator_> >
{
    friend _w32class ext::SmartPointer< Type_, Type_ *, ThreadLocal<Type_, Delete_, Allocator_> >;

  private:
    typedef ThreadLocal<Type_, Delete_, Allocator_> This_;

  private:
    w32::DWord index_;
    Type_ *default_;

    void Init_() {
        if (index_ == TLS_OUT_OF_INDEXES)
            throw Error();
    }

    inline Type_ *GetSmartPointer_() const {
        return GetValue();
    }

  public:
    ThreadLocal() :
        index_(::TlsAlloc())
    {
        Init_();
    }

    // this is for pointer compatibility only
    // XXX: think about a default_ parameter
    explicit ThreadLocal(Type_ *value) :
        index_(::TlsAlloc()),
        default_(value)
    {
        Init_();
    }

    // XXX: Delete_ doesn't actually work
    ~ThreadLocal() {
        if (w32::False == ::TlsFree(index_))
            throw Error();
    }

    This_ &operator =(Type_ *rhs) {
        if (w32::False == ::TlsSetValue(index_, rhs))
            throw Error();
        return *this;
    }

    Type_ *GetValue() const {
        if (void *value = ::TlsGetValue(index_))
            return reinterpret_cast<Type_ *>(value);
        else if (ApiTraits::GetLastError() != 0)
            throw Error();
        return default_;
    }

    _finline operator Type_ *() const {
        return GetValue();
    }

    w32::DWord GetIndex() const {
        return index_;
    }
};

} }

#endif//MENES_API_WIN32_THREADLOCAL_HPP
