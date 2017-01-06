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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/win32/memorymap.hpp"
#include "cse/utf16.hpp"

#include <windows.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Win32 {

MemoryMap::MemoryMap(const cse::String &path) :
    file_(NULL),
    map_(NULL),
    view_(NULL)
{
    file_.Mend(::CreateFileW(cse::Utf16String(path).NullTerminate(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
    if (file_ == INVALID_HANDLE_VALUE)
        throw Error();

    map_.Mend(::CreateFileMapping(file_, NULL, PAGE_READONLY, 0, 0, NULL));
    if (map_ == NULL)
        throw Error();

    view_ = ::MapViewOfFile(map_, FILE_MAP_READ, 0, 0, 0);
    if (view_ == NULL)
        throw Error();

    BY_HANDLE_FILE_INFORMATION info;
    if (FALSE == ::GetFileInformationByHandle(file_, &info))
        throw Error();

    size_ = info.nFileSizeLow;
}

MemoryMap::~MemoryMap() {
    if (FALSE == ::UnmapViewOfFile(view_))
        throw Error();
    map_.Clear();
    file_.Clear();
}

MemoryMap::ConstIterator MemoryMap::Begin() const {
    return reinterpret_cast<char *>(view_);
}

MemoryMap::Iterator MemoryMap::Begin() {
    return reinterpret_cast<char *>(view_);
}

MemoryMap::ConstIterator MemoryMap::End() const {
    return Begin() + GetSize();
}

MemoryMap::Iterator MemoryMap::End() {
    return Begin() + GetSize();
}

size_t MemoryMap::GetSize() const {
    return size_;
}

} }
