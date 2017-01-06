/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "api/posix/error.hpp"
#include "app/simple.hpp"

#include <cstdlib>

#include <sys/types.h>
#include <sys/ptrace.h>

#include <sys/syscall.h>
#include <sys/wait.h>

#include <sys/user.h>

#include <unistd.h>

using api::Posix::CheckError;

cse::String ReadString(int child, uintptr_t begin) {
    ext::Buffer string;

    _forever {
        uintptr_t word(CheckError(::ptrace(PT_READ_D, child, reinterpret_cast<void *>(begin), 0)));
        const uint8_t *bytes(reinterpret_cast<const uint8_t *>(&word));

        _forall (unsigned, i, 0, sizeof(uintptr_t))
            if (uint8_t byte = bytes[i])
                string.InsertLast(byte);
            else
                return string;

        begin += sizeof(uintptr_t);
    }
}

cse::String ReadString(int child, uintptr_t begin, uintptr_t end) {
    ext::Buffer string;

    while (begin != end) {
        uintptr_t word(CheckError(::ptrace(PT_READ_D, child, reinterpret_cast<void *>(begin), 0)));
        const uint8_t *bytes(reinterpret_cast<const uint8_t *>(&word));

        _forall (unsigned, i, 0, sizeof(uintptr_t)) {
            if (uint8_t byte = bytes[i])
                string.InsertLast(byte);
            if (++begin == end)
                break;
        }
    }

    return string;
}

int Main(const app::Options &options) {
    if (int pid = CheckError(::fork())) {
        _forever {
            int status, child(::waitpid(pid, &status, 0));

            _assert(child == pid);
            if (!WIFSTOPPED(status))
                return 0;

            _assert(WSTOPSIG(status) == SIGTRAP);

            ::user_regs_struct registers;
            CheckError(::ptrace(PT_GETREGS, child, 0, &registers));

            /*if (registers.eax == -ENOSYS) {
                if (registers.orig_eax == SYS_open) {
                    //cse::String path(GetUserString(child, registers.ebx));
                    //api::Cout << registers.orig_eax << " (" << path << ")" << ios::NewLine;
                }
            }*/

            CheckError(::ptrace(PT_SYSCALL, child, 0, 0));
        }
    } else {
        CheckError(::ptrace(PT_TRACE_ME, 0, 0, 0));

        ext::Vector<const char *> argv;
        _foreach (const app::ArgumentList, argument, options.GetArguments())
            argv.InsertLast(argument->NullTerminate());

        ::execvp(options[0]->NullTerminate(), const_cast<char **>(argv.NullTerminate()));
    }

    return 0;
}
