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

#include "alg/logbase2.hpp"

#include "api/exename.hpp"
#include "api/files.hpp"

#include "abu/module.hpp"
#include "ext/singleton.hpp"

#include "ios/fork.hpp"
#include "ios/streambuf.hpp"

#include "log/logging.hpp"

// XXX: this is stupid because I hate it
namespace opt {

namespace be {
namespace {

const char *Levels_[] = {
    "Trace",
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Fatal",
    NULL
};

struct LogData_ {
    _S<api::FileWriter> projstr;
    _S<ios::WriterBuffer> projbuf;
    _S<ios::FormatWriter> projlog;

    _S<ios::ForkedWriter> forkstr;
    _S<ios::WriterBuffer> forkbuf;
    _S<ios::FormatWriter> forklog;

    LogData_() :
        projstr(_S<ios::String>() << api::GetExecutablePath().GetParent().GetPath() << "/" << abu::GetModuleName() << ".log"),
        projbuf(&projstr),
        projlog(projbuf),
        forkbuf(&forkstr),
        forklog(forkbuf)
    {
    }
};

ios::PrintWriter &Log_(Level level, unsigned priority) {
    LogData_ &data(_1<LogData_>());

    /*size_t index(alg::ilog2(uint32_t(level)));
    _assert(index < NumberL);

    log << "[" << Levels_[index][0] << "/";
    log << "00:00:00";
    log << "] ";
    log.flush();

    std::ostream &thread(Thread());
    thread << "(" << app::GetModuleName() << ") ";
    thread.flush();*/

    return level == TraceLev ? data.projlog : data.forklog;
}

} }

#define MENES_LOG_LOG_LEVEL(Level) \
ios::PrintWriter &Level(unsigned priority) { \
    return be::Log_(Level ## Lev, priority); \
}

MENES_LOG_LOG_LEVEL(Trace)
MENES_LOG_LOG_LEVEL(Debug)
MENES_LOG_LOG_LEVEL(Info)
MENES_LOG_LOG_LEVEL(Warning)
MENES_LOG_LOG_LEVEL(Error)
MENES_LOG_LOG_LEVEL(Fatal)

#undef MENES_LOG_LOG_LEVEL

void OpenLogs() {
    be::LogData_ &data(_1<be::LogData_>());
    data.forkstr.Insert(&Thread());
    data.forkstr.Insert(&data.projstr);
}

}
