/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#include "api/win32/error.hpp"
#include "api/win32/handle.hpp"
#include "api/win32/pipe.hpp"
#include "api/win32/process.hpp"

#include "cse/utf16.hpp"

#include <wtypes.h>
#include <winbase.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

cse::String GetCommandLine_(const cse::String &executable, const _L<cse::String> &arguments) {
    bool space(ext::Contains(executable, ' '));
    _S<ios::String> line;

    line << (space ? "\"" : "") << executable << (space ? "\"" : "");

    _foreach (const _L<cse::String>, argument, arguments)
        line << " " <<  *argument;

    return line;
}

Process::Process(const cse::String &executable, const _L<cse::String> &arguments) {
    cse::String line(GetCommandLine_(executable, arguments));
    Pipe reader, writer;

    // XXX: do I need to Tear() these?...
    reader_ = reader.GetReader();
    writer_ = writer.GetWriter();

    STARTUPINFOW startup;
    ::memset(&startup, 0, sizeof(STARTUPINFOW));
    startup.cb = sizeof(STARTUPINFOW);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = writer.GetReadHandle();
    startup.hStdOutput = reader.GetWriteHandle();
    startup.hStdError = reader.GetWriteHandle();

    startup.lpReserved = NULL;
    startup.lpDesktop = NULL;

    PROCESS_INFORMATION process;

    // this const_cast is sketchy and possibly wrong; I think that it might actually add a '\0' to the string; the result, however, is harmless
    CheckError(::CreateProcessW(cse::Utf16String(executable).NullTerminate(), const_cast<wchar_t *>(cse::Utf16String(line).NullTerminate()), NULL, NULL, w32::False, NULL, NULL, NULL, &startup, &process));

    process_.Mend(process.hProcess);
    thread_.Mend(process.hThread);
}

int Process::Join() const {
    process_.Wait();

    // XXX: get return state?
    return 0;
}

} }
