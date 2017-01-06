/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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
#include "api/win32/stacktrace.hpp"

#include "app/setup.hpp"

#include <wtypes.h>
#include <dbghelp.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

namespace {
class PerProcess :
    public app::PerProcess
{
  public:
    virtual void Start() {
        char file[MAX_PATH];
        int idx(::GetModuleFileNameA(::GetModuleHandle(NULL), file, MAX_PATH));

        while(file[idx] != '\\' && idx > 0) idx--;
        file[idx] = 0;

        if (FALSE == ::SymInitialize(::GetCurrentProcess(), file, TRUE))
            throw Error();
    }

    virtual void Stop() {
        /* // XXX: this didn't work or something
        if (FALSE == ::SymCleanup(::GetCurrentProcess()))
            throw Error(); */
    }
} perprocess_; }

StackTrace::StackTrace(int depth) {
    STACKFRAME frame;
    ::memset(&frame, 0, sizeof(STACKFRAME));

    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;

    // or use CONTEXT's Eip
    __asm {
        call label
        label: pop frame.AddrPC.Offset
        mov frame.AddrStack.Offset,esp
        mov frame.AddrFrame.Offset,ebp
    }

    HANDLE hProcess(::GetCurrentProcess());

    for (int i(0); i != depth; ++i) {
        if (FALSE == ::StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, ::GetCurrentThread(), &frame, NULL, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL)) {
            if (ApiTraits::GetLastError() != ERROR_SUCCESS)
                throw Error();
            break;
        }

        if (i != 0)
            frames_.InsertLast(frame.AddrPC.Offset);
    }
}

void StackTrace::Pop(unsigned number) {
    for (unsigned i(0); i != number; ++i) {
        if (frames_.IsEmpty())
            return;
        frames_.RemoveAt(frames_.Begin());
    }
}

size_t StackTrace::GetSize() const {
    return frames_.GetSize();
}

bool StackTrace::IsEmpty() const {
    return GetSize() == 0;
}

bool StackTrace::operator <(const StackTrace &rhs) const {
    return frames_ < rhs.frames_;
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const StackTrace &trace) {
    HANDLE hProcess(::GetCurrentProcess());

    for (StackTrace::FrameList_::ConstIterator frame(trace.frames_.Begin()); frame != trace.frames_.End(); ++frame) {
        if (frame != trace.frames_.Begin())
            out << ios::NewLine;

        {
            IMAGEHLP_MODULE module;
            ::memset(&module, 0, sizeof(IMAGEHLP_MODULE));
            module.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

            if (TRUE == ::SymGetModuleInfo(hProcess, *frame, &module))
                out << "[" << module.ModuleName << "] ";
        }

        char buff[sizeof(SYMBOL_INFO) + 1024];
        ::memset(&buff, 0, sizeof(buff));

        PSYMBOL_INFO symbol(reinterpret_cast<PSYMBOL_INFO>(buff));
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = sizeof(buff) - sizeof(SYMBOL_INFO);

        DWORD64 place(0);
        if (TRUE == ::SymFromAddr(hProcess, *frame, &place, symbol)) {
            IMAGEHLP_LINE64 line;
            ::memset(&line, 0, sizeof(IMAGEHLP_LINE64));
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

            out << symbol->Name << "(";

            DWORD place(0);
            if (TRUE == ::SymGetLineFromAddr64(hProcess, *frame, &place, &line))
                out << line.FileName << ":" << line.LineNumber;

            out << ")";
        }
    }

    return out;
}

} }
