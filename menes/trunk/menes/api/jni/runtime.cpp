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

#include "api/process.hpp"
#include "ios/helpers.hpp"

#include "api/jni/class.hpp"
#include "api/jni/runtime.hpp"

namespace api {
namespace Jni {

EXT_HANDLESHIM(Runtime)

namespace {
    _R<Runtime> runtime_;
}

Runtime::Runtime(const _L<cse::String> &options) {
    ext::Vector<JavaVMOption> vmoptions;
    vmoptions.Reserve(options.GetSize());

    _foreach (const _L<cse::String>, option, options) {
        vmoptions.InsertLast();
        vmoptions.Last().optionString = const_cast<char *>(option->NullTerminate());
        vmoptions.Last().extraInfo = NULL;
    }

    ::JavaVMInitArgs args;
    args.version = JNI_VERSION_1_4;
    args.options = vmoptions.Begin();
    args.nOptions = vmoptions.GetSize();
    args.ignoreUnrecognized = JNI_FALSE;

    void *environment;
    ::jint error(JNI_CreateJavaVM(&jvm_, &environment, &args));
    _assert(error == 0);

    environment_ = static_cast< ::JNIEnv *>(environment);
}

Runtime::~Runtime() {
    //::jint error(jvm_->DestroyJavaVM());

    // XXX: apparently this _always_ returns an error
    //      java == good :(
    //_assert(error == 0);
}

_R<Class> Runtime::GetClass(const cse::String &name) {
    jclass _class((*this)->FindClass(name.NullTerminate()));
    _assert(_class != NULL);
    return new _H<Class>(this, _class);
}

::JNIEnv *Runtime::operator ->() const {
    if (void *environment = environment_.GetValue())
        return static_cast< ::JNIEnv *>(environment);
    else {
        ::JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_2;
        // XXX: this would be fun to support
        args.name = NULL;
        // XXX: what does this even do?
        args.group = NULL;

        ::jint error(jvm_->AttachCurrentThread(&environment, &args));
        _assert(error == 0);

        environment_ = static_cast< ::JNIEnv *>(environment);
        return static_cast< ::JNIEnv *>(environment);
    }
}

_R<Runtime> Runtime::GetRuntime() {
    if (runtime_.IsEmpty()) {
        _L<cse::String> vmoptions;

        vmoptions.InsertLast("-Xbootclasspath/p:" CFG_MENES_ROOT "/java-bcp/xercesImpl.jar");
        //vmoptions.InsertLast("-Xdebug");
        //vmoptions.InsertLast("-verbose:jni");
        vmoptions.InsertLast("-Xcheck:jni");
        //vmoptions.InsertLast("-Xint");
        //vmoptions.InsertLast("-Xnoclassgc");

        {
            _S<api::Process> classpath(CFG_MENES_ROOT "/utils/classpath.sh");
            classpath.ClearWriter();
            vmoptions.InsertLast(_S<ios::String>() << _B("-Djava.class.path=") << ios::ReadAll(*classpath.GetReader()));
        }

        runtime_ = new _H<Runtime>(vmoptions);
    }

    return runtime_;
}

} }
