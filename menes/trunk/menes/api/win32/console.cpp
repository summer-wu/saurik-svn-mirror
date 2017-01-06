/* Menes - C++ High-Level Utility Library
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

#include "api/win32/console.hpp"
#include "cse/utf16.hpp"

namespace api {
namespace Win32 {

// XXX: move this somewhere more useful
template <DWORD Handle_, typename Base_>
class StandardHandle :
    public HandleHolder<Base_>
{
  public:
    StandardHandle() {
        Reset();
    }

    void Reset() {
        HANDLE handle(::GetStdHandle(Handle_));
        if (handle == INVALID_HANDLE_VALUE)
            throw Error();
        handle_ = handle;
    }
};

namespace {
    _S< StandardHandle<STD_INPUT_HANDLE, ConsoleReader> > StdIn;
    _S< StandardHandle<STD_OUTPUT_HANDLE, ConsoleWriter> > StdOut;
    _S< StandardHandle<STD_ERROR_HANDLE, ConsoleWriter> > StdErr;
}

API_DECLARE _S<ConsoleBuffer> Console;

API_DECLARE ios::Reader &Cin(StdIn);
API_DECLARE _S<ios::FormatWriter> Cout(StdOut);
API_DECLARE _S<ios::FormatWriter> Cerr(StdErr);

// XXX: this is not Unicode compliant
size_t ConsoleReader::Read(char *data, size_t length) {
    OPT_TIMER("api::Win32::ConsoleReader::Read()")
    w32::DWord read;
    CheckError(::ReadConsoleA(GetHandle_(), data, static_cast<w32::DWord>(length), &read, NULL));
    return read;
}

size_t ConsoleWriter::Write(const byte_t *data, size_t length) {
    OPT_TIMER("api::Win32::ConsoleWriter::Write()")
    DWORD writ;
    CheckError(::WriteConsoleA(GetHandle_(), data, static_cast<w32::DWord>(length), &writ, NULL));
    return writ;
}

// XXX: doesn't this comment mean it doesn't work?!?
void ConsoleWriter::Flush() {
    OPT_TIMER("api::Win32::ConsoleWriter::Flush()")
    //CheckError(::FlushFileBuffers(GetHandle_()));
}

void Screen::Select() {
    CheckError(::SetConsoleActiveScreenBuffer(GetHandle_()));
}

void Screen::Close() {
    handle_.Clear();
}

void ConsoleBuffer::Open_(const wchar_t *name, Handle &handle) {
    // XXX: insufficient; doesn't handle non-console app case
    HANDLE value(::CreateFileW(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
    if (value == INVALID_HANDLE_VALUE) 
        throw Error();
    handle.Mend(value);
}

ConsoleBuffer::ConsoleBuffer() {
    Reset();
}

void ConsoleBuffer::Reset() {
    Open_(L"CONIN$", HandleHolder<ConsoleReader>::handle_);
    Open_(L"CONOUT$", Screen::handle_);
}

void ConsoleBuffer::Allocate() {
    CheckError(::AllocConsole());
    Reset();
}

void ConsoleBuffer::Free() {
    Close();
    CheckError(::FreeConsole());
}

void ConsoleBuffer::SetTitle(const cse::String &title) {
    CheckError(::SetConsoleTitleW(cse::Utf16String(title).NullTerminate()));
}

ScreenBuffer::ScreenBuffer() {
    HANDLE handle(::CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL));
    if (handle == INVALID_HANDLE_VALUE)
        throw Error();
    handle_.Mend(handle);
}

} }
