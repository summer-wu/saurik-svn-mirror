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

#ifndef MENES_API_POSIX_FILES_HPP
#define MENES_API_POSIX_FILES_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/posix/fdstream.hpp"
#include "api/posix/handle.hpp"

#include "cse/string.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace api {
namespace Posix {

template <typename Mixed_, int Flags_, int Mode_>
class FileStream :
    public HandleHolder<Mixed_>
{
  private:
    typedef HandleHolder<Mixed_> Base_;

  public:
    FileStream(const cse::String &path, int flags = Flags_, mode_t mode = Mode_) {
        Base_::handle_.Mend(CheckError(::open(path.NullTerminate(), flags, mode)));
    }

    ~FileStream() {
        // XXX: is this neccessary?
        Base_::Flush();
    }
};

typedef FileStream<HandleReader, O_RDONLY, 0> FileReader;
typedef FileStream<HandleWriter, O_WRONLY | O_CREAT | O_TRUNC, 0666> FileWriter;
//typedef FileStream<HandleStream, O_RDWR, 0> FileStream;

} }

#endif//MENES_API_POSIX_FILES_HPP
