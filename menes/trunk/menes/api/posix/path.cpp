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

#include "api/posix/path.hpp"

#include <cstdlib>

#include <sys/param.h>
#include <sys/stat.h>

#include <unistd.h>

namespace api {
namespace Posix {

mode_t Path::GetMode_() const {
    // ERR: error: `::' does not have a class or union named `stat'
    struct stat status;
    if (::stat(path_.NullTerminate(), &status) != 0)
        throw Error();
    return status.st_mode;
}

Path::Path() {
}

Path::Path(const cse::String &value) :
    path_(value)
{
}

Path Path::GetParent() const {
    return cse::String(path_.Begin(), ext::FindLastAll(path_, GetSeparator()));
}

Path Path::GetChild(const cse::String &name) const {
    return _S<ios::String>() << path_ << GetSeparator() << name;
}

cse::String Path::GetName() const {
    return cse::String(ext::FindLastAll(path_, GetSeparator()) + 1, path_.End());
}

cse::String Path::GetPath() const {
    return path_;
}

bool Path::IsEmpty() const {
    return path_.IsEmpty();
}

Path &Path::operator =(const cse::String &rhs) {
    path_ = rhs;
    return *this;
}

Path Path::ReadLink() const {
    ext::Buffer buffer(1024);

    _forever {
        int size(CheckError(::readlink(path_.NullTerminate(), buffer.Begin(), buffer.GetSize())));
        _assert(size >= 0);

        if (static_cast<size_t>(size) < buffer.GetSize()) {
            buffer.SetSize(size);
            return Path(buffer);
        }

        buffer.SetSize(buffer.GetSize() * 2);
    }
}

Path Path::GetRealPath() const {
    char real[PATH_MAX];

    // XXX: the man page explicitely tells me not to use this function ;(
    if (::realpath(path_.NullTerminate(), real) == NULL)
        throw Error();

    return Path(cse::String(real));
}

void Path::CreateDirectory() {
    CheckError(::mkdir(path_.NullTerminate(), 0777));
}

cse::String Path::GetSeparator() {
    return _B("/");
}

// XXX: fsize_t
size_t Path::GetSize() const {
    // ERR: error: `::' does not have a class or union named `stat'
    struct stat status;
    if (::stat(path_.NullTerminate(), &status) != 0)
        throw Error();
    return status.st_size;
}

// XXX: time_t?!?
time_t Path::GetLastModified() const {
    // ERR: error: `::' does not have a class or union named `stat'
    struct stat status;
    if (::stat(path_.NullTerminate(), &status) != 0)
        throw Error();
    return status.st_mtime;
}

bool Path::Exists() const {
    // ERR: error: `::' does not have a class or union named `stat'
    struct stat status;
    int error(::stat(path_.NullTerminate(), &status));

    if (error == 0)
        return true;
    else {
        int error(ApiTraits::GetLastError());
        if (error != ENOENT)
            throw Error(error);
        return false;
    }
}

bool Path::IsDirectory() const {
    return S_ISDIR(GetMode_());
}

bool Path::operator ==(const Path &rhs) const {
    return path_ == rhs.path_;
}

bool Path::operator !=(const Path &rhs) const {
    return path_ != rhs.path_;
}

bool Path::operator <(const Path &rhs) const {
    return path_ < rhs.path_;
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Path &path) {
    out << path.GetPath();
    return out;
}

} }
