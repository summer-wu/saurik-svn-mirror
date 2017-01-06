/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#ifndef MENES_API_JNI_METHOD_HPP
#define MENES_API_JNI_METHOD_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/jni/types.hpp"

namespace api {
namespace Jni {

EXT_HANDLESTUB(class Class)
EXT_HANDLESTUB(class Runtime)

class Method :
    public ext::ReferenceCounted
{
  private:
    _R<Class> class_;
    ::jmethodID id_;

  public:
    Method(const _R<Class> &_class, ::jmethodID id);

    _R<Class> GetClass();
    ::jmethodID GetId();
};

/*template <typename Prototype_>
class StaticMethod :
    public hop::FunctionInvoke<
        StaticMethod<Prototype_>,
        Prototype_
    >
{
  private:
    _R<Method> method_;

  public:
    typedef hop::FunctionTraits<Prototype_> Traits;

    StaticMethod(const _R<Method> &method) :
        method_(method)
    {
    }

    inline typename Traits::Return InvokeValue(hop::ArgStruct<typename Traits::Args> args) const {
        static const size_t size(etl::Length<typename Traits::Args>::value);
        _L<jvalue> values(size);
        //method_->CallStaticVoid();
    }
};*/

} }

#endif//MENES_API_JNI_METHOD_HPP
