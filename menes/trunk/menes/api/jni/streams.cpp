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

#include "ios/streams.hpp"

#include "api/jni/class.hpp"
#include "api/jni/method.hpp"
#include "api/jni/object.hpp"
#include "api/jni/runtime.hpp"
#include "api/jni/streams.hpp"

extern "C" {

JNIEXPORT jint JNICALL Java_api_jni_Reader_read_1__I(JNIEnv *environment, jclass _class, jint pointer) {
    byte_t value;
    return reinterpret_cast<ios::Reader *>(pointer)->Get(value) ? value : -1;
}

JNIEXPORT jint JNICALL Java_api_jni_Reader_read_1__I_3BII(JNIEnv *environment, jclass _class, jint pointer, jbyteArray data, jint offset, jint length) {
    jbyte *elements(environment->GetByteArrayElements(data, NULL));
    jint read(reinterpret_cast<ios::Reader *>(pointer)->Read(reinterpret_cast<byte_t *>(elements) + offset, length));
    environment->ReleaseByteArrayElements(data, elements, 0);
    return read == 0 ? -1 : read;
}

JNIEXPORT void JNICALL Java_api_jni_Writer_write_1__II(JNIEnv *environment, jclass _class, jint pointer, jint value) {
    // XXX: should I afford this?
    // _assert(value >= 0 && value < 256);
    ios::Put(*reinterpret_cast<ios::Writer *>(pointer), value);
}

JNIEXPORT void JNICALL Java_api_jni_Writer_write_1__I_3BII(JNIEnv *environment, jclass _class, jint pointer, jbyteArray data, jint offset, jint length) {
    jbyte *elements(environment->GetByteArrayElements(data, NULL));
    reinterpret_cast<ios::Writer *>(pointer)->WriteFully(reinterpret_cast<byte_t *>(elements) + offset, length);
    environment->ReleaseByteArrayElements(data, elements, JNI_ABORT);
}

}

namespace api {
namespace Jni {

namespace {
    _R<Method> reader_;
    _R<Method> writer_;
}

API_DECLARE _R<Object> Adapt(const _R<ios::Reader> &reader) {
    OPT_TIMER("api::Jni::Adapt(ios::Reader)")

    _R<Runtime> runtime(Runtime::GetRuntime());

    if (reader_.IsEmpty()) {
        _R<api::Jni::Class> _class(runtime->GetClass("api/jni/Reader"));
        reader_ = _class->GetMethod("<init>", "(I)V");

        JNINativeMethod method[2];

        method[0].name = const_cast<char *>("read_");
        method[0].signature = const_cast<char *>("(I)I");
        method[0].fnPtr = lowlevel_cast<void *>(&Java_api_jni_Reader_read_1__I);

        method[1].name = const_cast<char *>("read_");
        method[1].signature = const_cast<char *>("(I[BII)I");
        method[1].fnPtr = lowlevel_cast<void *>(&Java_api_jni_Reader_read_1__I_3BII);

        (*runtime)->RegisterNatives(_class->GetId(), method, 2);
    }

    jobject object((*runtime)->NewObject(reader_->GetClass()->GetId(), reader_->GetId(), reader.GetValue()));
    _assert(object != NULL);
    return new _H<api::Jni::Object>(runtime, object);
}

API_DECLARE _R<Object> Adapt(const _R<ios::Writer> &writer) {
    OPT_TIMER("api::Jni::Adapt(ios::Writer)")

    _R<Runtime> runtime(Runtime::GetRuntime());

    if (writer_.IsEmpty()) {
        _R<api::Jni::Class> _class(runtime->GetClass("api/jni/Writer"));
        writer_ = _class->GetMethod("<init>", "(I)V");

        JNINativeMethod method[2];

        method[0].name = const_cast<char *>("write_");
        method[0].signature = const_cast<char *>("(II)V");
        method[0].fnPtr = lowlevel_cast<void *>(&Java_api_jni_Writer_write_1__II);

        method[1].name = const_cast<char *>("write_");
        method[1].signature = const_cast<char *>("(I[BII)V");
        method[1].fnPtr = lowlevel_cast<void *>(&Java_api_jni_Writer_write_1__I_3BII);

        (*runtime)->RegisterNatives(_class->GetId(), method, 2);
    }

    jobject object((*runtime)->NewObject(writer_->GetClass()->GetId(), writer_->GetId(), writer.GetValue()));
    _assert(object != NULL);
    return new _H<api::Jni::Object>(runtime, object);
}

} }
