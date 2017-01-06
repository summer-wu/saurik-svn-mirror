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

#ifndef MENES_API_WIN32_PATH_HPP
#define MENES_API_WIN32_PATH_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/win32/error.hpp"

#include "cse/string.hpp"
#include "cse/utf16.hpp"

#include <wtypes.h>
#include <winbase.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

class Path {
  private:
    cse::String path_;

  public:
    Path() {
    }

    Path(const cse::String &value) :
        path_(value)
    {
    }

    Path GetParent() const {
        return cse::String(path_.Begin(), ext::FindLastAll(path_, GetSeparator()));
    }

    Path GetChild(const cse::String &name) const {
        return _S<ios::String>() << path_ << GetSeparator() << name;
    }

    cse::String GetName() const {
        return cse::String(ext::FindLastAll(path_, GetSeparator()) + 1, path_.End());
    }

    cse::String GetPath() const {
        return path_;
    }

    bool IsEmpty() const {
        return path_.IsEmpty();
    }

    Path &operator =(const cse::String &rhs) {
        path_ = rhs;
        return *this;
    }

    void CreateDirectory() {
        CheckError(::CreateDirectoryW(cse::Utf16String(path_).NullTerminate(), NULL));
    }

    static cse::String GetSeparator() {
        return _B("\\");
    }

    bool operator ==(const Path &rhs) const {
        return path_ == rhs.path_;
    }

    bool operator !=(const Path &rhs) const {
        return path_ != rhs.path_;
    }

    bool operator <(const Path &rhs) const {
        return path_ < rhs.path_;
    }
};

inline ios::PrintWriter &operator <<(ios::PrintWriter &out, const Path &path) {
    out << path.GetPath();
    return out;
}

} }

#endif//MENES_API_WIN32_PATH_HPP
