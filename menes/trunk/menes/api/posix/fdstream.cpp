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

#include "api/posix/error.hpp"
#include "api/posix/fdstream.hpp"

#include <unistd.h>

namespace api {
namespace Posix {

void HandleReader::Flush() {
}

size_t HandleReader::Read(char *data, size_t length) {
    OPT_TIMER("api::Posix::HandleReader::Read()")

    ssize_t size(::read(GetHandle_(), data, length));
    if (size == -1)
        throw Error();
    return size;
}

size_t HandleWriter::Write(const byte_t *data, size_t length) {
    OPT_TIMER("api::Posix::HandleWriter::Write()")

    int handle(GetHandle_());
    ssize_t size(::write(handle, data, length));
    if (size == -1)
        throw Error();
    return size;
}

void HandleWriter::Flush() {
    int handle(GetHandle_());
    if (handle == -1)
        return;
    /*if (-1 == ::fsync(handle))
        if (ApiTraits::GetLastError() != EINVAL)
            throw Error();*/
}

} }
