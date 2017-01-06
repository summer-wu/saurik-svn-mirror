/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "api/console.hpp"
#include "api/debug.hpp"
#include "api/exename.hpp"
#include "api/files.hpp"

#include "app/configuration.hpp"
#include "app/setup.hpp"

#include "ext/singleton.hpp"

#include "ios/fork.hpp"
#include "ios/streambuf.hpp"

#include "log/logging.hpp"

// XXX: this is extra stupid because I hate god
namespace opt {

namespace {
    struct LogData_ {
        _R<api::FileWriter> file_;
        _S<ios::ForkedWriter> fork_;
        _S<ios::WriterBuffer> forkbuf_;
        _S<ios::FormatWriter> log_;

        LogData_() :
            forkbuf_(&fork_),
            log_(forkbuf_)
        {
        }
    };

    class PerProcess :
        public app::PerProcess
    {
      public:
        virtual void Start() {
            OpenThreadLog();
        }

        virtual void Stop() {
        }
    } perprocess_;
}

// XXX: warning: dereferencing pointer `anonymous' does break strict-aliasing rules
void _() {
    static LogData_ _;
}

ios::PrintWriter &Thread() {
    return _1<LogData_>().log_;
}

LOG_DECLARE void OpenThreadLog() {
    LogData_ &data(_1<LogData_>());

    if (_C("menes/log/targets/@file", true)) {
        data.file_ = new _H<api::FileWriter>(_S<ios::String>() << api::GetExecutablePath().GetParent().GetPath() << "/Error.log");
        data.fork_.Insert(data.file_);
    }

    if (_C("menes/log/targets/@debugger", false))
        data.fork_.Insert(&api::Cdbg);

    if (_C("menes/log/targets/@stderr", false))
        data.fork_.Insert(&api::Cerr);
}

}
