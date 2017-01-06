/* Menes - C++ High-Level Utility Library
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "app/entrypoint.hpp"

// XXX: this file is everything I hate about most peoples' software

#ifdef _WIN32
#include "abu/win32/module.hpp"

#include <windows.h>
#include "api/win32/undefine.hpp"
#else
//#include <unistd.h>
//#include <signal.h>
//#include <sys/syscall.h>
#endif

/*#ifndef _WIN32
class SignalException :
    public ext::Exception
{
};

static void SignalHandler_(int, siginfo_t *, void *context) {
    ucontext_t *ucontext(reinterpret_cast<ucontext_t *>(context));
    throw SignalException();
}
#endif*/

int _w32cdecl main(int argc, const char *argv[]) {
#if defined(_WIN32)
    abu::Win32::be::SetModule_(::GetModuleHandle(NULL));
#elif 0
    struct old_i386_kernel_sigaction {
        void (*k_sa_handler) (int);
        unsigned long k_sa_mask;
        unsigned long k_sa_flags;
        void (*sa_restorer) (void);
    };

    struct old_i386_kernel_sigaction _new, old;
    ::memset(&_new, 0, sizeof(_new));
    //_new.sa_sigaction = &SignalHandler_;
    //_new.sa_flags = SA_SIGINFO;
    _new.k_sa_handler = reinterpret_cast<void (*)(int)>(&SignalHandler_);
    _new.k_sa_mask = 0x0;
    _new.k_sa_flags = 0x4000000;
    _new.sa_restorer = NULL;
    api::Posix::CheckError(syscall(SYS_sigaction, SIGSEGV, &_new, &old));
#endif
    return app::be::AppMain_(argc, argv);
}
