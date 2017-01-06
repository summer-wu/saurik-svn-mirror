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

#ifndef MENES_API_APR_MMAP_HPP
#define MENES_API_APR_MMAP_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/apr/error.hpp"
#include "api/apr/pool.hpp"

#include <apr_mmap.h>

namespace api {
namespace Apr {

class MemoryMap {
  public:
    typedef const char *ConstIterator;
    typedef char *Iterator;

  protected:
    Pool context_;

    apr_finfo_t info_;
    apr_file_t *file_;
    apr_mmap_t *mmap_;

  public:
    MemoryMap(const cse::String &path) :
        file_(NULL),
        mmap_(NULL)
    {
        CheckError(::apr_file_open(&file_, path.NullTerminate(), APR_READ, APR_UREAD | APR_GREAD, context_));
        CheckError(::apr_file_info_get(&info_, APR_FINFO_SIZE, file_));
        CheckError(::apr_mmap_create(&mmap_, file_, 0, static_cast<apr_size_t>(info_.size), APR_MMAP_READ, context_));
    }

    ~MemoryMap() {
        CheckError(::apr_mmap_delete(mmap_));
        CheckError(::apr_file_close(file_));
    }

    ConstIterator Begin() const {
        return reinterpret_cast<char *>(mmap_->mm);
    }

    Iterator Begin() {
        return reinterpret_cast<char *>(mmap_->mm);
    }

    ConstIterator End() const {
        return Begin() + GetSize();
    }

    Iterator End() {
        return Begin() + GetSize();
    }

    size_t GetSize() const {
        return info_.size;
    }
};

} }

#endif//MENES_API_APR_MMAP_HPP
