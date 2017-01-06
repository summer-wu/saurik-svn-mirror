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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/reader.hpp"
#include "ios/writer.hpp"

#include "api/jni/class.hpp"
#include "api/jni/method.hpp"
#include "api/jni/object.hpp"
#include "api/jni/runtime.hpp"
#include "api/jni/streams.hpp"
#include "api/jni/string.hpp"

namespace api {
namespace Jni {
namespace Saxon {

namespace {
    _R<Class> saxon_;

    _R<Method> init_;
    _R<Method> transform_;

    _R<Class> GetSaxon_() {
        if (saxon_.IsEmpty())
            saxon_ = Runtime::GetRuntime()->GetClass("api/jni/saxon/Saxon");
        return saxon_;
    }
}

API_DECLARE _R<Object> New(const _R<ios::Reader> &xsl, const cse::String &uri) {
    OPT_TIMER("api::Jni::Saxon::New()")

    _R<Runtime> runtime(Runtime::GetRuntime());

    if (init_.IsEmpty())
        init_ = GetSaxon_()->GetMethod("<init>", "(Ljava/io/InputStream;Ljava/lang/String;)V");

    jobject object((*runtime)->NewObject(init_->GetClass()->GetId(), init_->GetId(), Adapt(xsl)->GetId(), Adapt(uri)->GetId()));
    _assert(object != NULL);
    return new _H<Object>(runtime, object);
}

API_DECLARE void Transform(const _R<Object> &saxon, const _R<ios::Reader> &in, const _R<ios::Writer> &out) {
    OPT_TIMER("api::Jni::Saxon::Transform()")

    _R<Runtime> runtime(Runtime::GetRuntime());

    if (transform_.IsEmpty())
        transform_ = GetSaxon_()->GetMethod("Transform", "(Ljava/io/InputStream;Ljava/io/OutputStream;)V");

    (*runtime)->CallVoidMethod(
        saxon->GetId(),
        transform_->GetId(),
        Adapt(in)->GetId(),
        Adapt(out)->GetId()
    );
}

} } }
