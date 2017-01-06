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

#ifndef MENES_API_WIN32_HANDLE_HPP
#define MENES_API_WIN32_HANDLE_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/win32/error.hpp"
#include "ext/refcount.hpp"
#include "api/win32/types.hpp"

namespace api {
namespace Win32 {

static const unsigned Infinite = ~0;

class API_DECLARE HandleManager :
    public ext::ValueReferenceHelper<w32::Handle>
{
  public:
    static void Clear(Value &value);

    static bool IsEmpty(const Value &value);
    static bool IsEqual(const Value &lhs, const Value &rhs);

    static void Take(Value &lhs, const Value &rhs);
    static bool Drop(const Value &value);
};

class Handle :
    public _R<w32::Handle, HandleManager>
{
  private:
    typedef _R<w32::Handle, HandleManager> Base_;

  public:
    Handle() {
    }

    Handle(w32::Handle rhs, bool mended = false) :
        Base_(rhs, mended)
    {
    }

    Handle(const Handle &rhs) :
        Base_(rhs)
    {
    }

    API_DECLARE bool Wait(unsigned msecs = Infinite);

    Handle &operator =(const Handle &rhs) {
        Base_::operator =(rhs);
        return *this;
    }
};

template <typename Base_>
class HandleHolder :
    public Base_
{
  protected:
    Handle handle_;

    virtual w32::Handle GetHandle_() {
        return handle_;
    }

  public:
    HandleHolder() {
    }

    HandleHolder(w32::Handle handle, bool mended = false) :
        handle_(handle, mended)
    {
    }

    HandleHolder(const Handle &handle) :
        handle_(handle)
    {
    }

    const Handle &GetHandle() const {
        return handle_;
    }
};

} }

ETL_MEMMOVABLE(api::Win32::Handle)

#endif//MENES_API_WIN32_HANDLE_HPP
