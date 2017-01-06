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

#ifndef MENES_API_POSIX_MEMORYMAP_HPP
#define MENES_API_POSIX_MEMORYMAP_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/posix/error.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

namespace api {
namespace Posix {

class MemoryMap {
  public:
    typedef const char *ConstIterator;
    typedef char *Iterator;

  protected:
    int file_;
    void *start_;
    size_t size_;

  public:
    MemoryMap(const cse::String &path) :
        file_(-1)
    {
        file_ = ::open(path.NullTerminate(), O_RDONLY);
        if (file_ == -1)
            throw Error();

        struct stat stats;
        if (::fstat(file_, &stats) == -1)
            throw Error();
        size_ = stats.st_size;

        start_ = ::mmap(NULL, size_, PROT_READ, MAP_SHARED, file_, 0);
        if (start_ == reinterpret_cast<void *>(-1) /* XXX: MAP_FAILED */)
            throw Error();
    }

    ~MemoryMap() {
        if (file_ == -1)
            return;
        if (start_ != NULL && -1 == ::munmap(start_, size_))
            throw Error();
        if (-1 == ::close(file_))
            throw Error();
        file_ = -1;
    }

    ConstIterator Begin() const {
        return reinterpret_cast<char *>(start_);
    }

    Iterator Begin() {
        return reinterpret_cast<char *>(start_);
    }

    ConstIterator End() const {
        return Begin() + GetSize();
    }

    Iterator End() {
        return Begin() + GetSize();
    }

    size_t GetSize() const {
        return size_;
    }
};

} }

#endif//MENES_API_POSIX_MEMORYMAP_HPP
