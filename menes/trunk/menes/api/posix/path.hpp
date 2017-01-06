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

#ifndef MENES_API_POSIX_PATH_HPP
#define MENES_API_POSIX_PATH_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/posix/error.hpp"

#include "ext/list.hpp"
#include "cse/string.hpp"

#include <sys/types.h>

namespace api {
namespace Posix {

class Path {
  private:
    cse::String path_;

    mode_t GetMode_() const;

  public:
    Path();
    Path(const cse::String &value);

    Path GetParent() const;
    Path GetChild(const cse::String &name) const;

    cse::String GetName() const;
    cse::String GetPath() const;

    bool IsEmpty() const;

    Path &operator =(const cse::String &rhs);

    Path ReadLink() const;

    Path GetRealPath() const;

    void CreateDirectory();

    static cse::String GetSeparator();

    // XXX: fsize_t
    size_t GetSize() const;

    // XXX: time_t?!?
    time_t GetLastModified() const;

    bool Exists() const;

    bool IsDirectory() const;

    bool operator ==(const Path &rhs) const;
    bool operator !=(const Path &rhs) const;
    bool operator <(const Path &rhs) const;
};

API_DECLARE ios::PrintWriter &operator <<(ios::PrintWriter &out, const Path &path);

} }

#endif//MENES_API_POSIX_PATH_HPP
