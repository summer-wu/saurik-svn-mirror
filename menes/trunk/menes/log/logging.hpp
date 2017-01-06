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

#ifndef MENES_LOG_LOGGING_HPP
#define MENES_LOG_LOGGING_HPP

#include "cxx/platform.hpp"
#include "log/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/printwriter.hpp"

// XXX: this is extra stupid because I hate god
namespace opt {

enum Level {
    TraceLev   = 0x01,
    DebugLev   = 0x02,
    InfoLev    = 0x04,
    WarningLev = 0x08,
    ErrorLev   = 0x10,
    FatalLev   = 0x20,
    // XXX: Assert?

    NumberL  = 6
};

_finline ios::PrintWriter &Null(unsigned level = 0) {
    return *reinterpret_cast<ios::PrintWriter *>(NULL);
}

ios::PrintWriter &Trace(unsigned level = 0);
ios::PrintWriter &Debug(unsigned level = 0);
ios::PrintWriter &Info(unsigned level = 0);
ios::PrintWriter &Warning(unsigned level = 0);
ios::PrintWriter &Error(unsigned level = 0);
ios::PrintWriter &Fatal(unsigned level = 0);

void OpenLogs();

LOG_DECLARE ios::PrintWriter &Thread();
LOG_DECLARE void OpenThreadLog();

}

#ifdef LOG_THREADLOG
#define LOG_THREAD opt::Thread
#else
#define LOG_THREAD if (true); else opt::Null
#endif

#endif//MENES_LOG_LOGGING_HPP
