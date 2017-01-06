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

#ifndef MENES_EXT_WEAKREF_HPP
#define MENES_EXT_WEAKREF_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/refcount.hpp"

namespace ext {

class WeakReferencable;

class WeakShell :
    public ext::ReferenceCounted
{
  private:
    WeakReferencable *value_;

  public:
    WeakShell(WeakReferencable *value) :
        value_(value)
    {
    }

    void Clear() {
        value_ = NULL;
    }

    WeakReferencable *GetValue() const {
        return value_;
    }

    operator WeakReferencable *() const {
        return GetValue();
    }
};

class EXT_DECLARE WeakReferencable {
  private:
    _R<WeakShell> shell_;

  public:
    WeakReferencable();
    virtual ~WeakReferencable();

    _R<WeakShell> GetShell() const;
};

template <typename Type_>
class WeakHandle :
    public ext::SmartPointer< Type_, Type_ *, WeakHandle<Type_> >
{
    friend class SmartPointer< Type_, Type_ *, WeakHandle<Type_> >;

  private:
    typedef WeakHandle<Type_> This_;

    Type_ *value_;
    mutable _R<WeakShell> shell_;

    Type_ *GetSmartPointer_() const {
        if (!shell_.IsEmpty()) {
            if (shell_->GetValue() == NULL)
                shell_.Clear();
            else
                return value_;
        }

        return NULL;
    }

    // XXX: make public?
    void AssignToShell_(WeakShell *shell) {
        if (shell == NULL) {
            value_ = NULL;
            shell_.Clear();
        } else {
            value_ = dynamic_cast<Type_ *>(shell->GetValue());
            _assert(*shell == NULL || value_ != NULL);
            shell_.operator =(shell);
        }
    }

  public:
    WeakHandle() :
        value_(NULL)
    {
    }

    template <typename Type2_>
    WeakHandle(const WeakHandle<Type2_> &rhs) :
        value_(NULL)
    {
        operator =(rhs);
    }

    /*WeakHandle(WeakShell *shell) {
        AssignToShell(shell);
    }*/

    WeakHandle(Type_ *rhs) :
        value_(NULL)
    {
        operator =(rhs);
    }

    This_ &operator =(const This_ &rhs) {
        value_ = rhs.value_;
        shell_ = rhs.shell_;
        return *this;
    }

    template <typename Type2_>
    This_ &operator =(const WeakHandle<Type2_> &rhs) {
        AssignToShell_(rhs.shell_);
        return *this;
    }

    /*This_ &operator =(const _R<WeakShell> &rhs) {
        AssignToShell_(rhs);
        return *this;
    }*/

    This_ &operator =(Type_ *rhs) {
        WeakShell *shell;
        if (rhs == NULL) {
            value_ = NULL;
            shell = NULL;
        } else {
            WeakReferencable *weak(dynamic_cast<WeakReferencable *>(rhs));
            _assert(weak != NULL);
            value_ = rhs;
            shell = weak->GetShell();
        }

        shell_ = shell;
        return *this;
    }

    bool operator !() const {
        return IsEmpty();
    }

    bool IsEmpty() const {
        return GetValue() == NULL;
    }

    void Clear() {
        value_ = NULL;
        shell_.Clear();
    }

    Type_ *GetValue() const {
        return GetSmartPointer_();
    }

    void Assign(Type_ *value, WeakReferencable *weak) {
        value_ = value;
        shell_ = weak->GetShell();
    }

    bool operator <(const This_ &rhs) const {
        return value_ < rhs.value_;
    }
};

template <typename Type_, typename Handle_>
class StrongWeakHandle {
  private:
    typedef StrongWeakHandle<Type_, Handle_> This_;

    WeakHandle<Type_> weak_;
    Handle_ strong_;

  public:
    StrongWeakHandle() {
    }

    StrongWeakHandle(const This_ &rhs) {
        operator =(rhs);
    }

    StrongWeakHandle(Type_ *rhs) {
        operator =(rhs);
    }

    This_ &operator =(const This_ &rhs) {
        weak_ = rhs.weak_;
        strong_ = rhs.strong_;
        return *this;
    }

    This_ &operator =(Type_ *rhs) {
        WeakReferencable *weak(dynamic_cast<WeakReferencable *>(rhs));

        if (weak != NULL) {
            weak_.Assign(rhs, weak);
            strong_.Clear();
        } else {
            strong_ = rhs;
            weak_.Clear();
        }

        return *this;
    }

    operator Handle_() const {
        if (strong_.IsEmpty())
            return weak_.GetValue();
        else
            return strong_;
    }

    bool IsEmpty() const {
        return strong_.IsEmpty() && weak_.IsEmpty();
    }
};

}

#endif//MENES_EXT_WEAKREF_HPP
