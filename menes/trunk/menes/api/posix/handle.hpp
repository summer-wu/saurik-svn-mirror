/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#ifndef MENES_API_POSIX_HANDLE_HPP
#define MENES_API_POSIX_HANDLE_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/posix/error.hpp"
#include "etl/traits.hpp"

#include "ext/listset.hpp"
#include "ext/refcount.hpp"

#include <unistd.h>

#if defined(CFG_SYMBOL_FCNTL)
#include <fcntl.h>
#elif defined(CFG_DEFINE_FIOCLEX)
#include <sys/ioctl.h>
#endif

namespace api {
namespace Posix {

// XXX: this should use NullReferenceHelper<int, -1>
class HandleManager :
    public ext::ValueReferenceHelper<int>
{
  public:
    inline static void Clear(Value &value) {
        value = -1;
    }

    inline static bool IsEmpty(const Value &value) {
        return value == -1;
    }

    static void Mend(Value &lhs, const Value &rhs) {
        SetCloseOnExec(lhs = rhs, true);
    }

    static void Take(Value &lhs, const Value &rhs) {
        Mend(lhs, CheckError(::dup(rhs)));
    }

    static bool Drop(const Value &value) {
        CheckError(::close(value));
        // XXX: incorrect return semantic
        return true;
    }

#if defined(CFG_SYMBOL_FCNTL)
    static void SetCloseOnExec(const Value &handle, bool value) {
        CheckError(::fcntl(handle, F_SETFD, value ? FD_CLOEXEC : 0));
    }

    static bool GetCloseOnExec(const Value &handle) {
        return (CheckError(::fcntl(handle, F_GETFD)) & FD_CLOEXEC) != 0;
    }
#elif defined(CFG_DEFINE_FIOCLEX)
    static void SetCloseOnExec(const Value &handle, bool value) {
        CheckError(::ioctl(handle, value ? FIOCLEX : FIONCLEX, NULL));
    }
#endif
};

class Handle :
    public _R<int, HandleManager>
{
  private:
    typedef _R<int, HandleManager> Base_;

  public:
    Handle() {
    }

    Handle(int rhs, bool mended = false) :
        Base_(rhs, mended)
    {
    }

    Handle(const Handle &rhs) :
        Base_(rhs)
    {
    }

    Handle &operator =(const Handle &rhs) {
        Base_::operator =(rhs);
        return *this;
    }

    void SetCloseOnExec(bool value) {
        HandleManager::SetCloseOnExec(GetValue(), value);
    }

    bool GetCloseOnExec() const {
        return HandleManager::GetCloseOnExec(GetValue());
    }
};

template <typename Base_>
class HandleHolder :
    public Base_
{
  protected:
    Handle handle_;

    virtual int GetHandle_() {
        return handle_;
    }

  public:
    HandleHolder() {
    }

    HandleHolder(int handle, bool mended = false) :
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

template <typename Base_>
class FixedHandleHolder :
    public Base_
{
  protected:
    int file_;

    virtual int GetHandle_() {
        return file_;
    }

  public:
    FixedHandleHolder() :
        file_(-1)
    {
    }

    FixedHandleHolder(int file) :
        file_(file)
    {
    }
};

} }

ETL_MEMMOVABLE(api::Posix::Handle)

#endif//MENES_API_POSIX_HANDLE_HPP
