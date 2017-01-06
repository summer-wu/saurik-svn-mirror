/* Menes Optimization - C++ Profiling and Verification
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

#ifndef MENES_SZF_METHOD_HPP
#define MENES_SZF_METHOD_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "com/object.hpp"
#include "ext/types.hpp"

#include <typeinfo>

namespace szf {

class Method;
class Serializable;

class Method :
    virtual public com::Object
{
  private:
    virtual void *GetContext_(const std::type_info *type) const = 0;
    virtual void SetContext_(const std::type_info *type, void *context) = 0;

  public:
    template <typename Type_>
    Type_ *GetContext() const {
        return reinterpret_cast<Type_ *>(GetContext_(&typeid(Type_)));
    }

    template <typename Type_>
    void SetContext(Type_ *context) const {
        SetContext_(&typeid(context), reinterpret_cast<void *>(context));
    }

    virtual void OpenScope(const char *name) = 0;
    virtual void CloseScope() = 0;

    virtual void ClearObjects() = 0;

    template <typename Type_>
    Type_ Load(const char *name);

    virtual void SaveBoolean(bool value, const char *name) = 0;
    virtual void LoadBoolean(bool &value, const char *name) = 0;

    virtual void SaveSigned(int64_t value, const char *name) = 0;
    virtual void LoadSigned(int64_t &value, const char *name) = 0;

    virtual void SaveUnsigned(uint64_t value, const char *name) = 0;
    virtual void LoadUnsigned(uint64_t &value, const char *name) = 0;

    virtual void SaveFloating(double value, const char *name) = 0;
    virtual void LoadFloating(double &value, const char *name) = 0;

    virtual void SaveObject(const _R<Serializable> &value, const char *name) = 0;
    virtual void LoadObject(_R<Serializable> &value, const char *name) = 0;

    virtual void SaveUuid(const ext::Uuid &value, const char *name) = 0;
    virtual void LoadUuid(ext::Uuid &value, const char *name) = 0;

    virtual void SaveString(const cse::String &value, const char *name) = 0;
    virtual void LoadString(cse::String &value, const char *name) = 0;

    virtual void SaveData(const char *data, size_t length, const char *name) = 0;
    virtual void LoadData(char *data, size_t length, const char *name) = 0;
};

template <typename Type_>
class ScopeContext {
  private:
    Method *method_;
    Type_ *context_;

  public:
    ScopeContext(Method *method, Type_ *context) :
        method_(method),
        context_(method->template GetContext<Type_>())
    {
        method->SetContext(context);
    }

    ~ScopeContext() {
        method_->SetContext(context_);
    }
};

}

#endif//MENES_SZF_METHOD_HPP
