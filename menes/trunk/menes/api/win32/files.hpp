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

#ifndef MENES_WIN32_FILES_HPP
#define MENES_WIN32_FILES_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/win32/error.hpp"
#include "api/win32/handle.hpp"

#include "ios/streams.hpp"

#include <wtypes.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

class HandleReader :
    public ios::Reader
{
  protected:
    virtual HANDLE GetHandle_() = 0;

  public:
    virtual size_t Read(char *data, size_t length) {
        OPT_TIMER("api::Win32::HandleWriter::Read()")
        DWORD read;
        CheckError(::ReadFile(GetHandle_(), data, static_cast<DWORD>(length), &read, NULL));
        return read;
    }
};

class HandleWriter :
    public ios::Writer
{
  protected:
    virtual HANDLE GetHandle_() = 0;

  public:
    virtual size_t Write(const byte_t *data, size_t length) {
        OPT_TIMER("api::Win32::HandleWriter::Write()")
        DWORD writ;
        CheckError(::WriteFile(GetHandle_(), data, static_cast<DWORD>(length), &writ, NULL));
        return writ;
    }

    // XXX: doesn't this comment mean it doesn't work?!?
    virtual void Flush() {
        OPT_TIMER("api::Win32::HandleWriter::Flush()")
        //CheckError(::FlushFileBuffers(GetHandle_()));
    }
};

class HandleStream :
    public HandleReader,
    public HandleWriter
{
};

template <typename Mixed_, DWORD Access_, DWORD Disposition_>
class FileStream :
    public HandleHolder<Mixed_>
{
  private:
    typedef HandleHolder<Mixed_> Base_;

  public:
    FileStream(const cse::String &path) {
        OPT_TIMER("api::Win32::FileStream()")
        // XXX: should be using CreateFileW
        HANDLE handle(::CreateFileA(path.NullTerminate(), Access_, FILE_SHARE_READ, NULL, Disposition_, FILE_ATTRIBUTE_NORMAL, NULL));
        if (handle == INVALID_HANDLE_VALUE)
            throw Error();
        Base_::handle_.Mend(handle);
    }
};

typedef FileStream<HandleReader, GENERIC_READ, OPEN_EXISTING> FileReader;
typedef FileStream<HandleWriter, GENERIC_WRITE, CREATE_ALWAYS> FileWriter;
//typedef FileStream<HandleStream, GENERIC_READ | GENERIC_WRITE, CREATE_ALWAYS> FileStream;

} }

#endif//MENES_WIN32_FILES_HPP
