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

#include "api/files.hpp"
#include "api/uuid.hpp"

#include "api/jni/saxon/saxon.hpp"

#include "ext/uuid.hpp"
#include "ios/filter.hpp"
#include "mak/compiler.hpp"

#include "waf/escape.hpp"
#include "waf/processor.hpp"

namespace waf {

Processor::Compiled_::Compiled_() :
    modified_(0),
    process_(NULL)
{
}

Processor::Processor() :
    taglib_(api::Jni::Saxon::New(new _H<api::FileReader>(CFG_MENES_ROOT "/menes/waf/taglib.xsl"), "file://" CFG_MENES_ROOT "/menes/waf/taglib.xsl"))
{
}

void Processor::Process(const api::Path &file, net::Http::Server &server, const net::Http::Request &request, net::Http::Response &response) {
    Compiled_ &compiled(compiled_[file.GetPath()]);

    // XXX: time_t?!?
    time_t modified(file.GetLastModified());

    _R<api::Module> module;

    if (compiled.modified_ < modified) {
        ext::Uuid id(api::Uuid::Create());

        _S<ios::String> cpp, so;
        cpp << "/tmp/menes/" << id << ".cpp";
        so << "/tmp/menes/" << id << ".so";

        _S<api::FileReader> in(file.GetPath());

        api::Jni::Saxon::Transform(
            taglib_,
            ios::FilterAsReader<waf::Escape>(&in),
            new _H<api::FileWriter>(cpp)
        );

        mak::Compile(cpp, so);

        module = new _H<api::Module>(so, api::Module::Object);

        compiled.modified_ = modified;
        compiled.module_ = module;
        compiled.process_ = lowlevel_cast<WafProcessor>((*module)[_B("WafProcess")]);
    }

    compiled.process_(server, request, response);
}

}
